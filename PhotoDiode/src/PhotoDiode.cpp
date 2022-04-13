/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/Abeyta/Documents/IoT/ABQ-Smart-Bus-Stop/PhotoDiode/src/PhotoDiode.ino"
/*
 * Project PhotoDiode
 * Description:
 * Author: Thomas Abeyta
 * Date: 4/11/22
 */

void setup ();
void loop();
#line 8 "/Users/Abeyta/Documents/IoT/ABQ-Smart-Bus-Stop/PhotoDiode/src/PhotoDiode.ino"
const int DIODEPIN = A4;
const int LEDPIN = A5;

int diodeState;
int nightLed;
int currentTime, lastTime;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup (){

    pinMode(DIODEPIN,INPUT);
    pinMode(LEDPIN, OUTPUT);

}
void loop(){

    diodeState = analogRead(DIODEPIN);

    nightLed = map(diodeState,120,195,255,-0);
    currentTime = millis();
    if((currentTime-lastTime)>2000){
    Serial.printf("diode State:%i conversion:%i\n", diodeState,nightLed);
    lastTime =millis();
    }
     analogWrite(LEDPIN,nightLed);

}