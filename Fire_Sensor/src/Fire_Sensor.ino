/*
 * Project Fire_Sensor
 * Description:
 * Author: Thomas abeyta
 * Date:
 */

SYSTEM_MODE(SEMI_AUTOMATIC);
const int FLAMEPIN = A2;
const int FLAMEPINDIGITAL = D6;
int flameSensor;
int currentTime, lastTime;

void setup() {
    pinMode(FLAMEPIN, INPUT);
}

void loop() {
    currentTime = millis();
    if ((currentTime - lastTime) > 1000) {
        
        flameSensor = analogRead(FLAMEPIN);
        if (flameSensor<1500){
          Serial.printf("Flame Detected!");
        }
        Serial.printf("flame sensor:%i DIGITAL:%i\n", flameSensor, FLAMEPINDIGITAL);
        lastTime = millis();
    }
}
