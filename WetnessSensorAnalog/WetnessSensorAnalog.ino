/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, reads the result
 and determines wetness to set an output
 Also prints the results to the serial monitor if required
 
 Note: uses watchdog and timer functions for deep sleep
 
 The circuit:
 * water sensor connected connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 13 to ground
 
 created 11 Jun, 2012
 by Peter van Es
 
 
 */

#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int led = 13; // Analog output pin that the LED is attached to
int nint;
int pinPiezoSpeaker=10;

volatile boolean f_wdt=1;

int sensorValue = 0;        // value read from the pot
int treshold = 600;         // value when wet < treshold

#define ECHO_TO_SERIAL  0  // echo data to serial port ?

byte del;
int cnt;
byte state=0;

void setup() {

#if ECHO_TO_SERIAL  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
#endif  

  // CPU Sleep Modes 
  // SM2 SM1 SM0 Sleep Mode
  // 0    0  0 Idle
  // 0    0  1 ADC Noise Reduction
  // 0    1  0 Power-down
  // 0    1  1 Power-save
  // 1    0  0 Reserved
  // 1    0  1 Reserved
  // 1    1  0 Standby(1)

  cbi( SMCR,SE );      // sleep enable, power down mode
  cbi( SMCR,SM0 );     // power down mode
  sbi( SMCR,SM1 );     // power down mode
  cbi( SMCR,SM2 );     // power down mode

  setup_watchdog(9);
  pinMode(led, OUTPUT);     
}

void loop() {

  if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
    f_wdt=0;       // reset flag

  // read the analog in value:
  sensorValue = analogRead(analogInPin);            

#if ECHO_TO_SERIAL
  // print the results to the serial monitor:
  Serial.print("sensor = " );                       
  Serial.println(sensorValue);
  delay(20);               // wait until the last serial character is send
#endif

    switch (state) {
    case 0:               // waiting for wetness
      if (sensorValue < treshold) {
        // it's wet!
        state=1;
        digitalWrite(led,HIGH);  // let led blink
        whistle();
      }
      break;
    case 1:                // waiting for dry
      if (sensorValue > treshold)  {
        digitalWrite(led,LOW);  // let led blink
        state=0;
      } else {
        // keep alarming
        digitalWrite(led,HIGH);  // let led blink
        whistle();
      }
      break;
    }

    nint++;

#if ECHO_TO_SERIAL
  // print the results to the serial monitor:
  Serial.print("State " );                       
  Serial.println(state);
  Serial.print("Sleep " );                       
  Serial.println(nint);
#endif
    delay(20);               // wait until the last serial character is send
    digitalWrite(led,LOW);

    // pinMode(led,INPUT); // set all used port to intput to save power
    pinMode(pinPiezoSpeaker,INPUT); // set all used port to intput to save power

    system_sleep();

    pinMode(led,OUTPUT); // set all ports into state before sleep
    pinMode(pinPiezoSpeaker,OUTPUT); // set all ports into state before sleep


  }
}


//****************************************************************  
// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

//****************************************************************  
// put some whiste sound on piezo
void whistle() {
  for (int ii = 0; ii<= 20; ii++) {  
    for (del=0; del <=254; del++) {
      digitalWrite(pinPiezoSpeaker,0);
      delayMicroseconds((ii*5)+(del * 2));
      digitalWrite(pinPiezoSpeaker,1);
      delayMicroseconds((ii*5)+(del * 2));
    }
    PORTB ^= 32;  // toggle pinLed
  }
}

//****************************************************************
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9=8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

#if ECHO_TO_SERIAL
  // print the results to the serial monitor:
  Serial.print("WD " );                       
  Serial.println(ww);
  delay(20);
#endif

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);


}
//****************************************************************  
// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}


