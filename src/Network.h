#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <ESP8266WiFi.h>
#include <Hash.h>
#include <Settings.h>
#include <WiFiClient.h>

#define ST_CONN_TIMEOUT   10000
#define ST_RETRIES        3
#define WIFI_CHECK_PERIOD 180 * 1000 // Check every 3 minutes 180 000 mSec

namespace Network {
  // WiFiClient net; Don't think I need it
  enum MODES { AP,
               ST,
               DISCONNECTED };
  byte getMode();
  void beginAP();
  bool beginST();
  void init();
  void checkAlive();
} // namespace Network

#endif
