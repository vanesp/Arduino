
// Detect IT+ devices
// 2012-12-16 <vanes.peter@gmail.com>
// http://opensource.org/licenses/mit-license.php

// LaCrosse IT+ Sensor readings added with thanks to
// Gérard Chevalier, Nov 2011
// IT+ decoding was possible thank to
//   - The great job done by fred, see here: http://fredboboss.free.fr/tx29/tx29_1.php?lang=en
//   - And stuff found here: http://forum.jeelabs.net/node/110

#include <JeeLib.h>
#include <Arduino.h>
#include <RF12.h>
#include <Ports.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// IT+ Decoding debug flags
#define ITPLUS_DEBUG_FRAME
// #define ITPLUS_DEBUG
// #define DEBUG_CRC
// #define RF12_DEBUG

#define SENSORS_RX_TIMEOUT 5        // in minutes?
#define ITPLUS_MAX_SENSORS  10
#define ITPLUS_MAX_DISCOVER  ITPLUS_MAX_SENSORS
#define ITPLUS_DISCOVERY_PERIOD 255

#define ITPLUS_ID_MASK	0b00111111
#define REPORT_EVERY   1   // report every N measurement cycles, a cycle takes a minute

// LaCrosse Radio Sensor structure (for IT+)
typedef struct {
  byte SensorID;
  byte LastReceiveTimer;
  byte Temp, DeciTemp;
  byte Humidity;
} Type_Channel;

// Data definitions
Type_Channel ITPlusChannels[ITPLUS_MAX_SENSORS];
// External definitions from RF12.h
extern boolean ITPlusFrame;
extern void rf12_initialize_overide_ITP ();

// payload data, as sent over the serial port

typedef struct {
    uint16_t count;  // counter, wraps after reaching 4095
    uint8_t value;  // current sensor value (only 0 or 1 used)
    uint8_t wrap;   // set if the counter has wrapped at least once
    uint8_t fresh; // set if this count is a new value
    uint16_t rate;   // number of milliseconds since last change
} TxItem;

MilliTimer milliTimer; // poll sensor every millisecond
MilliTimer sendTimer;  // retransmit the payload buffer every 3s
#define TIMER 60000      // Nr of milliseconds in main timing loop = 60 secs
#define TEMP  5          // send temperature every 5 minute (5 loops)

// Pulse counting settings
long startTime, endTime;  // counters for millis
int power, elapsedWh;     // power and energy
int ppwh = 1;             // 1000 pulses/kwh = 1 pulse per wh - Number of pulses per wh - found or set on the meter.

static byte myNodeID;       // node ID used for this unit
byte    j;
byte    ActualSensors;      // Actual number of sensors
int     Temperature;

uint8_t txrequest;          // set whenever there is new data to send
int     tempLoop;

// stack of subroutines for processing LaCrosse data

#define CRC_POLY 0x31
boolean CheckITPlusCRC(byte *msge, byte nbBytes) {
  byte reg = 0;
  short int i;
  byte curByte, curbit, bitmask;
  byte do_xor;

  while (nbBytes-- != 0) {
    curByte = *msge++; bitmask = 0b10000000;
    while (bitmask != 0) {
      curbit = ((curByte & bitmask) == 0) ? 0 : 1;
      bitmask >>= 1;
      do_xor = (reg & 0x80);
      reg <<=1;
      reg |= curbit;
      if (do_xor) {
        reg ^= CRC_POLY;
      }
    } // while bitmask
  } // while nbBytes
  return (reg == 0);
}

void printHex(byte data) {
  if (data < 16)
    Serial.print('0');
  Serial.print(data, HEX);
}

