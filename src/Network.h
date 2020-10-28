#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <ESP8266WiFi.h>
#include <Hash.h>
#include <Settings.h>
#include <WiFiClient.h>
#include <types.h>

#define ST_CONN_TIMEOUT 10000
#define ST_RETRIES 3

namespace Network {
// WiFiClient net; Don't think I need it
enum MODES { AP, ST, DISCONNECTED };
byte getMode();
void beginAP();
bool beginST(const char* ssid, const char* pwd);
void init(String ssid, String pwd);
void checkAlive();
}  // namespace Network

#endif
