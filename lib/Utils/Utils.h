#ifndef _UTILS_H_
#define _UTILS_H_

#define EEPROM_SIZE 512
#define SETTINGS_ADDR 0x00

#include <types.h>

namespace Utils {
  void initStorage();
  bool clearStorage();
  bool storeSettings(settings_t settings);
  settings_t getSettings();
}

#endif
