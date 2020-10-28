#include "Settings.h"

namespace Settings {
  settings_t settings = {};

  void init() {
    LittleFS.begin();

    FSInfo fsi;
    if (LittleFS.info(fsi)) {
      Serial.printf("Found %d bytes filesystem\n", fsi.totalBytes);
      Serial.printf("%d bytes free\n", (fsi.totalBytes - fsi.usedBytes));
    } else {
      Serial.println("No fs found, formatting");
      LittleFS.format();
    }

    Settings::get();
  }

  String _stToJSON(settings_t s) {
    const size_t capacity = JSON_OBJECT_SIZE(7) + 320;
    DynamicJsonDocument doc(capacity);
    String buff;

    doc["ssid"] = s.ssid;
    doc["pass"] = s.pass;
    doc["human_name"] = s.human_name;
    doc["ap_ssid"] = s.ap_ssid;
    doc["topic"] = s.topic;
    doc["announce_topic"] = s.announce_topic;
    doc["broker"] = s.broker;

    serializeJson(doc, buff);

    return buff;
  }

  String _settingsAsJSON() { return _stToJSON(settings); }

  bool clear() {
    if (LittleFS.exists(SETTINGS_FILE)) {
      return LittleFS.remove(SETTINGS_FILE);
    }

    return true;
  }

  bool store(settings_t s) {
    Serial.println(s.ssid);
    Serial.println(s.ap_ssid);
    String sts = _stToJSON(s);
    Serial.println("Unable to write file contents: " + sts);

    File sf = LittleFS.open(SETTINGS_FILE, "w+");
    if (sf && sf.write(sts.c_str()) > 0) {
      sf.close();
      Settings::get();
      return true;
    }

    Serial.println("Unable to write file contents: " + sts);
    sf.close();

    return false;
  }

  void _loadDefaults() {
    Serial.println("Settings file not found, using defaults");
    strcpy(settings.ssid, DEFAULTS_SSID);
    strcpy(settings.broker, "");
    strcpy(settings.human_name, DEFAULTS_HUMAN_NAME);
    strcpy(settings.pass, DEFAULTS_PASS);
    strcpy(settings.topic, "");
    strcpy(settings.announce_topic, "");
    strcpy(settings.ap_ssid, getDeviceId().c_str());
  }

  settings_t get() {
    if (LittleFS.exists(SETTINGS_FILE)) {
      File sf = LittleFS.open(SETTINGS_FILE, "r+");
      if (!sf) {
        Serial.println("File open failed");
        _loadDefaults();
        return settings;
      }

      size_t filesize = sf.size();
      uint8_t *buff = (uint8_t *)malloc(filesize + 1);
      sf.read(buff, filesize);
      buff[filesize] = '\0';

      const size_t capacity = JSON_OBJECT_SIZE(7) + 320;
      DynamicJsonDocument doc(capacity);

      deserializeJson(doc, buff);
      strcpy(settings.ssid, doc["ssid"]);
      strcpy(settings.pass, doc["pass"]);
      strcpy(settings.human_name, doc["human_name"]);
      strcpy(settings.ap_ssid, doc["ap_ssid"]);
      strcpy(settings.topic, doc["topic"]);
      strcpy(settings.announce_topic, doc["announce_topic"]);
      strcpy(settings.broker, doc["broker"]);
    } else {
      _loadDefaults();
    }
    return settings;
  }

  String getDeviceId() {
    return String(settings.human_name) + "_" + String(ESP.getChipId(), HEX);
  }

  String getAnnounceInfo() {
    return getDeviceId() + "|" + WiFi.localIP().toString() + "|" +
           String(settings.human_name);
  }

  String getInfoJson() {
    String info = "{\"ssid\":\"" + String(settings.ssid) + "\",\"ap_ssid\":\"" +
                  String(settings.ap_ssid) + "\",\"human_name\":\"" +
                  String(settings.human_name) + "\",\"announce_topic\":\"" +
                  String(settings.announce_topic) + "\",\"device_id\":\"" +
                  String(getDeviceId()) + "\",\"broker\":\"" +
                  String(settings.broker) + "\",\"topic\":\"" +
                  String(settings.topic) + "\",\"build\":\"" + VERSION +
                  "\",\"ip\":\"" + WiFi.localIP().toString() + "\"}";

    return info;
  }
}  // namespace Settings
