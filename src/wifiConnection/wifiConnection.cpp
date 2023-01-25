#include "wifiConnection.h"
#include "../display/lcdTaskCode.h"

const char *WIFI_SSID = SSID;
const char *WIFI_PASS = PASS;
int connectionTimeout = 2000;

bool wifiConnect(){
  unsigned long startTime = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("WiFi Status: ");
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    unsigned long actualTime = millis();
    if(actualTime-startTime >= connectionTimeout){
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      startTime = millis();
    }
    delay(100);
  }

  wifiIP = WiFi.localIP();
  lcdSwitchState = 2;
  Serial.print("Wifi Connected with IP: ");
  Serial.println(WiFi.localIP());
  return true;
}