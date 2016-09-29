//we need to use a serial library, not the default tx/rx pins. That would interfere with downloading
#include <SoftwareSerial.h>
#define RX 4
#define TX 5
SoftwareSerial wifiSerial(RX, TX);

//define the pins we're using.
int led = 13;

//note that we're using 'A/B' rather than 'L/R'.
//we don't know, or care, which is left or right.
//we'll fix it later by setting some flags.
int motorDirA = 12;
int motorDirB = 13;
int motorSpdA = 3;
int motorSpdB = 11;


//these two flags indicate if we need to flip one of the motors, globally.
bool flipA = false;     // is motorA backwards?
bool flipB = false;     // is motorB backwards?
bool flipSpin = false;  // do we spin in the wrong direction?

//helper function to deal with motor direction bits.
//This allows us to easily flip a motor direction if we have 
//a motor wired the wrong way.
int dir(bool desired, bool flip) {
  if (flip) { desired = !desired; }
  return (int)desired;
}

//function to move forward and backwards
//use speed=0 to stop.
void go(bool forward, int speed) {
  digitalWrite(motorDirA, dir(forward,flipA) ); //spin both motors in the same direction
  digitalWrite(motorDirB, dir(forward,flipB) ); //spin both motors in the same direction
  analogWrite(motorSpdA, speed);
  analogWrite(motorSpdB, speed);
}
//function to spin, given direction and speed
void spin(bool left, int speed) {  
  if (flipSpin) { left = !left; }
  digitalWrite(motorDirA, dir( left,flipA) );     //spin one motor one direction
  digitalWrite(motorDirB, dir(!left,flipB) );    //spin the other motor another direction
  analogWrite(motorSpdA, speed);
  analogWrite(motorSpdB, speed);
}


// the setup routine runs once when you press reset:
void setup() {  
                
  // initialize all our control pins to be outputs
  pinMode(led, OUTPUT);    
  pinMode(motorSpdA, OUTPUT); 
  pinMode(motorSpdB, OUTPUT);
  pinMode(motorDirA, OUTPUT);
  pinMode(motorDirB, OUTPUT);

  //set the motors off.
  analogWrite(motorSpdA, 0);  
  analogWrite(motorSpdB,  0);  

  //initialie the hardware serial port.
  Serial.begin(9600); 
  wifiSerial.begin(9600);
  

}

// the loop routine runs over and over again forever:
void loop() {
  //we check if we have received anything on the serial port
  if (wifiSerial.available() > 0){
    //if there is something waiting, read a character, and deal with it.
    char c = wifiSerial.read();
    Serial.print(c); //echo everything we receive over USB for debug
    wifiSerial.print(".");
    if (c=='f') { go(true, 100); }
    else if (c=='b') { go(false, 100); }
    else if (c=='s') { go(true, 0); }
    else if (c=='l') { spin(true, 50); }
    else if (c=='r') { spin(false, 50); }
    else if (c=='A') { flipA = !flipA; }
    else if (c=='B') { flipB = !flipB; }
    else if (c=='S') { flipSpin = !flipSpin; }
    else if (c=='\n') { } //ignore new lines
    else if (c=='\r') { } //ignore cariage returns
    else { wifiSerial.print("!"); }
  }
}
