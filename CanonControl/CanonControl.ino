
/*
Arduino sketch for simulating a Canon RC-1 IR remote control to do timelapse photography with a compatible Canon DSLR
2010, Martin Koch
http://controlyourcamera.blogspot.com/
Huge thanks go to http://www.doc-diy.net/photo/rc-1_hacked/index.php for figuring out the IR code.

Note: Drive mode of camera should be set to Remote Control. The I sensor is in the grip. See:
http://www.eos-magazine.com/articles/remotes/canonrc6.html
*/

#define irLED 11 
#define statusLED 13
#define pushBUTTON 7

int interval = 5; //seconds
int timelapseDuration = 60; //seconds
int numberOfShots = timelapseDuration / interval;

void setup() {
  pinMode(irLED, OUTPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(pushBUTTON, INPUT);
  digitalWrite(pushBUTTON, HIGH);
  
}

void loop() { 
  if (digitalRead(pushBUTTON) == LOW) {
    for (int i=0; i <= numberOfShots; i++) {
      digitalWrite(statusLED, HIGH); //Timelapse active
      sendInfraredSignal();
      digitalWrite(statusLED, LOW);
      if (i < numberOfShots) delay(interval * 1000); //ms
    }
  }
}

void sendInfraredSignal() {
  for(int i=0; i<16; i++) { 
    digitalWrite(irLED, HIGH);
    delayMicroseconds(11);
    digitalWrite(irLED, LOW);
    delayMicroseconds(11);
   } 
   delayMicroseconds(7330); 
   for(int i=0; i<16; i++) { 
     digitalWrite(irLED, HIGH);
     delayMicroseconds(11);
     digitalWrite(irLED, LOW);
     delayMicroseconds(11);
   }   
}
