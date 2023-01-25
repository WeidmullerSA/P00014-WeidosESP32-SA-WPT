#include "../users/users.h"
#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>

#ifndef LCDTASKCODE_H
#define LCDTASKCODE_H

extern int lcdSwitchState;
extern float loadCellVoltage;
extern IPAddress wifiIP;

void createTask();
void lcdTaskcode( void * parameter);

#endif