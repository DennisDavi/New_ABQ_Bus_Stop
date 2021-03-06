/*
 * Project MP3player
 * Description: Argon-based code for DFPlayer Mini Player
 *              Note: DFRobotDFPlayer.cpp modified to work on Argon
 * Author: Thomas Abeyta
 * Date: 20-APR-2022
 */

#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
const int PLAYTIME = 10000;
const int NUMBERTRACKS = 1;
const int BUTTONSTATE = D4;
bool status2;
int p, timer3;
int button;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
    Serial1.begin(9600);

    pinMode(BUTTONSTATE, INPUT);

    status2 = myDFPlayer.begin(Serial1, false);

    myDFPlayer.volume(25); // Set volume value. From 0 to 30
    p = 0;
    timer3 = -PLAYTIME;
}

void loop() {

    // button = digitalRead(BUTTONSTATE);
    // if (button == 1) {
    //     if (millis() - timer3 > PLAYTIME) {
    //         timer3 = millis();
    //         p++;
    //         if (p > NUMBERTRACKS) {
    //             p = 1;
    //         }
    //         Serial.printf("Play Next - Track\n");
            myDFPlayer.play(1); // Play next mp3 every 3 second.
            delay(6000);
    //     }
    // }

    // if (myDFPlayer.available()) {
    //   printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    // }
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