void ProcessITPlusFrame() {
  byte Temp, DeciTemp, SensorId, ResetFlag, Channel, Hygro, lcSensorId;

  // Here it is possible that the frame just received is an IT+ one (flag ITPlusFrame set), but not sure.
  // So, check CRC, and decode if OK.
#ifdef ITPLUS_DEBUG_FRAME
  Serial.println("GotIT+");
  for(uint8_t i = 0; i < 5; i++) {
      printHex(rf12_buf[i]);
      Serial.print(' ');
  }
  Serial.println();
#endif

  if (CheckITPlusCRC((byte *)rf12_buf, 5)) {
      // OK, CRC is valid, we do have an IT+ valid frame
      SensorId = ((rf12_buf[0] & 0x0f) << 4) + ((rf12_buf[1] & 0xf0) >> 4) >> 2;
      lcSensorId = (((rf12_buf[0] & 0x0f) << 4) + ((rf12_buf[1] & 0xf0) >> 4)) & 0xfc;
      // Reset flag is stored as bit #6 in sensorID.
      ResetFlag = (rf12_buf[1] & 0b00100000) << 1;
      // Sign bit is stored into bit #7 of temperature. IT+ add a 40° offset to temp, so < 40 means negative
      Temp = (((rf12_buf[1] & 0x0f) * 10) + ((rf12_buf[2] & 0xf0) >> 4));
      DeciTemp = rf12_buf[2] & 0x0f;
      if (Temp >= 40) {
          Temp -= 40;
      } else {
          if (DeciTemp == 0) {
             Temp = 40 - Temp;
          } else {
             Temp = 39 - Temp;
             DeciTemp = 10 - DeciTemp;
          }
          Temp |= 0b10000000;
      }
      Hygro = rf12_buf[3] & 0x7f;

#ifdef ITPLUS_DEBUG
      Serial.print("Id: "); printHex(SensorId);
      Serial.print(" - lcId: "); printHex(lcSensorId);
      if (ResetFlag)
          Serial.print(" R");
      else
          Serial.print("  ");
      Serial.print(" Temp: ");
      if (Temp & 0b10000000)
          Serial.print("-");
      if (Temp < 10)
          Serial.print("0");
      Serial.print(Temp & 0x7f, DEC); Serial.print("."); Serial.print(DeciTemp, DEC);
      if (Hygro != 106) {
          Serial.print(" Hygro: "); Serial.print(Hygro, DEC); Serial.print("%");
       }
       Serial.println();
#endif

       // Process received measures (only if sensor is registered)
      if ((Channel = CheckITPlusRegistration((SensorId | ResetFlag), Temp, DeciTemp, Hygro)) != 0xff) {
          ITPlusChannels[Channel].Temp = Temp;
          ITPlusChannels[Channel].DeciTemp = DeciTemp;
          ITPlusChannels[Channel].Humidity = Hygro;
       }
    } else {
#ifdef ITPLUS_DEBUG_FRAME
       Serial.println("BadCRC");
#endif
    }
}

// Find an IT+ ID into the registered IDs table. If found, return the index in table
// Bit 6 of ID is the "Sensor Reseted" indicator, meaning the battery was replaced and a new
// ID was generated. This flag is held on for about 4h30mn, enabling sensor / receiver peering.
// This function should be passed the "raw ID", including the flag (in bit #6) in order to store it
// into the discovered table with the sensor id to distinguish lists in display later on.
// If the ID is not found, the sensor is added to the discovered IDs table (if not already there).
// When ID not found, return 0xff
byte CheckITPlusRegistration(byte id, byte Temp, byte DeciTemp, byte Humid) {
    byte i, FreeIndex;
    unsigned int MaxTime;

    for (i = 0; i < ITPLUS_MAX_SENSORS; i++) {
        if (ITPlusChannels[i].SensorID == (id & ITPLUS_ID_MASK)) {  // Do the search without reset flag
            // OK Found, reset receive timer & return channel = index
            ITPlusChannels[i].LastReceiveTimer = SENSORS_RX_TIMEOUT;
            return i;
        }
     }

     // Not found: insert into a free slot, including the reset flag.
     for (i = 0; i < ITPLUS_MAX_SENSORS; i++) {	// Find a free slot
         if (ITPlusChannels[i].SensorID == 0xff) {
             ITPlusChannels[i].SensorID = id;
             ITPlusChannels[i].LastReceiveTimer = ITPLUS_DISCOVERY_PERIOD;
             return i;
         }
     }

     // No free slot found. Use the one with oldest receiving time.
     MaxTime = ITPLUS_DISCOVERY_PERIOD;
     FreeIndex = 0;
     for (i = 0; i < ITPLUS_DISCOVERY_PERIOD; i++) {
         if (ITPlusChannels[i].LastReceiveTimer < MaxTime) {
             MaxTime = ITPlusChannels[i].LastReceiveTimer;
             FreeIndex = i;
         }
  }
  ITPlusChannels[FreeIndex].SensorID = id;
  ITPlusChannels[FreeIndex].LastReceiveTimer = ITPLUS_DISCOVERY_PERIOD;
  return FreeIndex;
}



