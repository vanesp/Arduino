/*
  Water-Detector
  
  Detect water with a little circuit
  
  2N390 NPN transistor
  E (pin 2) to ground
  C (pin 3) is output to waterPin, and 10KOhm to +5V
  B (pin 2) through 10K Ohm to sensor 1
  Sensor 2 to +5V
  When water = conductive = +ve voltage to base = collector goes to ground = LOW on waterPin

  Additionally, a DS18S20 temperature sensor is connected
  (flat towards you, pins 1,2,3 from left to right)
  pin 1 to ground
  pin 2 to digital ip (port 10) and via 4K7 resistor to +5V
  pin 3 to VCC (+5V)
  
  

  modified 7 november 2014
  by Peter van Es
 */

#include <OneWire.h>

// constants won't change. They're used here to 
// set pin numbers:
const int waterPin = 3;    // the number of the water pin
const int ledPin = 9;      // the number of the LED pin
OneWire  ds(10);           // on pin 10 with 4K7 to vcc

// Variables will change:
int ledState = LOW;         // the current state of the output pin
int waterState = LOW;             // the current reading from the input pin
int lastwaterState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(waterPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);
  // debugging
  Serial.begin(9600);
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(waterPin);

  // check to see if you just detected water 
  // Note: 1 = water, 0 = no water
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastwaterState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the water state has changed:
    if (reading != waterState) {
      waterState = reading;

      // only toggle the LED if the new water state is HIGH
      if (waterState == LOW) {
        ledState = HIGH;
        // and generate an alert cause you are gonna get wet feet
      } else {
        ledState = LOW;
      }
    }
  }
  
  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastwaterState:
  lastwaterState = reading;
}
