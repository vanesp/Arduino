
/*
 * Door controller for Loxone
 * 
 * This program is for a Adafruit Huzzah ESP 8266 controller,
 * with a Pololu Basic 2-Channel SPDT Relay Carrier with 5VDC Relays 
 * These control door open / door close contacts
 * 
 * Documentation:
 * https://www.pololu.com/product/2485
 * https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/overview
 * Select Board AdaFruit Huzzah ESP8266
 * To program: 
 * 
 * Hold down the GPIO0 button, the red LED will be lit
 * While holding down GPIO0, click the RESET button
 * Release RESET, then release GPIO0
 * When you release the RESET button, the red LED will be lit dimly, this means its ready to bootload
 * 
 * Pololu screw terminals for relay (if on the left of the board)
 *
 * top:    normally close
 * middle: common
 * bottom: normally open
 * 
 * top relay = 1, enable 1
 * bottom relay = 2, enable 2
 * 
 * Power V+ connect to Pololu VDD
 * GND to Pololu GND
 * pin 13 is connected to Relay 1 (which is for open / up) 
 * pin 12 is connected to Relay 2 (which is for close / down)
 * pin 14 is a movement sensor. It is used to keep track of the state of the door or gate
 * states are: CLOSED, OPENING, OPEN, CLOSING
 * 
 * Control is via sending UDP commands to port 2807 of the IP address (which is linked to the MAC address and static)
 * 
 * UDP commands are:
 * up (to open or send the garage door up)
 * dn (to close or send the garage door down)
 * stateopen (to change the internal state of the controller)
 * stateclosed (also to change state, used in case the Loxone controller and device get out of sync)
 * 
 */

#define DEBUG 1
#define GARAGE  1      // or GARAGE
#define USETILT 0      // if we use a tilt sensor

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid     = "mozart-n";
const char* password = "12HaHeLi94BebCnof";

WiFiUDP UDPTestServer;
unsigned int UDPPort = 2807;
String MiniServer = "192.168.2.102"; // the remote IP address

// IP address is fixed in router against MAC address

const int packetSize = 128;
byte packetBuffer[packetSize];

#define UP  13   // is connected to UP relay
#define DN  12   // is connected to DOWN relay
#define TILT 14  // tilt sensor 
#define LED 0    // show a led, note, LED is inverted

#define OPEN    1
#define CLOSING 2
#define CLOSED  3
#define OPENING 4

char* replyBuffer []={"", "open", "closing", "closed", "opening"};

// Variables will change:
int doorState = CLOSED
;         // the current state of the door...
int tiltState;             // the current reading from the input pin
int lasttiltState = LOW;   // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

void setup() {

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  pinMode(DN, OUTPUT);
  pinMode(UP, OUTPUT);
  pinMode(TILT, INPUT);

  digitalWrite(DN, LOW);             // turn the relay off by making the voltage LOW
  digitalWrite(UP, LOW);             // turn the relay off by making the voltage LOW

  // And ensure we close the garage door
  digitalWrite(DN, HIGH);            // turn the relay on, and the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a bit
  digitalWrite(DN, LOW);             // turn the relay off by making the voltage LOW

#ifdef DEBUG
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.begin(ssid, password);

#ifdef GARAGE
  // deze waarde is ook gereserveerd voor 18:FE:34:F4:D4:71, ons MAC adres
  WiFi.config(IPAddress(192, 168, 2, 103), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));
#endif

#ifdef GATE
  // deze waarde is ook gereserveerd voor 18:FE:34:F4:D4:71, ons MAC adres
  WiFi.config(IPAddress(192, 168, 2, 105), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));
#endif
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG    
    Serial.print(".");
#endif
  }

#ifdef DEBUG    
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif

  UDPTestServer.begin(UDPPort);
  
}

int value = 0;
int duration = 0;

void loop() {
   handleUDPServer();
   delay(10);

#ifdef TILTSENSOR
  // read the state of the switch into a local variable:
  int reading = digitalRead(TILT);

  // check to see if you just pressed the tilt
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lasttiltState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the tilt state has changed:
    if (reading != tiltState) {
      tiltState = reading;

      // tilt state has changed, now we handle the door state
      switch (doorState) {
        // Finite State Machine
        case OPEN:
          doorState = CLOSING;
          break;
        case CLOSING:
          doorState = CLOSED;
          break;
        case CLOSED:
          doorState = OPENING;
          break;
        case OPENING:
          doorState = OPEN;
          break;
      }


    // send a reply, to the IP address and port that sent us the packet we received
    UDPTestServer.beginPacket(UDPTestServer.remoteIP(), UDPTestServer.remotePort());
    UDPTestServer.write(replyBuffer[doorState]);
    UDPTestServer.endPacket();

  #ifdef DEBUG      
      Serial.print("Sending doorState ");
      Serial.println(replyBuffer[doorState]);
  #endif      
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lasttiltState:
  lasttiltState = reading;
#endif
}

void handleUDPServer() {
  int cb = UDPTestServer.parsePacket();
  if (cb) {
    UDPTestServer.read(packetBuffer, packetSize);
    String myData = ""; 
    for(int i = 0; i < packetSize; i++) {
      myData += (char)packetBuffer[i];
    }
#ifdef DEBUG    
    Serial.println(myData);
#endif
    // mydata is: up or dn
    if (myData == "up") {
      digitalWrite(UP, HIGH);            // turn the relay on, and the LED on (HIGH is the voltage level)
      delay(500);                       // wait for a bit
      digitalWrite(UP, LOW);             // turn the relay off by making the voltage LOW
     }
    if (myData == "dn") {
      digitalWrite(DN, HIGH);            // turn the relay on, and the LED on (HIGH is the voltage level)
      delay(500);                       // wait for a bit
      digitalWrite(DN, LOW);             // turn the relay off by making the voltage LOW
     }

    if (myData == "stateopen") {
      doorState = OPEN;
     }
    if (myData == "stateclosed") {
      doorState = CLOSED;
     }

  }
}
