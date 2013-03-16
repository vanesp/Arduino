/*
  Button
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when pressing a pushbutton attached to pin 2. 
 
 
 The circuit:
 * LED attached from pin 13 to ground 
 * pushbutton attached to pin 12 from +5V
 * 10K resistor attached to pin 12 from ground
 
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 
 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Button
 */

// constants won't change. They're used here to 
// set pin numbers:
const int buttonPin = 12;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int PINS_SHUTTER = 11;
const int PINS_AUTOFOCUS = 12;

const int s_shutterLag = 63;   // D60 milliseconds shutterlag
const int s_autofocusTime = 292; // D60 Autofocus time, should be set to 0 for manual focus

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

// Begins the shutter 
void camera_shutterBegin(unsigned int shutterLag){
    digitalWrite(PINS_SHUTTER,HIGH); 
    delay(shutterLag);
}
 
// Ends the shutter
void camera_shutterEnd(unsigned int preCloseTime){
    delay(preCloseTime);
    digitalWrite(PINS_SHUTTER,LOW); 
}

// Begins the autofocus
void camera_autofocusBegin(unsigned int autofocusTime){
    digitalWrite(PINS_AUTOFOCUS,HIGH); 
    delay(autofocusTime);
}

// Ends the autofocus
void camera_autofocusEnd(){
    digitalWrite(PINS_AUTOFOCUS,LOW);
}


void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  pinMode(PINS_SHUTTER, OUTPUT);      
  pinMode(PINS_AUTOFOCUS, OUTPUT);      

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);     
}

void loop(){
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH); 
   
    camera_autofocusBegin(s_autofocusTime); 
    camera_shutterBegin(s_shutterLag); 
  
    camera_shutterEnd(30); 
    camera_autofocusEnd(); 
    
    
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
  }
}
