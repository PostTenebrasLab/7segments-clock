
//#include <Servo.h> 
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> 
#include "RTClib.h"

RTC_DS1307 rtc;

boolean DODEBUG = false;

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); //servo driver board 1
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); //servo driver board 2

short segA = 0; //Top
short segB = 1; //right top (when facing it)
short segC = 2; //right bottom (when facing it)
short segD = 3; //bottom
short segE = 4; //left bottom (when facing it)
short segF = 5; //left top (when facing it)
short segG = 6; //middle

short OEPin = 13;
short ledPin = 2;
short ledPin2 = 3;

short segIN = 90; //default values
short segOUT = 110; //default values

DateTime datePast; //last date we displayed
DateTime dateNow; //actual time now


int myOffsetIN[] = {-6, -6, -4, -6, -1, 2, -9,//digit 1 Hours
                    1,-7,-3,-2,2,0,-1,              //digit 2 Hours
                    -1,-3,-3,-1,5,4,-2,              //digit 3 Minutes
                    0,-1,-1,-1,2,-8,-6};             //digit 4 Minutes
                    
int myOffsetOUT[] = {-3, -6, -1, -3, 1, 5, -2, //digit 1 Hours
                    2,0,0,0,2,1,4,              //digit 2 Hours
                    0,0,0,0,5,3,8,              //digit 3 Minutes
                    5,1,0,1,1,-1,0};             //digit 4 Minutes


//initialize an array to keep track of where every motor is, so we only move what we want.
boolean myCurrentPos[]={
 1,1,1,1,1,1,1,
 1,1,1,1,1,1,1,
 1,1,1,1,1,1,1,
 1,1,1,1,1,1,1
};

void setup() 
{ 
  pinMode(OEPin, OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(ledPin2,OUTPUT);
  Serial.begin(9600);
  
  pwm.begin();
  pwm2.begin();
  
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
  pwm2.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
  
  #ifdef AVR
  Wire.begin();
  #else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  #endif
  rtc.begin();
  if (! rtc.isrunning()) 
    Serial.println("RTC is NOT running!");



  
  Serial.print("Starting \n");
  lightNumber(10,1);  //display nothing
  lightNumber(10,2);  //display nothing
  lightNumber(10,3);  //display nothing
  lightNumber(10,4);  //display nothing
  delay(4000);        //wait 4 secs 
   
  datePast = rtc.now();   
  displayNumber(datePast.hour()*100+datePast.minute());

 
} 
 

void loop() 
{ 
    dateNow = rtc.now();
    if(!(datePast.hour() == dateNow.hour() && datePast.minute() == dateNow.minute() ))
    {
      Serial.print("Time now: ");
      Serial.print(dateNow.hour());
      Serial.print(":");
      Serial.print("");
      Serial.print(dateNow.minute());
      Serial.print("\n");
      Serial.print("Time past: ");
      Serial.print(datePast.hour());
      Serial.print(":");
      Serial.print("");
      Serial.print(datePast.minute());
      Serial.print("\n");
      digitalWrite(OEPin, LOW);
      displayNumber(dateNow.hour()*100+dateNow.minute());
      datePast = dateNow;
      delay(1000); //wait for servo to finish moving
    }
    if(dateNow.second()%2==0){ 
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, HIGH);
    }
    else{
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, LOW);
    }
    
    stopPWM();
    digitalWrite(OEPin, HIGH);
  
}

void displayNumber(int toDisplay) {
  //Serial.println(toDisplay);
  for(int digit = 4 ; digit > 0 ; digit--) {
    //Serial.println(toDisplay % 10);
    lightNumber(toDisplay % 10,digit);
    toDisplay /= 10;
  }
}

void stopPWM(){
  for(int i=0;i<15;i++)
  {
    pwm.setPWM(i, 0, 0);
    pwm2.setPWM(i, 0, 0);  
  }
  delay(100);
}
void move(int servo, int position) {
  boolean myPos;
  if(DODEBUG) 
  {
    Serial.print("Moving servo #");
    Serial.print(servo);
    Serial.print(" to ");
    Serial.print(position);
    Serial.print("\n");
  }
  if (position>segOUT-10)
    myPos = 1;
  else
    myPos = 0;
  Serial.print(myCurrentPos[servo]);
  Serial.print("---");
  Serial.println(myPos);
  if(myCurrentPos[servo] != myPos)
  {
    Serial.println("Entering...");
    myCurrentPos[servo] = myPos;
    if (servo <14)  
      pwm.setPWM(servo, 0, map(position, 0, 180, SERVOMIN, SERVOMAX));  
    else  //then we must be on the second board for the minutes
      pwm2.setPWM(servo-14, 0, map(position, 0, 180, SERVOMIN, SERVOMAX));   //-14 so we can start again from pin0
  }
    
  //if (servo >21) Serial.print("SERVO: "); Serial.println(servo);  
}

void lightNumber(int numberToDisplay, int segment) {

  int offset = (segment - 1)*7;
  switch (numberToDisplay){

  case 0:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]); //segment a + offset to get right digit
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segOUT + myOffsetOUT[4+offset]);
    move(segF + offset , segOUT + myOffsetOUT[5+offset]);
    move(segG + offset , segIN  +  myOffsetIN[6+offset]);
    break;

  case 1:
    move(segA + offset , segIN  +  myOffsetIN[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segIN  +  myOffsetIN[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segIN  +  myOffsetIN[5+offset]);
    move(segG + offset , segIN  +  myOffsetIN[6+offset]);
    break;

  case 2:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segIN  +  myOffsetIN[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segOUT + myOffsetOUT[4+offset]);
    move(segF + offset , segIN  +  myOffsetIN[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 3:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segIN  +  myOffsetIN[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 4:
    move(segA + offset , segIN  +  myOffsetIN[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segIN  +  myOffsetIN[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segOUT + myOffsetOUT[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 5:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segIN  +  myOffsetIN[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segOUT + myOffsetOUT[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 6:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segIN  +  myOffsetIN[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segOUT + myOffsetOUT[4+offset]);
    move(segF + offset , segOUT + myOffsetOUT[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 7:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segIN  +  myOffsetIN[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segIN  +  myOffsetIN[5+offset]);
    move(segG + offset , segIN  +  myOffsetIN[6+offset]);
    break;

  case 8:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segOUT + myOffsetOUT[4+offset]);
    move(segF + offset , segOUT + myOffsetOUT[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 9:
    move(segA + offset , segOUT + myOffsetOUT[0+offset]);
    move(segB + offset , segOUT + myOffsetOUT[1+offset]);
    move(segC + offset , segOUT + myOffsetOUT[2+offset]);
    move(segD + offset , segOUT + myOffsetOUT[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segOUT + myOffsetOUT[5+offset]);
    move(segG + offset , segOUT + myOffsetOUT[6+offset]);
    break;

  case 10:  //ALL IN
    move(segA + offset , segIN  +  myOffsetIN[0+offset]);
    move(segB + offset , segIN  +  myOffsetIN[1+offset]);
    move(segC + offset , segIN  +  myOffsetIN[2+offset]);
    move(segD + offset , segIN  +  myOffsetIN[3+offset]);
    move(segE + offset , segIN  +  myOffsetIN[4+offset]);
    move(segF + offset , segIN  +  myOffsetIN[5+offset]);
    move(segG + offset , segIN  +  myOffsetIN[6+offset]);
    break;
  }
  delay(50); //wait a bit for servo to finish move
}

