
#include <Utils.h>

namespace Utils {
  void initStorage(){
    EEPROM.begin(EEPROM_SIZE);
  }

  bool clearStorage() {   
    for (int i = 0; i < EEPROM_SIZE; ++i) {
      EEPROM.write(i, 1);
    }

    return EEPROM.commit();
  }

  bool storeSettings(settings_t settings) {
    EEPROM.put(SETTINGS_ADDR, settings);
    return EEPROM.commit();
  }

  settings_t getSettings() {
    settings_t settings = {};
    EEPROM.get(0x00, settings);
    return settings;
  }
  
  String getDeviceId() {
    return String("Aurora_") + String(ESP.getChipId(), HEX);
  }

  String getInfoJson() {
    settings_t settings = getSettings();
    String info = "{\"settings\":{\"ssid\":\"" + String(settings.ssid) + 
    "\",\"ap_ssid\":\"" + String(settings.ap_ssid) +
    "\",\"human_name\":\"" + String(settings.human_name) +
    "\",\"announce_topic\":\"" + String(settings.announce_topic) +
    "\",\"device_id\":\"" + String(getDeviceId()) +
    "\",\"broker\":\"" + String(settings.broker) +
    "\" ,\"topic\":\"" + String(settings.topic) +
    "\",\"strip_size\":\"" + String(settings.strip_size, DEC) +    
    "\"}}";

    return info;
  }
}
