/*
 * RTS (RJ11 pin 2) blauwwit op pin 4
 * GND (RJ11 pin 3) blauw op GND
 * RxD (RJ11 pin 5) groen/wit op pin 10
 * 
 */

#include <SoftwareSerial.h>

const int requestPin =  4;
int incomingByte = 0;
String inputString = "";
boolean start_p1_record;
boolean data_ready = false;

SoftwareSerial mySerial(10, 11, true); // RX, TX, inverted

void setup () {
  mySerial.begin(9600);
  delay(1);
  Serial.begin(57600);

  Serial.println("P1 Smart meter reading - START\n");

  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, HIGH);
}

void loop () {
  while (mySerial.available() > 0) {
    
    incomingByte = mySerial.read();
    incomingByte &= ~(1 << 7);
    char inChar = (char)incomingByte;
    
    Serial.write(inChar);
   }

}
