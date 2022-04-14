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

void setup();
void loop();
#line 12 "c:/Users/DennisDavis/Documents/IoT/New_ABQ_Bus_Stop/testing_sensors/src/testing_sensors.ino"
SYSTEM_MODE(SEMI_AUTOMATIC)

Ultrasonic ultrasonic(A3);

int mq = A0;
int mqSensor;
int lastTime;
int lastTime2;
int button=A1;
int oldButton;
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



void setup() {

  Serial.begin(9600);
  pinMode(mq, INPUT);
  pinMode(button,INPUT_PULLDOWN);
  pinMode(AQpin,INPUT);
  pinMode(flamePin,INPUT);
  pinMode(fanPin,OUTPUT);

  status = bme.begin(hexAddress);
    if (status == false) {
        Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
    }

}

void loop() {
  tempC = bme.readTemperature();
  tempF = tempC * (9.0/5.0)+32.2;

  
  if(millis()-lastTime2>10000){
  long RangeInCentimeters;
  RangeInCentimeters=ultrasonic.MeasureInCentimeters();
  Serial.printf("RIC:%i\n",RangeInCentimeters);
  if(RangeInCentimeters<50){
    Serial.printf("The trash is full\n");
  }
  lastTime2=millis();
  }

mqSensor = analogRead(mq);
buttonState = digitalRead(button);
AQSensor = analogRead(AQpin);

if(millis()-lastTime>1000){
 Serial.printf("MQ-7: %i\n", mqSensor);
// Serial.printf("button:%i\n",buttonState);
 Serial.printf("AQSensor%i\n",AQSensor);
Serial.printf("Temp F:%0.2f\n",tempF);
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