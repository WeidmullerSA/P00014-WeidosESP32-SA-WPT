#include <esp_crt_bundle.h>
#include <WiFi.h>
#include "../Secrets.h"
#include "../display/lcdTaskCode.h"

#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

extern WiFiClass WiFi;

bool wifiConnect();

#endif