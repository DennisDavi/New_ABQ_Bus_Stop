/*
 * Project Master_Code_SmartBusStop
 * Description:
 * Author:
 * Date:
 */
#include <Adafruit_BME280.h>
#include "Adafruit_MQTT_SPARK.h"
#include "Grove-Ultrasonic-Ranger.h"
//#include "TCreds.h"
#include <Adafruit_MQTT.h>
#include <math.h>


Ultrasonic ultrasonic(A0);
const int FLAMEPIN = A1;
const int MQ4ANALOGPIN = A2;
const int MQ7ANALOGPIN = A3;
const int PHOTODIODEPIN = A4;
const int AQPIN = A5;
const int LOADCELL = D2;
 const int FANPIN = D3;
const int EMERGENCYBUTTON = D4;
const int LEDPIN = D5;
const int FLAMEPINDIGITAL = D6;
const int MOTIONSENSOR = D7;
//const int FANPIN = D8;

Adafruit_BME280 bme;

int flameSensor;
int currentTime, currentTime1, currentTime2, currentTime3, currentTime4, currentTime5, currentTime6;
int lastTime, lastTime1, lastTime2, lastTime3, lastTime4, lastTime5, lastTime6;
int mq4Digital, mq4Analog;
int diodeState;
int nightLed;
int motion;
int aqSensor;
int button;
float tempC, tempF;
int hexAddress = 0x76;
bool status;

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

// Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// // Adafruit_MQTT_Publish mqttObj = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/abq_gps");
// Adafruit_MQTT_Publish mqttObj1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_can_Fullness");
// Adafruit_MQTT_Publish mqttObj2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Fire_Sensor");
// Adafruit_MQTT_Publish mqttObj3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Methane_Sensor");
// Adafruit_MQTT_Publish mqttObj4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Daylight_Sensor");

// Adafruit_MQTT_Subscribe mqttON_OFFobject = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ON_OFF");

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    Serial.begin(9600);
    pinMode(FLAMEPIN, INPUT);
    pinMode(MQ4ANALOGPIN, INPUT);
    pinMode(PHOTODIODEPIN, INPUT);
    pinMode(LEDPIN, OUTPUT);
    pinMode(AQPIN,INPUT);
    pinMode(FANPIN,OUTPUT);

    WiFi.connect();
    while (WiFi.connecting()) {
        Serial.printf(".");
    }

    // Setup MQTT subscription for onoff feed.
   // mqtt.subscribe(&mqttON_OFFobject);

    status = bme.begin(hexAddress);
    if (status == false) {
        Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
    }
}
void loop() {
    tempC = bme.readTemperature();
    tempF = tempC * (9.0/5.0)+32.2;

   // MQTT_connect();
    // currentTime = millis();
    // if((currentTime-lastTime) > 6000){
    //     if(mqtt.Update()) {
    //       mqttObj.publish(value1);
    //       Serial.printf("Publishing test: %i \n",value1);
    //       }
    //     lastTime1 = millis();
    //   }

    // this is our 'wait for incoming subscription packets' busy subloop
    // Adafruit_MQTT_Subscribe *subscription;
    // while ((subscription = mqtt.readSubscription(100))) {
    //     if (subscription == &mqttON_OFFobject) {
    //         ON_OFF = atof((char *)mqttON_OFFobject.lastread);
    //         Serial.printf("Received %i from Adafruit.io feed FeedNameB \n", ON_OFF);
    //     }
    // }

    long RangeInCentimeters;

    currentTime1 = millis();
    if ((currentTime1 - lastTime1) > 5000) {
        Serial.println("The distance to obstacles in front is: ");
        RangeInCentimeters = ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
                                                                // Serial.print(RangeInCentimeters);                       // 0~400cm

        // if (mqtt.Update()) {
        //     mqttObj1.publish(RangeInCentimeters);
        //     Serial.printf("Publishing  %i", RangeInCentimeters);
        //     Serial.println("cm\n");
        //     lastTime1 = millis();
        // }
    }

    currentTime2 = millis();
    if ((currentTime2 - lastTime2) > 1000) {

        flameSensor = analogRead(FLAMEPIN);
        if (flameSensor < 1500) {
            Serial.printf("Flame Detected!");
            // if (mqtt.Update()) {
            //     mqttObj2.publish(flameSensor);
            //     Serial.printf("Publishing flameSensor:%i \n", flameSensor);
            // }
        }
        // Serial.printf("flame sensor:%i DIGITAL:%i\n", flameSensor, FLAMEPINDIGITAL);
        lastTime2 = millis();
    }
    currentTime3 = millis();
    if ((currentTime3 - lastTime3) > 5000) {
        mq4Analog = analogRead(MQ4ANALOGPIN);
        // if (mqtt.Update()) {
        //     mqttObj3.publish(mq4Analog);
        //     Serial.printf("Publishing Methane level:: %i \n", mq4Analog);
        // }

        // indoor reading: 1800-2100 by an exhaust is around 3200-3400
        Serial.printf("mq4 Analog Read:%i\n", mq4Analog);
        lastTime3 = millis();
    }

    diodeState = analogRead(PHOTODIODEPIN);
    nightLed = map(diodeState, 120, 195, 255, -0);

    currentTime4 = millis();
    if ((currentTime4 - lastTime4) > 5000) {
        // if (mqtt.Update()) {
        //     mqttObj4.publish(nightLed);
        //     Serial.printf("Publishing Daylight Sensor:%i \n", diodeState);
        // }
        lastTime4 = millis();
    }
    currentTime5 = millis();
    if ((currentTime5 - lastTime5) > 1000) {
        analogWrite(LEDPIN, nightLed);
        lastTime5 = millis();
    }

    aqSensor = analogRead(AQPIN);
    currentTime6 = millis();
    if ((currentTime6 - lastTime6) > 10000) {
        Serial.printf("Air Quality Sensor:%i  Temp:%0.2f\n", aqSensor, tempF);
        lastTime6 = millis();
    }

    if (tempF > 71 && aqSensor < 3000) {
        digitalWrite(FANPIN, 1);
    } else {
        digitalWrite(FANPIN, 0);
    }

    
}

// void MQTT_connect() {
//     int8_t ret;

//     // Stop if already connected.
//     if (mqtt.connected()) {
//         return;
//     }

//     Serial.print("Connecting to MQTT... ");

//     while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
//         Serial.printf("%s\n", (char *)mqtt.connectErrorString(ret));
//         Serial.printf("Retrying MQTT connection in 5 seconds..\n");
//         // mqtt.disconnect();
//         delay(5000); // wait 5 seconds
//     }
//     Serial.printf("MQTT Connected!\n");
// }
