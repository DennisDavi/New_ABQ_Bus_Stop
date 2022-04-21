/*
 * Project Master_Code_SmartBusStop
 * Description:
 * Author:
 * Date:
 */
#include "Adafruit_MQTT_SPARK.h"
#include "DFRobotDFPlayerMini.h"
#include "Grove-Ultrasonic-Ranger.h"
#include "HX711.h"
#include "TCreds.h"
#include "neopixel.h"
#include <Adafruit_BME280.h>
#include <Adafruit_MQTT.h>
#include <math.h>

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

Ultrasonic ultrasonic(A0);
const int MQ4ANALOGPIN = A1;
const int FLAMEPIN = A2;
const int MQ7ANALOGPIN = A3;
const int PHOTODIODEPIN = A4;
const int AQPIN = A5;
HX711 loadCell(D2, D13);
const int FANPIN = D3;
const int EMERGENCYBUTTON = D4;
const int LEDPIN = D5;
// const int FLAMEPINDIGITAL = D6;
const int MOTIONSENSOR = D6;
const int PLAYTIME = 10000;
const int NUMBERTRACKS = 1;

Adafruit_BME280 bme;

bool status2;
int p, timer3;
int flameSensor;
int currentTime, currentTime1, currentTime2, currentTime3, currentTime4, currentTime5, currentTime6, currentTime7, currentTime8, currentTime9;
int lastTime, lastTime1, lastTime2, lastTime3, lastTime4, lastTime5, lastTime6, lastTime7, lastTime8, lastTime9;
int mq4Analog;
int mq7Analog;
int diodeNum;
int nightLed;
int motion;
int AqSensor;
bool button;
int oldButton;
int onOff;
float tempC, tempF;
int hexAddress = 0x76;
bool status;
int i;
int pixelCount = 24;
int pixelType = WS2812B;
int pixelBri;
int ultraSonicSensor;
int trashMesure;

unsigned int timer;
unsigned int timer2;
bool redState;

Adafruit_NeoPixel strip(pixelCount, LEDPIN, pixelType);
const int CAL_FACTOR = 2333;
const int SAMPLES = 10;
float weight, rawData, calibration;
int offset;

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish mqttUltraSonic = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_can_Ultrasonic");
Adafruit_MQTT_Publish mqttFire = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Fire_Sensor");
Adafruit_MQTT_Publish mqttMethane = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Methane_Sensor");
Adafruit_MQTT_Publish mqttAlcohol = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/MQ7_Sensor");
Adafruit_MQTT_Publish mqttPhotoDiode = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Daylight_Sensor");
Adafruit_MQTT_Publish mqttTemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish mqttLoadCell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_Can_Loadcell");
Adafruit_MQTT_Publish mqttAqSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Air_Quality_Sensor");
Adafruit_MQTT_Publish mqttTrashIsFull = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_Is_Full");

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    
    // pinMode(FLAMEPIN, INPUT);
    pinMode(MQ4ANALOGPIN, INPUT);
    pinMode(MQ7ANALOGPIN, INPUT);
    pinMode(PHOTODIODEPIN, INPUT);
    pinMode(EMERGENCYBUTTON, INPUT);
    pinMode(LEDPIN, OUTPUT);
    pinMode(AQPIN, INPUT);
    pinMode(FANPIN, OUTPUT);
    pinMode(LEDPIN, OUTPUT);
    pinMode(MOTIONSENSOR, INPUT);
    strip.begin();
    strip.show();

    // MP3
    status2 = myDFPlayer.begin(Serial1, false);
    myDFPlayer.volume(25); //  0 to 30
    p = 0;
    timer3 = -PLAYTIME;
    // LOADCELL
    loadCell.set_scale();
    loadCell.tare();
    loadCell.set_scale(CAL_FACTOR);
    // WIFI
    WiFi.connect();
    while (WiFi.connecting()) {
        Serial.printf(".");
    }
    // BME
    status = bme.begin(hexAddress);
    if (status == false) {
        Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
    }
}
void loop() {

    MQTT_connect();
    long RangeInCentimeters;

    diodeNum = analogRead(PHOTODIODEPIN);
    // Serial.printf("motion:%i\n", motion);

    motion = digitalRead(MOTIONSENSOR);

     
    // test, now delete

    button = digitalRead(EMERGENCYBUTTON);  

    if(button!=oldButton){
        if (button == true) {
            onOff = !onOff;
        }
            oldButton = button;
        }
        //Serial.printf("onOff:%i\n", onOff);

    if(onOff==0){
        if (diodeNum > 3000) {
    diodeNum = 3000;
    }
    if (diodeNum < 80) {
    diodeNum = 80;
     }
     pixelBri = map(diodeNum, 80, 3000, 255, 0);
     for (i = 0; i < 24; i++) {
     strip.setBrightness(pixelBri);
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
            // FLASHRED
            flashRed();
            // mp3
            mp3();
            timer = millis();
        }

        Serial.printf("Emergency button has been pressed.\n", button);
    }

    // Ultrasonic Sensor
    ultraSonicSensor = (ultrasonic.MeasureInCentimeters());

    if ((millis() - lastTime1) > 30000) {
        mq4Analog = analogRead(MQ4ANALOGPIN);
        mq7Analog = analogRead(MQ7ANALOGPIN);

        mq4Analog = analogRead(MQ4ANALOGPIN);
        mq7Analog = analogRead(MQ7ANALOGPIN);

        if (mqtt.Update()) {

            // MQ4 & MQ7 Sensors
            // indoor reading: 1800-2100 by an exhaust is around 3200-3400

            mqttMethane.publish(mq7Analog);
            Serial.printf("Publishing MQ7 level:: %i \n", mq7Analog);
            mqttAlcohol.publish(mq4Analog);
            Serial.printf("Publishing MQ4 level:: %i \n", mq4Analog);

            // Ultra Sonic
            trashMesure = map(ultraSonicSensor,0,10,500,0);
            mqttUltraSonic.publish(trashMesure);
            Serial.printf("Publishing Ultra Sonic Sensor  %i\n", trashMesure);

            // Flame Sensor
            // flameSensor = analogRead(FLAMEPIN);
            // if (flameSensor < 1500) {
            //     Serial.printf("Flame Detected!");
            //     mqttFire.publish(flameSensor);
            //     Serial.printf("Publishing flameSensor:%i \n", flameSensor);
            // }

            // Photo Diode Sensor
            mqttPhotoDiode.publish(diodeNum);
            Serial.printf("Publishing Daylight Sensor:%i \n", diodeNum);

            weight = loadCell.get_units(SAMPLES);
            rawData = loadCell.get_offset();
            calibration = loadCell.get_scale();

            // Load Cell Sensor
            mqttLoadCell.publish(weight);
            Serial.printf("Publishing trash can weight:%0.2f \n", weight);

            // Air Quality Sensor
            AqSensor = analogRead(AQPIN);
            mqttAqSensor.publish(AqSensor);
            Serial.printf("Publishing Air Quality:%i \n", AqSensor);

            // Temperature/BME
            tempC = bme.readTemperature();
            tempF = tempC * (9.0 / 5.0) + 32.2;

            mqttTemp.publish(tempF);
            Serial.printf("Publishing temparture:%0.2f \n", tempF);

            lastTime1 = millis();
        }
    }
    if (tempF > 69 && AqSensor < 1500) {
        digitalWrite(FANPIN, 1);
    } else {
        digitalWrite(FANPIN, 0);
    }
}

void MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    if (mqtt.connected()) {
        return;
    }

    Serial.print("Connecting to MQTT... ");

    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.printf("%s\n", (char *)mqtt.connectErrorString(ret));
        Serial.printf("Retrying MQTT connection in 5 seconds..\n");
        // mqtt.disconnect();
        delay(5000); // wait 5 seconds
    }
    Serial.printf("MQTT Connected!\n");
}

void flashingLights() {
    strip.clear();
    for (i = 0; i < 24; i++) {
        currentTime3 = millis();
        if ((currentTime3 - lastTime3) > 1000) {
            strip.setPixelColor(i, 255, 0, 0);
            lastTime3 = millis();
        }
        currentTime4 = millis();
        if ((currentTime4 - lastTime4) > 1000) {
            strip.setPixelColor(i, 255, 255, 255);
            lastTime4 = millis();
        }
    }
    strip.show();
}

void flashRed() {
    int i;
    int j;

   
        if (redState) {
             for (j = 0; j < 24; j++) {
            strip.setPixelColor(j, 0XFF0000);
            strip.setBrightness(20);
            strip.show();
             }
        }
        if (redState == false) {
             for (j = 0; j < 24; j++) {
            strip.setPixelColor(j, 0,0,0);
            strip.setBrightness(20);
            strip.show();
             }
        }
    }


void mp3() {
        Serial.printf("Play Next - Track\n");
        myDFPlayer.play(1); // Play next mp3 every 3 second.
        delay(6000);
    }


void printDetail(uint8_t type, int value) {
    switch (type) {
    case TimeOut:
        Serial.printf("Time Out!\n");
        break;
    case WrongStack:
        Serial.printf("Stack Wrong!\n");
        break;
    case DFPlayerCardInserted:
        Serial.printf("Card Inserted!\n");
        break;
    case DFPlayerCardRemoved:
        Serial.printf("Card Removed!\n");
        break;
    case DFPlayerCardOnline:
        Serial.printf("Card Online!\n");
        break;
    case DFPlayerPlayFinished:
        Serial.printf("Number: %i Play Finished\n", value);
        break;
    case DFPlayerError:
        Serial.printf("DFPlayerError: ");
        switch (value) {
        case Busy:
            Serial.printf("Card not found\n");
            break;
        case Sleeping:
            Serial.printf("Sleeping\n");
            break;
        case SerialWrongStack:
            Serial.printf("Get Wrong Stack\n");
            break;
        case CheckSumNotMatch:
            Serial.printf("Check Sum Not Match\n");
            break;
        case FileIndexOut:
            Serial.printf("File Index Out of Bound\n");
            break;
        case FileMismatch:
            Serial.printf("Cannot Find File\n");
            break;
        case Advertise:
            Serial.printf("In Advertise\n");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}