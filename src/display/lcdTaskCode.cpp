#include "lcdTaskCode.h"

DFRobot_RGBLCD1602 lcd(16,2);
TaskHandle_t lcdTask;

int lcdSwitchState;
float loadCellVoltage;
IPAddress wifiIP;

User user1(0xA8, "John", "Doe");
User user2(0x90, "Jane", "Doe");
User users[2] = {user1,user2}; // A8= tarjeta
int actualUserID = 100;

void lcdTaskcode( void * parameter){
    vTaskDelay(100);
    lcd.init();
    lcd.setRGB(255,255,255);
    lcd.setCursor(0,0);
    for(;;){
        switch (lcdSwitchState) {
            case 0:
        
            break;
            case 1:
                lcdSwitchState = 0;
                lcd.setBacklight(true);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Waste Processor");
                lcd.setCursor(0,1);
                lcd.print("Initializing...");
            break;
            case 2:
                lcdSwitchState = 3;
                lcd.setBacklight(true);
                lcd.clear();
                lcd.print("Wifi Connected with IP: ");
                lcd.print(wifiIP);
                for (int positionCounter = 0; positionCounter < 40; positionCounter++) {
                    lcd.scrollDisplayLeft();
                    delay(400);
                }
            break;
            case 3:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Welcome to W.P.T.");
                lcd.setCursor(0,1);
                lcd.print((char)126);
                lcd.print(" Put the card ");
                lcd.print((char)127);
            break;
            case 4:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Please wait...");
            break;
            case 5:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(3,0);
                lcd.print("Hi ");
                lcd.print(users[actualUserID].getName());
                lcd.setCursor(0,1);
                lcd.print("You've ");
                lcd.print(users[actualUserID].getPoints());
                lcd.print(" Points");
            break;
            case 6:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(2,0);
                lcd.print("Invalid User");
            break;
            case 7:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Door Unlocked");
                lcd.setCursor(2,1);
                lcd.print("Press Start  ");
            break;
            case 8:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("START Processing");
                lcd.setCursor(0,1);
                lcd.print(loadCellVoltage,2);
                lcd.print(" Kg ");
                lcd.print((int)loadCellVoltage);
                lcd.print(" Points");
            break;
            case 9:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(2,0);
                lcd.print("Login Timeout");
            break;
            case 10:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("First close Door");
                lcd.setCursor(0,1);
                lcd.print(" & Press Start");
            break;
            case 11:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(3,0);
                lcd.print("PROCESSING ");
                lcd.setCursor(0,1);
                lcd.print("..Please Wait...");
            break;
            case 12:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Process END.");
                lcd.setCursor(0,1);
                lcd.print(users[actualUserID].getPoints());
                lcd.print(" Total Points");
                
                
            break;
            case 13:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("EMERGENCY STOP");
                lcd.setCursor(0,1);
                lcd.print("BUTTON PRESSED");
            break;
            case 14:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("PLEASE, CLOSE");
                lcd.setCursor(0,1);
                lcd.print("THE DOOR");
            break;
            case 15:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(" CONTAINER FULL");
            break;
            case 16:
                lcdSwitchState = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Connect to");
                lcd.setCursor(0,1);
                lcd.print("Weidos Network");
            break;
            default:
                lcd.setCursor(0,0);
                lcd.print("Default");
            break;
        }
    delay(10);
    }
}

void createTask(){
    xTaskCreatePinnedToCore(
    lcdTaskcode,
    "lcdTask", 
    10000,
    NULL,
    1,
    &lcdTask,
    0);
}
