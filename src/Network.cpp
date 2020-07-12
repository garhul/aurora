#include <Network.h>

namespace Network {
  byte mode;

  void init(String ssid, String pwd) {
    mode = MODES::DISCONNECTED;
    
    if (beginST(ssid.c_str(), pwd.c_str())) {
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
      settings_t st = Utils::getSettings();      
      WiFi.softAP(String(st.ap_ssid));
    #endif

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
      WiFi.begin (ssid, pwd);
      attempts++;
      delay(ST_CONN_TIMEOUT);
    }

    //are we connected yet ?
    if (WiFi.status() != WL_CONNECTED)
      return false;

    Serial.println("Station startup successful");
    WiFi.printDiag(Serial);
    Serial.println (WiFi.localIP());

    return true;
  }

  /**
   * checks the connection is still alive, if not resets the device
  */  
  void checkAlive() {
    if (!WiFi.isConnected()) ESP.reset();
  }
}
