/*
 * Project test_ledLights
 * Description:
 * Author:
 * Date:
 */

#include "neopixel.h"

int neoPin = D4;
int pixelCount = 12;
int pixelType = SK6812RGBW;
int i;
int diode = A1;
int diodeNum;
int pixelBri;
SYSTEM_MODE(SEMI_AUTOMATIC)
Adafruit_NeoPixel strip(pixelCount,neoPin,pixelType);

void setup() {
  Serial.begin(9600);
  pinMode(neoPin,OUTPUT);
  strip.begin();
  strip.show();

  pinMode(diode,INPUT);

}

void loop() {

  diodeNum = analogRead(diode);
  if (diodeNum>3000){
    diodeNum=3000;
  }
  if(diodeNum<80){
    diodeNum=80;
 }
  pixelBri = map(diodeNum,80, 3000, 255,0);

  

  for (i=0; i<12; i++){
    strip.setBrightness(pixelBri);
    strip.setPixelColor(i,0,255,0);
    strip.show();
  }
  //analogWrite(neoPin,strip.show());
  Serial.printf("diode:%i\n",diodeNum);
Serial.printf("pixelBri:%i\n",pixelBri);
}