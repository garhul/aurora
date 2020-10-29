#include <Network.h>

namespace Network {
  byte mode;
  boolean isDefaultSsid;
  boolean isDefaultPass;

  void init(String ssid, String pwd) {
    mode = MODES::DISCONNECTED;
    isDefaultSsid = strcmp(ssid.c_str(), DEFAULTS_SSID) == 0;
    isDefaultPass = strcmp(pwd.c_str(), DEFAULTS_PASS) == 0;

    if (isDefaultSsid && isDefaultPass) {
      mode = MODES::AP;
      beginAP();
      return;
    }

    if (beginST(ssid.c_str(), pwd.c_str())) {
      mode = MODES::ST;
    }
  }

  byte getMode() { return mode; }

  void beginAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(Settings::settings.ap_ssid, Settings::settings.pass);
    WiFi.printDiag(Serial);
    Serial.println(WiFi.softAPIP());
  }

  bool beginST(const char* ssid, const char* pwd) {
    int attempts = 0;

    WiFi.mode(WIFI_STA);

    // disconnect from any previously connected network (open networks?)
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect(false);
      delay(2000);
    }

    while (WiFi.status() != WL_CONNECTED && attempts < ST_RETRIES) {
      WiFi.begin(ssid, pwd);
      attempts++;
      delay(ST_CONN_TIMEOUT);
    }

    // are we connected yet ?
    if (WiFi.status() != WL_CONNECTED) return false;

    Serial.println("Station startup successful");
    WiFi.printDiag(Serial);
    Serial.println(WiFi.localIP());

    return true;
  }

  /**
   * checks the connection is still alive, if not resets the device
   */
  void checkAlive() {
    if (!WiFi.isConnected()) ESP.reset();
  }
}  // namespace Network
