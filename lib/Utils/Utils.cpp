
#include <EEPROM.h>
#include <Utils.h>
#include <types.h>
/** completely clear eeprom **/

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
}
