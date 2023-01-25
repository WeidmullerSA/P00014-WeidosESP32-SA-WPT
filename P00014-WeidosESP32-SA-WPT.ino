/**
 * @copyright 
* Copyright (C) 2023 Weidmüller SA. All rights reserved.
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/ 

/**
 * @file WeidosESP32-WasteProcessing.ino
 *
 * @brief This sketch is Waste Processing Demo
 *
 * @author Daniela Zaffalon.
 * Contact: daniela.zaffalon@weidmueller.com
 */

#include "src/wifiConnection/wifiConnection.h"
#include <SPI.h>
#include <MFRC522.h>

extern int actualUserID;
extern User users[2];

MFRC522 mfrc522(CS, MF_0);

void obtainUserID(byte* buffer, byte bufferSize, byte* userID);
bool validateID(byte* id);

static const uint32_t startButton = pin25;
static const uint32_t stopButton = pin26;
static const uint32_t doorSensor = pin27;
static const uint32_t fullSensor = pin28;
static const uint32_t loadCell = pin18; 

static const uint32_t yellowLed = pin45;
static const uint32_t greenLed = pin55; 
static const uint32_t redLed = pin56;
static const uint32_t doorLock = pin57;
static const uint32_t motor = pin58;

unsigned long startTime;
bool startButtonState = false;
bool stopButtonState = false;
bool fullButtonPressed = false;
unsigned long delayTime = 0;
int loginTimeout = 50000;
int processTimeout = 5000;
int processSwitchState = 0;

byte userID[] = {0x00,0x00,0x00,0x00};

void startISR() {
   if(processSwitchState == 3 || processSwitchState == 4) startButtonState = true;
}

void stopISR() {
   if(4 <= processSwitchState <= 6) {
        stopButtonState = true;
        processSwitchState = 8;
   } 
}

void fullISR() {
    fullButtonPressed = true;
    processSwitchState = 11;
}

void setup(){
    Serial.begin(115200);
    createTask();
    lcdSwitchState = 1;
    processSwitchState = 0;
    delay(5000);
    pinMode(yellowLed, OUTPUT);
    pinMode(motor, OUTPUT);
    digitalWrite(greenLed,LOW);
    digitalWrite(redLed,LOW);
    digitalWrite(yellowLed,LOW);
    digitalWrite(doorLock,HIGH);
    attachInterrupt(startButton, startISR, RISING);
    attachInterrupt(stopButton, stopISR, RISING);
    attachInterrupt(fullSensor, fullISR, CHANGE);
    SPI.begin();
	mfrc522.PCD_Init();
	delay(4);
}

