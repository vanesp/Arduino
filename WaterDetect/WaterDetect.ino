/*
  Water-Detector
  
  Detect water with a little circuit
  
  2N390 NPN transistor
  E (pin 2) to ground
  C (pin 3) is output to waterPin, and 10KOhm to +5V
  B (pin 2) through 10K Ohm to sensor 1
  Sensor 2 to +5V
  When water = conductive = +ve voltage to base = collector goes to ground = LOW on waterPin

  modified 7 november 2014
  by Peter van Es
 */

// constants won't change. They're used here to 
// set pin numbers:
const int waterPin = 4;    // the number of the water pin (on D on P1 = PD4)
const int ledPin = 6;      // the number of the LED pin (D on P3 is PD6)

// Variables will change:
int ledState = LOW;         // the current state of the output pin
int waterState = LOW;             // the current reading from the input pin
int lastwaterState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

static void setLED (byte pin, byte on) {
  if (on) {
    // led is on, output mode, pulled low
    digitalWrite(pin, 0);
    pinMode(pin, OUTPUT);
  } else {
    // led = off, input mode with pul-up resistor
    pinMode (pin, INPUT);
    digitalWrite (pin, 1);
  }
}

void setup() {
  pinMode(waterPin, INPUT);

  // set initial LED state
  setLED(ledPin, ledState);
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
  setLED(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastwaterState:
  lastwaterState = reading;
}

