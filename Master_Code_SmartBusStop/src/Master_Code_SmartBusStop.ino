/*
 * Project Master_Code_SmartBusStop
 * Description:
 * Author:
 * Date:
 */
#include "Grove-Ultrasonic-Ranger.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT_SPARK.h"
#include "TCreds.h"

const int FLAMEPIN = A2;
const int FLAMEPINDIGITAL = D6;
const int MQ4ANALOGPIN = A3;
const int MQ4DIGITALPIN = D5;
const int DIODEPIN = A4;
const int LEDPIN = A5;

int flameSensor;
int currentTime, currentTime1, currentTime2, currentTime3, currentTime4, currentTime5;
int lastTime, lastTime1, lastTime2, lastTime3, lastTime4,lastTime5;
int mq4Digital, mq4Analog;
int diodeState;
int nightLed;

// delete after published is established
int value1 = 102; 
int ON_OFF;
struct geo{
    float lat;
    float lon;
    int alt;  
    };

geo myLoc;
geo locations[13];




Ultrasonic ultrasonic(A2);

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//Adafruit_MQTT_Publish mqttObj = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/abq_gps");
Adafruit_MQTT_Publish mqttObj1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Trash_can_Fullness");
Adafruit_MQTT_Publish mqttObj2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Fire_Sensor");
Adafruit_MQTT_Publish mqttObj3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Methane_Sensor");
Adafruit_MQTT_Publish mqttObj4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Daylight_Sensor");


Adafruit_MQTT_Subscribe mqttON_OFFobject = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ON_OFF");

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    Serial.begin(9600);
    pinMode(FLAMEPIN, INPUT);
    pinMode(MQ4ANALOGPIN, INPUT);
    pinMode(MQ4DIGITALPIN, INPUT);
    pinMode(DIODEPIN, INPUT);
    pinMode(LEDPIN, OUTPUT);

     WiFi.connect();
    while (WiFi.connecting()) {
        Serial.printf(".");
    }

     // Setup MQTT subscription for onoff feed.
    mqtt.subscribe(&mqttON_OFFobject);
}
void loop() {

   MQTT_connect();
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
    if ((currentTime1 - lastTime1) > 1000) {
        Serial.println("The distance to obstacles in front is: ");
        RangeInCentimeters = ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
        //Serial.print(RangeInCentimeters);                       // 0~400cm
        

         if(mqtt.Update()) {
        mqttObj1.publish(RangeInCentimeters);
        Serial.printf("Publishing  %i \n",RangeInCentimeters);
        Serial.println(" cm");
        lastTime1 = millis();
         }
    }

    currentTime2 = millis();
    if ((currentTime2 - lastTime2) > 1000) {

        flameSensor = analogRead(FLAMEPIN);
        if (flameSensor < 1500) {
            Serial.printf("Flame Detected!");
            if(mqtt.Update()) {
            mqttObj2.publish(flameSensor);
            Serial.printf("Publishing flameSensor:%i \n",flameSensor);
        }
        }
        //Serial.printf("flame sensor:%i DIGITAL:%i\n", flameSensor, FLAMEPINDIGITAL);
        lastTime2 = millis();
    }
    currentTime3= millis();
    if ((currentTime3 - lastTime3) > 2000) {
        mq4Analog = analogRead(MQ4ANALOGPIN);
        mq4Digital = analogRead(MQ4DIGITALPIN);
        if(mqtt.Update()) {
        mqttObj3.publish(mq4Analog);
        Serial.printf("Publishing MQ4ANALOGPIN: %i \n",mq4Analog);
        }

        // indoor reading: 1800-2100 by an exhaust is around 3200-3400
        Serial.printf("mq4 Analog Read:%i\n", mq4Analog);
        lastTime3 = millis();
    }

    diodeState = analogRead(DIODEPIN);
    nightLed = map(diodeState,120,195,255,-0);

    currentTime4 = millis();
    if((currentTime4-lastTime4)>2000){
      if(mqtt.Update()) {
        mqttObj4.publish(diodeState, nightLed);
        Serial.printf("Publishing diodeState%inightLed:%i \n",diodeState, nightLed);
        }
    Serial.printf("diode State:%i conversion:%i\n", diodeState,nightLed);
    lastTime4 =millis();
    }
     analogWrite(LEDPIN,nightLed);
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
        //mqtt.disconnect();
        delay(5000); // wait 5 seconds
    }
    Serial.printf("MQTT Connected!\n");
}
