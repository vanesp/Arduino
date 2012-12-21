/*
  Blink_PvE
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#define LedG 13
#define LedR 6
#define LedY 9

// state definitions
#define flash 0
#define red   1
#define green 2
#define orange 3
#define dark  1
#define light  0

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot
int outside = dark;         // assume it is dark
int Duur = 1000;
int State = flash;
int Flash = 0;
int Time = 0;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(LedG, OUTPUT);     
  pinMode(LedR, OUTPUT);     
  pinMode(LedY, OUTPUT);
 // initialize serial communications at 9600 bps:
  Serial.begin(115200); 
}

void loop() {
  delay(Duur);
  // read the analog in value:
  sensorValue = analogRead(analogInPin);            
  // a sensor value over 300 means dark, under 300 means light
  // if it's dark, go to flashing, else, keep running
  if (sensorValue > 500) {
    outside = dark;
  } else {
    outside = light;
  }  
  // print the results to the serial monitor:
  Serial.print("state = " );                       
  Serial.print(State);      
  Serial.print("\t sensor = " );                       
  Serial.println(sensorValue);      
  
  switch (State) {
    case flash:
      // flash longer, then jump to red
      if (Time > 20 && outside==light) {
        digitalWrite(LedY, LOW);    // set the LED off
        digitalWrite(LedR, HIGH);   // set the LED on
        State = red;
        Time = 0;
      } else {
        if (Flash == 0) {
           digitalWrite(LedY, HIGH);
           Flash = 1;
        } else {
           digitalWrite(LedY, LOW);
           Flash = 0;
        }
      }
      break;
    case red:
      // need to go to green yet?
      if (Time > 4) {
        digitalWrite(LedR, LOW);    // set the LED off
        digitalWrite(LedG, HIGH);   // set the LED on
        Time = 0;
        if (outside != dark) {
          State = green;
        } else {
          State = flash;
          digitalWrite(LedR, LOW);    // set the LED off
          digitalWrite(LedG, LOW);   // set the LED on
        }
      }
      break;
    case green:
      // need to go to green yet?
      if (Time > 4) {
        digitalWrite(LedG, LOW);    // set the LED off
        digitalWrite(LedY, HIGH);   // set the LED on
        Time = 0;
        if (outside != dark) {
          State = orange;
        } else {
          State = flash;
          digitalWrite(LedR, LOW);    // set the LED off
          digitalWrite(LedG, LOW);   // set the LED on
        }
      }
      break;
    case orange:
      // need to go to green yet?
      if (Time > 2) {
        digitalWrite(LedY, LOW);    // set the LED off
        digitalWrite(LedR, HIGH);   // set the LED on
        Time = 0;
        if (outside != dark) {
          State = red;
        } else {
          State = flash;
          digitalWrite(LedR, LOW);    // set the LED off
          digitalWrite(LedG, LOW);   // set the LED on
        }
      }
      break;
    default:
      break;
  }
  Time++;
}
