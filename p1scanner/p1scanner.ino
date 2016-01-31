/// @dir p1scanner
/// Parse P1 data from smart meter and send as compressed packet over RF12.
/// @see http://jeelabs.org/2013/01/02/encoding-p1-data/
// 2012-12-31 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Note: 
// Node=18
// On my gas meter, the gas value is field 2421
// See sample

/*
 /ISk5\2MT382-1004

0-0:96.1.1(5A424556303035313631363530363133)
1-0:1.8.1(02285.588*kWh)
1-0:1.8.2(02659.324*kWh)
1-0:2.8.1(00000.002*kWh)
1-0:2.8.2(00000.000*kWh)
0-0:96.14.0(0001)
1-0:1.7.0(0000.88*kW)
1-0:2.7.0(0000.00*kW)
0-0:17.0.0(0999.00*kW)
0-0:96.3.10(1)
0-0:96.13.1()
0-0:96.13.0()
0-1:24.1.0(3)
0-1:96.1.0(3238303131303031333038313439353133)
0-1:24.3.0(151017150000)(00)(60)(1)(0-1:24.2.1)(m3)
(05416.586)
0-1:24.4.0(1)
!
 */


// This sketch reads data from a Dutch smart meter that is equipped with a P1-port.
// Connections are made to port 4
// Connect 'RTS' from meter to P4+ (+5V)
// Connect 'GND' from meter to P4 GND
// Connect 'RxD' from meter to P4 DIO (which is Arduino Pin 7)

#include <JeeLib.h>
#include <SoftwareSerial.h>

// External definitions from RF12.h to allow IT+ communications
extern boolean ITPlusFrame;
extern void rf12_initialize_overide_ITP ();
static byte myNodeID;       // node ID used for this unit


#define DEBUG 0   // set to 1 to print to serial port
#define LED   6   // set to 0 to disable LED blinking
                  // Pin 6 is PD6 is Jeenode Port 3 D

#define FORMAT 1
#define NTYPES (sizeof typeMap / sizeof *typeMap)

// list of codes to be sent out (only compares lower byte!)
const byte typeMap [] = {
  181, 182, 281, 282, 96140, 170, 270, 2410, 2421, 2440
};

// Define port and inverted Logic. Port 7 = P4 DIO
SoftwareSerial mySerial (7, 17, true); // inverted logic

byte type;
uint32_t value;
uint32_t readings[NTYPES+1];
byte payload[5*NTYPES], *fill;

static bool p1_scanner (char c) {
  switch (c) {
    case ':':
      type = 0;
      value = 0;
      break;
    case '(':
      if (type == 0)
        type = value; // truncates to lower byte
      value = 0;
    case '.':
      break;
    case ')':
      if (type)
        return true;
      break;
    default:
      if ('0' <= c && c <= '9')
        value = 10 * value + (c - '0');
  }
  return false;
}

static void addValue (uint32_t v, byte mask =0x80) {
  uint32_t w = v >> 7;
  if (w)
    addValue(w, 0);
  *fill++ = (v & 0x7F) | mask;
}

static void collectData (bool empty =false) {
  fill = payload;
  addValue(FORMAT);
  if (!empty)
    for (byte i = 0; i < NTYPES; ++i)
      addValue(readings[i]);
  byte n = fill - payload;

  #if DEBUG
    for (byte i = 0; i < n; ++i) {
      Serial.print(' ');
      Serial.print(payload[i], HEX);
    }
    Serial.println();
  #endif
  
  rf12_sendNow(0, payload, n);
  rf12_sendWait(1);
}

void setup () {

  mySerial.begin(9600);
  // digitalWrite(7, 1); // enable pull-up

  #if LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
  #endif

  myNodeID = rf12_initialize(18, RF12_868MHZ, 0xd4); // 0xd4 needed for IT+
  // Overide settings for RFM01/IT+ compliance
  rf12_initialize_overide_ITP();

  #if DEBUG
    Serial.begin(57600);
    Serial.println("\n[p1scanner.18]");
  #endif

  collectData(true); // empty packet on power-up
}

void loop () {
  int c;

    c = mySerial.read();
    if (c > 0) {
      c &= 0x7F;
      // Serial.write(c);
    }
  
  switch (c) {
    case '/':
    if (LED)
        digitalWrite(LED, 0); // LED on
      break;
    case '!':
      collectData();
      memset(readings, 0, sizeof readings);
      if (LED)
        digitalWrite(LED, 1); // LED off
      break;
    default:
      if (p1_scanner(c)) {
        #if DEBUG
          Serial.print(type);
          Serial.print('=');
          Serial.println(value);
        #endif
        for (byte i = 0; i < NTYPES; ++i)
          if (type == typeMap[i]) {
            readings[i] = value;
            break;
          }
    }
  }
}
