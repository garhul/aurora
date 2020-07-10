
#include <WebServer.h>

namespace WebServer {
  ESP8266WebServer server(80);
  void (*cmdHandler)(String cmd, String payload);

  void init(void (*f)(String,String)) {
    server.on("/setup", HTTP_POST, _setup);
    server.on("/cmd", HTTP_POST, _cmd);
    server.on("/clear", HTTP_POST, _clearCredentials); //endpoint for clearing ssid / pwd
    server.on("/info", HTTP_ANY, _info);
    server.on("/", HTTP_ANY, _control);
    server.onNotFound(_info);
    server.begin();
    cmdHandler = f;
  }

  void loop() {
    server.handleClient();
  }

  void _cmd() {
    char cmd[32];
    char payload[32] = "";

    if(!server.hasArg("cmd")) {
      server.send(400, "application/json", "{\"message\":\"Missing command[cmd] parameter\"}");
      return;
    }

    if(server.hasArg("payload")) {
      server.arg("payload").toCharArray(payload, 32);
    }

    server.arg("cmd").toCharArray(cmd, 32);
    cmdHandler(String(cmd), String(payload));
    server.send(200, "application/json", "{\"message\":\"ok\"}");
  }

  void _setup() {
    settings_t settings = {};
    
    //check post params exist
    if(
        !server.hasArg("pass") ||
        !server.hasArg("ssid") ||
        !server.hasArg("topic")||
        !server.hasArg("broker") ||
        !server.hasArg("announce_topic") ||
        !server.hasArg("human_name") ||
        !server.hasArg("ap_ssid") ||
        !server.hasArg("strip_size")
      ) {
      server.send(400, "text/plain", "Invalid params one of [pass, ssid, topic, broker, announce_topic, human_name, ap_ssid, strip_size] is missing");
      return;
    }

    if (!(Utils::clearStorage())) {
      server.send(500, "text/plain", "Error clearing credentials");
      return;
    };

    server.arg("ssid").toCharArray(settings.ssid, 32);
    server.arg("pass").toCharArray(settings.pass, 32);
    server.arg("topic").toCharArray(settings.topic, 32);
    server.arg("broker").toCharArray(settings.broker, 32);
    server.arg("announce_topic").toCharArray(settings.announce_topic, 16);
    server.arg("ap_ssid").toCharArray(settings.ap_ssid, 32);
    server.arg("human_name").toCharArray(settings.human_name, 32);
    settings.strip_size = server.arg("strip_size").toInt();

    if (Utils::storeSettings(settings)) {
      server.send(200, "text/plain", "Settings stored");
      delay(2000);
      ESP.restart();
      return;
    }

    server.send(500, "text/plain", "Unable to store settings");
  }

  void _control() {
    if (! SPIFFS.exists("/index.html")) {
      server.send(404,"text/plain", "File not found");
      return ;
    }
    
    File f = SPIFFS.open("/index.html", "r");  
    if (!f) {
      server.send(500, "text/plain", "Error opening file");
      return;
    }

    //read the file in chunks (not that much ram)
    server.streamFile(f, "text/html");
    f.close();
  }

  void _info() {
    server.send(200, "application/json", Utils::getInfoJson());
  }

  void _clearCredentials() {
    if (Utils::clearStorage()) {
      server.send(200, "text/plain", "EEPROM cleared!");
      return;
    }

    server.send(500, "text/plain", "Error clearing EEEPROM'}");
  }
}
