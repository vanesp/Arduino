/*
Example 13.0
Drive a piezoelectric buzzer with Arduino
http://tronixstuff.wordpress.com/tutorials > Chapter 13
*/
#define times 10
int i = 0;

void setup()
{
     pinMode(11, OUTPUT);   // sets the pin as output
      // initialize serial communications at 9600 bps:
     Serial.begin(115200); 

}
void loop()
{
     if (i < times) {
       Serial.println("Buzzer on");      
       analogWrite(11,128);
       // this value (128) equals around 1500 hz  
       // our device ABT-402-RC needs around 4000Hz
       delay(500);
       digitalWrite(11, LOW);
       Serial.println("Buzzer off");
       i++;
     }      
     delay(500);
}

