#include "Settings.h"

namespace Settings {
  char ssid[32];
  char pass[32];
  char topic[32];
  char announce_topic[32];
  char human_name[32];
  char broker[32];
  char ap_ssid[32];
  bool use_mqtt;
  unsigned int strip_size;

  void init() {
    LittleFS.begin();

    FSInfo fsi;
    if (LittleFS.info(fsi)) {
      Serial.printf("Found %d bytes filesystem\n", fsi.totalBytes);
      Serial.printf("%d bytes free\n", (fsi.totalBytes - fsi.usedBytes));
    }
    else {
      Serial.println("No fs found, formatting");
      LittleFS.format();
    }

    Settings::load();
  }

  bool clear() {
    if (LittleFS.exists(SETTINGS_FILE)) {
      return LittleFS.remove(SETTINGS_FILE);
    }

    return true;
  }

  bool store() {
    const size_t capacity = JSON_OBJECT_SIZE(7) + 450;
    DynamicJsonDocument doc(capacity);
    String buff;

    doc["ssid"] = ssid;
    doc["pass"] = pass;
    doc["human_name"] = human_name;
    doc["ap_ssid"] = ap_ssid;
    doc["topic"] = topic;
    doc["announce_topic"] = announce_topic;
    doc["broker"] = broker;
    doc["use_mqtt"] = use_mqtt;
    doc["strip_size"] = strip_size;

    serializeJson(doc, buff);

    Serial.println("Storing settings" + buff);

    File sf = LittleFS.open(SETTINGS_FILE, "w+");
    if (sf && sf.write(buff.c_str()) > 0) {
      sf.close();
      Settings::load();
      return true;
    }

    Serial.println("Unable to write file contents!");
    sf.close();

    return false;
  }

  void _loadDefaults() {
    strcpy(ssid, "");
    strcpy(broker, "");
    strcpy(human_name, DEFAULT_HUMAN_NAME);
    strcpy(pass, "");
    strcpy(topic, "");
    strcpy(announce_topic, "");
    strcpy(ap_ssid, (String(DEFAULT_HUMAN_NAME) + "_" + String(ESP.getChipId(), HEX)).c_str());
    strip_size = DEFAULT_STRIP_SIZE;
    use_mqtt = false;
  }

  void load() {
    _loadDefaults();

    if (LittleFS.exists(SETTINGS_FILE)) {
      File sf = LittleFS.open(SETTINGS_FILE, "r+");
      if (!sf) {
        Serial.println("Settings file open failed");
        return;
      }

      size_t filesize = sf.size();
      uint8_t* buff = (uint8_t*) malloc(filesize + 1);
      sf.read(buff, filesize);
      buff[filesize] = '\0';

      const size_t capacity = JSON_OBJECT_SIZE(7) + 320;
      DynamicJsonDocument doc(capacity);

      deserializeJson(doc, buff);
      strcpy(ssid, doc["ssid"]);
      strcpy(pass, doc["pass"]);
      strcpy(human_name, doc["human_name"]);
      strcpy(ap_ssid, doc["ap_ssid"]);
      strcpy(topic, doc["topic"]);
      strcpy(announce_topic, doc["announce_topic"]);
      strcpy(broker, doc["broker"]);
      strip_size = doc["strip_size"];
      use_mqtt = doc["use_mqtt"];
    }
    else {
      Serial.println("Settings file not found, using defaults");
    }
    Serial.println("Loadded settings - info: " + getInfoJson());
  }

  String getDeviceId() {
    return String(human_name) + "_" + String(ESP.getChipId(), HEX);
  }

  String getAnnounceInfo() {
    return getDeviceId() + "|" + WiFi.localIP().toString() + "|" + String(human_name);
  }

  String getInfoJson() {
    const size_t capacity = JSON_OBJECT_SIZE(11) + 600;
    DynamicJsonDocument doc(capacity);
    String buff;

    doc["ssid"] = ssid;
    doc["ap_ssid"] = ap_ssid;
    doc["human_name"] = human_name;
    doc["announce_topic"] = announce_topic;
    doc["device_id"] = getDeviceId();
    doc["broker"] = broker;
    doc["topic"] = topic;
    doc["build"] = VERSION;
    doc["use_mqtt"] = use_mqtt;
    doc["strip_size"] = strip_size;
    doc["ip"] = WiFi.localIP().toString();

    serializeJson(doc, buff);
    return buff;
  }
} // namespace Settings
