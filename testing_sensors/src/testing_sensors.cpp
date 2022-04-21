/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/DennisDavis/Documents/IoT/New_ABQ_Bus_Stop/testing_sensors/src/testing_sensors.ino"
/*
 * Project testing_sensors
 * Description:
 * Author:
 * Date:
 */

#include <Adafruit_BME280.h>
#include "Grove-Ultrasonic-Ranger.h"
#include "math.h"
#include "neopixel.h"

void setup();
void loop();
void flashRed();
#line 13 "c:/Users/DennisDavis/Documents/IoT/New_ABQ_Bus_Stop/testing_sensors/src/testing_sensors.ino"
SYSTEM_MODE(SEMI_AUTOMATIC)

Ultrasonic ultrasonic(A3);

int mq = A0;
int mqSensor;
int lastTime;
int lastTime2;
int buttonPin=D4;
bool button;
int oldButton;
int onOff;
int buttonState;
int AQpin=A4;
int AQSensor;
int flameSensor;
int flamePin= A5;
int fanPin=A2;
int busFan;
Adafruit_BME280 bme;
float tempC, tempF;
int hexAddress = 0x76;
bool status;
int motionPin=D7;
int motion;
int i;
int pixelCount = 24;
int pixelType = WS2812B;
int pixelPin=D5;

Adafruit_NeoPixel strip(pixelCount, pixelPin, pixelType);

unsigned int timer;
unsigned int timer2;
bool redState;

void setup() {

  Serial.begin(9600);
  pinMode(mq, INPUT);
  pinMode(buttonPin,INPUT);
  pinMode(AQpin,INPUT);
  pinMode(flamePin,INPUT);
  pinMode(fanPin,OUTPUT);
  pinMode(motionPin,INPUT);

  strip.begin();
  strip.show();

  status = bme.begin(hexAddress);
    if (status == false) {
        Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
    }

}

void loop() {
  tempC = bme.readTemperature();
  tempF = tempC * (9.0/5.0)+32.2;

  
         

    button = digitalRead(buttonPin);

        if (button != oldButton) {
          if(button==true){
            onOff = !onOff;
        }
            oldButton = button;
        }
        //Serial.printf("onOff:%i\n", onOff);
    
    if (onOff == 0){
      for (i = 0; i < 12; i++) {
         strip.setPixelColor(i, 255, 241, 224);
          strip.show();
      }
    }

    if (onOff == 1) {
        if (millis() - timer2 > 10000) {
            onOff = false;
            timer2 = millis();
        }
        if (millis() - timer > 1000) {
            redState = !redState;
            flashRed();
            timer = millis();
            Serial.printf("flash%i\n",redState);
        }

        //Serial.printf("Emergency button has been pressed.\n", button);
    }


  
  if(millis()-lastTime2>10000){
  long RangeInCentimeters;
  RangeInCentimeters=ultrasonic.MeasureInCentimeters();
  //Serial.printf("RIC:%i\n",RangeInCentimeters);
  if(RangeInCentimeters<50){
   // Serial.printf("The trash is full\n");
  }
  lastTime2=millis();
  }

mqSensor = analogRead(mq);
buttonState = digitalRead(button);
AQSensor = analogRead(AQpin);
motion=digitalRead(motionPin);

if(millis()-lastTime>1000){
 //Serial.printf("MQ-4: %i\n", mqSensor);
//Serial.printf("button:%i\n",buttonState);
//  Serial.printf("AQSensor%i\n",AQSensor);
// Serial.printf("Temp F:%0.2f\n",tempF);
// Serial.printf("motion:%i\n",motion);
lastTime=millis();
if(buttonState==1){
  Serial.printf("Emergancy button has been pressed, please send a response unit to station 1\n",buttonState);
}

}

flameSensor=digitalRead(flamePin);
if(flameSensor==0){
  //Serial.printf("Flame Detected.\n",flameSensor);
}

//busFan=digitalRead(fanPin);

if (tempF>80 && AQSensor<2000 && mqSensor<2000){
  digitalWrite(fanPin, HIGH);
}else{
digitalWrite(fanPin, LOW);
}

}

// bool isThereFire(){
// const int threshHold = 500;
// if(flameSensor>threshHold){
//   return true;
// }else{
//   return false;
// }

// }

void flashRed() {
    int i;
    int j;

    
        if (redState) {
          for (j = 0; j < 24; j++) {
            strip.setPixelColor(j, 255,0,0);
            strip.show();
          }
        }

        if (redState == false) {
          for (j = 0; j < 24; j++) {
            strip.setPixelColor(j, 0,0,0);
            strip.show();
              
        }
    }

}