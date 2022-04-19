/*
 * Project Master_Code_SmartBusStop
 * Description:
 * Author:
 * Date:
 */
#include "Adafruit_MQTT_SPARK.h"
#include "Grove-Ultrasonic-Ranger.h"
#include "HX711.h"
#include "TCreds.h"
#include "neopixel.h"
#include <Adafruit_BME280.h>
#include <Adafruit_MQTT.h>
#include <math.h>

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
const int FLAMEPINDIGITAL = D6;
const int MOTIONSENSOR = D7;

Adafruit_BME280 bme;

int flameSensor;
int currentTime, currentTime1, currentTime2, currentTime3, currentTime4, currentTime5, currentTime6, currentTime7, currentTime8, currentTime9;
int lastTime, lastTime1, lastTime2, lastTime3, lastTime4, lastTime5, lastTime6, lastTime7, lastTime8, lastTime9;
int mq4Digital, mq4Analog;
int mq7analog;
int diodeNum;
int nightLed;
int motion;
int AqSensor;
int button;
float tempC, tempF;
int hexAddress = 0x76;
bool status;
int i;
int pixelCount = 24;
int pixelType = WS2812B;
int pixelBri;
int ultraSonicSensor;

Adafruit_NeoPixel strip(pixelCount, LEDPIN, pixelType);
const int CAL_FACTOR = 2233;
const int SAMPLES = 10;
float weight, rawData, calibration;
int offset;

// delete after published is established
int value1 = 102;
int ON_OFF;
struct geo {
    float lat;
    float lon;
    int alt;
};

geo myLoc;
geo locations[13];

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Adafruit_MQTT_Publish mqttObj = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/abq_gps");
Adafruit_MQTT_Publish mqttUltraSonic = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_can_Ultrasonic");
Adafruit_MQTT_Publish mqttFire = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Fire_Sensor");
Adafruit_MQTT_Publish mqttMethane = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Methane_Sensor");
Adafruit_MQTT_Publish mqttPhotoDiode = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Daylight_Sensor");
Adafruit_MQTT_Publish mqttTemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish mqttLoadcell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_Can_Loadcell");
Adafruit_MQTT_Publish mqttAqSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Air_Quality_Sensor");