void loop(){
    unsigned long actualTime = millis();
    bool doorSensorState = digitalRead(doorSensor);
    int loadCellValue = analogRead(loadCell);
    bool fullButtonState = digitalRead(fullSensor);
    if(WiFi.status() != WL_CONNECTED) wifiConnect();
	
    switch (processSwitchState){
        case 0: // Waiting for RFID CARD
            digitalWrite(redLed,LOW);
            digitalWrite(greenLed,LOW);
            digitalWrite(yellowLed,LOW);
            if(mfrc522.PICC_IsNewCardPresent()) {
                lcdSwitchState = 4; 
                processSwitchState = 1;
            }
        break;
        case 1: // RFID Validation
            if (mfrc522.PICC_ReadCardSerial() ) {
                obtainUserID(mfrc522.uid.uidByte, mfrc522.uid.size, userID);
                delay(2000);
                if(validateID(userID[3])) {
                    processSwitchState = 3;
                    lcdSwitchState = 5;
                }
                else {
                    lcdSwitchState = 6;
                    digitalWrite(redLed,HIGH);
                    processSwitchState = 2;
                }
            }
            else {
                processSwitchState = 0;
                lcdSwitchState = 3;
            }
            break;
        case 2: //Invalid User
            delay(2000);
            digitalWrite(redLed,LOW);
            lcdSwitchState = 3;
            processSwitchState = 0;
            break;
        case 3:
            delay(2000);
            digitalWrite(doorLock,LOW);
            digitalWrite(greenLed,HIGH);
            lcdSwitchState = 7;
            processSwitchState = 4;
            startTime = millis();
            break;
        case 4:  // Valid User
            if(doorSensorState && startButtonState){
                loadCellVoltage = loadCellValue*10.0f/4095;
                lcdSwitchState = 8; 
                processSwitchState = 5;
                digitalWrite(greenLed,LOW);
                delay(2000);
                digitalWrite(doorLock,HIGH);
                delay(1000);
                digitalWrite(yellowLed,HIGH);
                delayTime = 3000;
                startTime = millis();
                startButtonState = false;
            }
            else if(actualTime-startTime >= loginTimeout){
                digitalWrite(doorLock,HIGH);
                digitalWrite(greenLed,LOW);
                digitalWrite(redLed,HIGH);
                lcdSwitchState = 9;
                delay(2000);
                startTime = millis();
                digitalWrite(redLed,LOW);
                lcdSwitchState = 3;
                processSwitchState = 0;
            }
            else if (!doorSensorState && startButtonState){
                lcdSwitchState = 10;
                startButtonState = false;
            }  
            break;
        case 5: // Weighing
            if(actualTime-startTime >= delayTime){
                processSwitchState = 6;
                digitalWrite(motor, HIGH);
                startTime = millis();
                lcdSwitchState = 11;
            }
            break;
        case 6: // DOING THE PROCESS
            if(actualTime-startTime >= processTimeout){
                digitalWrite(motor, LOW);
                digitalWrite(yellowLed,LOW);
                processSwitchState = 7;
                users[actualUserID].addPoints((int)loadCellVoltage);
                lcdSwitchState = 12;
            }
            break;
        case 7: // End process summary and start again
            delay(5000);
            processSwitchState = 0;
            lcdSwitchState = 3;
            break;
        case 8: // STOP BUTTON PRESSED
            digitalWrite(greenLed,LOW);
            digitalWrite(yellowLed,LOW);
            digitalWrite(redLed,HIGH);
            digitalWrite(motor, LOW);
            digitalWrite(doorLock,LOW);
            stopButtonState = false; 
            lcdSwitchState = 13; 
            processSwitchState = 9;
            delayTime = 5000;
            startTime = millis();
            break;
        case 9: // STOP BUTTON PRESSED
            if(actualTime-startTime >= delayTime){
                lcdSwitchState = 14;
                processSwitchState = 10;
            }            
        break;
        case 10: // STOP BUTTON PRESSED 
            if(doorSensorState){ 
                digitalWrite(doorLock,HIGH);
                digitalWrite(redLed,LOW);
                processSwitchState = 0;
                lcdSwitchState = 3;
            }
        break;
        case 11: // CONTAINER FULL 
            if(fullButtonState && fullButtonPressed){
                lcdSwitchState = 15;
                fullButtonPressed = false; 
                digitalWrite(redLed,LOW);
                digitalWrite(greenLed,LOW);
                digitalWrite(yellowLed,HIGH);
                digitalWrite(doorLock,HIGH);
                digitalWrite(motor,LOW);
            }
            else if(!fullButtonState && fullButtonPressed){
                fullButtonPressed = false;
                processSwitchState = 0;
                lcdSwitchState = 3;
            }
            else {}
        break;
        default:
            break;
    }
}

void obtainUserID(byte* buffer, byte bufferSize, byte* userID) {
  for (byte i = 0; i < bufferSize; i++) {
    userID[i] = buffer[i];
  }
}

bool validateID(byte id){
    for (byte i = 0; i < sizeof(users); i++) {
        if(users[i].getID() == id) {
            actualUserID = i;
            return true;
        }
    }
    return false;
}
