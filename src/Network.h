#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Hash.h>
#include <types.h>
#include <Utils.h>

#define AP_SSID "aurora_"
#define AP_USE_PWD false
#define AP_PWD "1234"
#define AP_DEVICE_NAME "aurora_"
#define ST_DEVICE_NAME "aurora"
#define ST_CONN_TIMEOUT 10000
#define ST_RETRIES 3


namespace Network {
  // WiFiClient net; Don't think I need it
  enum MODES {AP, ST, DISCONNECTED};
  byte getMode();
  void beginAP();
  bool beginST(const char* ssid, const char* pwd);
  void init(String ssid, String pwd);
  void checkAlive();
}

#endif
