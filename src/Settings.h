#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define SETTINGS_FILE      "/settings"
#define DEFAULT_HUMAN_NAME "Aurora"
#define AP_USE_PWD         false
#define AP_PWD             "1234"
#define DEFAULT_STRIP_SIZE 1

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <version.h>

namespace Settings {
  extern char ssid[32];
  extern char pass[32];
  extern char topic[32];
  extern char announce_topic[32];
  extern char human_name[32];
  extern char broker[32];
  extern char ap_ssid[32];
  extern bool use_mqtt;
  extern unsigned int strip_size;

  void init();
  bool clear();
  bool store();
  void load();
  String getInfoJson();
  String getAnnounceInfo();
  String getLastWillInfo();
  String getDeviceId();
  String getCommands();
} // namespace Settings

#endif
