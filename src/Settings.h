#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define SETTINGS_FILE "/settings"
#define DEFAULTS_HUMAN_NAME "Aurora"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <types.h>
#include <version.h>

namespace Settings
{
    extern settings_t settings;
    void init();
    bool clear();
    bool store(settings_t settings);
    settings_t get();
    String getInfoJson();
    String getAnnounceInfo();
    String getDeviceId();
} // namespace Settings

#endif