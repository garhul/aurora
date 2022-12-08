#include <Network.h>

namespace Network {
  byte mode;

  void init() {
    mode = MODES::DISCONNECTED;
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    if (beginST()) {
      mode = MODES::ST;
    } else {
      mode = MODES::AP;
      beginAP();
    }
  }

  byte getMode() {
    return mode;
  }

  void beginAP() {
    WiFi.mode(WIFI_AP);

#if AP_USE_PWD
    WiFi.softAP(AP_SSID, AP_PWD);
#else
    WiFi.softAP(String(Settings::ap_ssid));
#endif

    WiFi.printDiag(Serial);
    Serial.println(WiFi.softAPIP());
  }

  bool beginST() {
    int attempts = 0;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true); // remove any old wifi config

    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect(false);
      delay(2000);
    }

    while (WiFi.status() != WL_CONNECTED && attempts < ST_RETRIES) {
      WiFi.begin(Settings::ssid, Settings::pass);
      attempts++;
      delay(ST_CONN_TIMEOUT);
    }

    // are we connected yet ?
    if (WiFi.status() != WL_CONNECTED)
      return false;

    Serial.println("Station startup successful");
    WiFi.printDiag(Serial);
    Serial.println(WiFi.localIP());

    return true;
  }

  /**
   * checks the connection is still alive, if not resets the device
   */
  void checkAlive() {
    static unsigned long check_due = 0;
    unsigned long t = millis();

    if (t < check_due)
      return;

    check_due = t + WIFI_CHECK_PERIOD;

    if (mode == MODES::ST && !WiFi.isConnected()) {
      Serial.println("Wifi disconnected, attempting to reconnect");
      beginST();
    }
  }
} // namespace Network