void setup () {
    Serial.begin(57600);
    Serial.println("\n[DetectIT]");
    // Set-up RF12 library
    myNodeID = rf12_initialize(1, RF12_868MHZ, 0xd4); // 0xd4 needed for IT+
    // Overide settings for RFM01/IT+ compliance
    rf12_initialize_overide_ITP();

    // rf12_initialize(1, RF12_868MHZ, 5); // 868 Mhz, net group 5, node 1
    // keep as set up
    // myNodeID = rf12_config();

    // Set-up the LaCrosse IT+ receiving sensor structure
    for (byte i = 0; i < ITPLUS_MAX_SENSORS; i++)
        ITPlusChannels[i].LastReceiveTimer = 0;

    tempLoop = TEMP - 1;        // ensure first report is soon
    startTime = millis();
    Serial.flush();
}



void loop () {
    int i;
    int Temp;
    byte Channel;

    if (rf12_recvDone()) {
           // If a "Receive Done" condition is signaled, we can safely use the RF12 library buffer up to the next call to
           // rf12_recvDone: RF12 tranceiver is held in idle state up to the next call.
           // Is it IT+ or Jeenode frame ?
           if (ITPlusFrame) {
               ProcessITPlusFrame();  // Keep IT+ logic outside this loop
           }
    }
    
    if (sendTimer.poll(TIMER)) {
        txrequest = 1;
        tempLoop++;
        // Decrement LastReceiveTimer for all channels every report period.
        for (Channel = 0; Channel < ITPLUS_MAX_SENSORS; Channel++) {
            if (ITPlusChannels[Channel].LastReceiveTimer != 0) ITPlusChannels[Channel].LastReceiveTimer--;
        }
    }

    // and send the LaCrosse data too...
    // DataStream 1 to ITPLUS_MAX_SENSORS are IT+ Sensors
    for (Channel = 0; Channel < ITPLUS_MAX_SENSORS; Channel++) {
        if (ITPlusChannels[Channel].SensorID != 0xff) {  // Send only if registered
           if (ITPlusChannels[Channel].LastReceiveTimer != 0) {  // Send only if valid temp received
              // first issue temperature, on a scale of -400 .. 1000 
              Temperature = ITPlusChannels[Channel].Temp * 10 + ITPlusChannels[Channel].DeciTemp;
              Serial.print("TMP ");
              Serial.print(ITPlusChannels[Channel].SensorID);  
              Serial.print (' ');
              Serial.println (Temperature);        
              Serial.flush();

              if (ITPlusChannels[Channel].Humidity != 106) {
                 // We have humidity
                 Serial.print("HUMI ");
                 Serial.print(ITPlusChannels[Channel].SensorID);  
                 Serial.print (' ');
                 Serial.println (ITPlusChannels[Channel].Humidity);        
                 Serial.flush();
               } // humidity
           } // only if valid data
        } // only if registered
    } // for loop

}