Adafruit_MQTT_Subscribe mqttON_OFFobject = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ON_OFF");

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    Serial.begin(9600);
    pinMode(FLAMEPIN, INPUT);
    pinMode(MQ4ANALOGPIN, INPUT);
    pinMode(MQ7ANALOGPIN, INPUT);
    pinMode(PHOTODIODEPIN, INPUT);
    pinMode(EMERGENCYBUTTON, INPUT);
    pinMode(LEDPIN, OUTPUT);
    pinMode(AQPIN, INPUT);
    pinMode(FANPIN, OUTPUT);
    pinMode(LEDPIN, OUTPUT);
    strip.begin();
    strip.show();

    loadCell.set_scale();
    loadCell.tare();
    loadCell.set_scale(CAL_FACTOR);

    WiFi.connect();
    while (WiFi.connecting()) {
        Serial.printf(".");
    }

    // Setup MQTT subscription for onoff feed.
    mqtt.subscribe(&mqttON_OFFobject);

    status = bme.begin(hexAddress);
    if (status == false) {
        Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
    }
}
void loop() {
    

    diodeNum = analogRead(PHOTODIODEPIN);
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
    Serial.printf("diode:%i\n", diodeNum);

    button = digitalRead(EMERGENCYBUTTON);
    if (button == 1) {
        Serial.printf("Emergency button has been pressed.\n", button);
    }

    MQTT_connect();
    
    // currentTime = millis();
    // if ((currentTime - lastTime) > 6000) {
    //     if (mqtt.Update()) {
    //         mqttObj.publish(value1);
    //         Serial.printf("Publishing test: %i \n", value1);
    //     }
    //     lastTime1 = millis();
    // }

    // this is our 'wait for incoming subscription packets' busy subloop
    Adafruit_MQTT_Subscribe *subscription;
    // while ((subscription = mqtt.readSubscription(100))) {
    //     if (subscription == &mqttON_OFFobject) {
    //         ON_OFF = atof((char *)mqttON_OFFobject.lastread);
    //         Serial.printf("Received %i from Adafruit.io feed FeedNameB \n", ON_OFF);
    //     }
    // }

   
    
//Ultrasonic Sensor

   
    ultraSonicSensor = ((ultrasonic.MeasureInCentimeters() * 100)-10000);

    currentTime1 = millis();
    if (currentTime1 - lastTime1 > 5000) {
        if (mqtt.Update()) {
                mqttUltraSonic.publish(ultraSonicSensor);
                Serial.printf("Publishing  %i", ultraSonicSensor);
                Serial.println("cm\n");
            }
            lastTime1 = millis();
        }


//Flame Detector AKA Light sensor
        currentTime2 = millis();
        if ((currentTime2 - lastTime2) > 1000) {
            flameSensor = analogRead(FLAMEPIN);
            if (flameSensor < 1500) {
                Serial.printf("Flame Detected!");
                if (mqtt.Update()) {
                    mqttFire.publish(flameSensor);
                    Serial.printf("Publishing flameSensor:%i \n", flameSensor);
                }
            }
            // Serial.printf("flame sensor:%i DIGITAL:%i\n", flameSensor, FLAMEPINDIGITAL);
            lastTime2 = millis();
        }

//MQ4 & MQ7 Sensors
        currentTime3 = millis();
        if ((currentTime3 - lastTime3) > 5000) {
            mq4Analog = analogRead(MQ4ANALOGPIN);
            mq7analog = analogRead(MQ7ANALOGPIN);
            if (mqtt.Update()) {
                mqttMethane.publish(mq4Analog);
                Serial.printf("Publishing Methane level:: %i \n", mq4Analog);
            }
            // indoor reading: 1800-2100 by an exhaust is around 3200-3400
            Serial.printf("mq4 Analog Read:%i  mq7 Analog Read:%i\n", mq4Analog, mq7analog);
            lastTime3 = millis();
        }

//PhotoDiode Sensor
        currentTime4 = millis();
        if ((currentTime4 - lastTime4) > 5000) {
            if (mqtt.Update()) {
                mqttPhotoDiode.publish(diodeNum);
                Serial.printf("Publishing Daylight Sensor:%i \n", diodeNum);
                lastTime4 = millis();
            }
            
        }


//Load Cell 
        weight = loadCell.get_units(SAMPLES);
        rawData = loadCell.get_offset();
        calibration = loadCell.get_scale();

          currentTime5 = millis();
        if ((currentTime5 - lastTime5) > 3000) {
            if (mqtt.Update()) {
                mqttFire.publish(weight);
                Serial.printf("Publishing temparture:%i \n", weight);
            lastTime5 = millis();
        }}

// Air Quality Sensor
        AqSensor = analogRead(AQPIN);
        currentTime6 = millis();
        if ((currentTime6 - lastTime6) > 10000) {
            Serial.printf("Air Quality Sensor:%i  Temp:%0.2f\n", AqSensor, tempF);
            lastTime6 = millis();
            mqttFire.publish(AqSensor);
            Serial.printf("Publishing temparture:%i \n", AqSensor);

        if (tempF > 71 && AqSensor < 3000) {
            digitalWrite(FANPIN, 1);
        } else {
            digitalWrite(FANPIN, 0);
        }
        }

//BME/Temperature Sensor
        tempC = bme.readTemperature();
        tempF = tempC * (9.0 / 5.0) + 32.2;

        currentTime8 = millis();
        if ((currentTime8 - lastTime8) > 3000) {
            if (mqtt.Update()) {
                mqttFire.publish(tempF);
                Serial.printf("Publishing temparture:%i \n", tempF);
                lastTime8 = millis();
            }
        }


        currentTime9 = millis();
        if ((currentTime9 - lastTime9) > 3000) {
            if(ultraSonicSensor < 3 || weight > 400){
            if (mqtt.Update()) {
                mqttFire.publish(500);
                Serial.printf("Publishing temparture:%i \n", tempF);
            lastTime9 = millis();
            }
        }}

}
    void MQTT_connect() {
        int8_t ret;

        //Stop if already connected.
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
