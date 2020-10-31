#include <WebServer.h>

namespace WebServer {
  ESP8266WebServer server(80);
  void(*cmdHandler)(String cmd, String payload);

  void init(void(*f)(String, String)) {
    server.on("/setup", HTTP_POST, _setup);
    server.on("/cmd", HTTP_POST, _cmd);
    server.on("/clear", HTTP_POST, _clearCredentials); // endpoint for clearing ssid / pwd
    server.on("/info", HTTP_ANY, _info);
    server.on("/", HTTP_ANY, _control);
    server.onNotFound(_info);
    server.begin();
    cmdHandler = f;
  }

  void loop() {
    server.handleClient();
  }

  void _respond(int code, const char* message) {
    String msg = "{\"msg\":\"" + String(message) + "\"}";
    server.send(code, "application/json", msg.c_str());
  }

  void _cmd() {
    char cmd[32];
    char payload[32] = "";

    if (!server.hasArg("cmd")) {
      _respond(400, "Missing command[cmd] parameter");
      return;
    }

    if (server.hasArg("payload")) {
      server.arg("payload").toCharArray(payload, 32);
    }

    server.arg("cmd").toCharArray(cmd, 32);
    cmdHandler(String(cmd), String(payload));
    _respond(200, "ok");
  }

  void _setup() {
    // check post params exist
    String requiredArgs[9] = {
      "pass",
      "ssid",
      "topic",
      "broker",
      "announce_topic",
      "human_name",
      "ap_ssid",
      "strip_size",
      "use_mqtt"
    };

    for (byte i = 0; i < 9; i++) {
      if (!server.hasArg(requiredArgs[i])) {
        _respond(400, String("Missing parameter [" + requiredArgs[i] + "]").c_str());
        return;
      }
    }

    server.arg("ssid").toCharArray(Settings::ssid, 32);
    server.arg("pass").toCharArray(Settings::pass, 32);
    server.arg("topic").toCharArray(Settings::topic, 32);
    server.arg("broker").toCharArray(Settings::broker, 32);
    server.arg("announce_topic").toCharArray(Settings::announce_topic, 32);
    server.arg("ap_ssid").toCharArray(Settings::ap_ssid, 32);
    server.arg("human_name").toCharArray(Settings::human_name, 32);
    Settings::strip_size = server.arg("strip_size").toInt();
    Settings::use_mqtt = (server.arg("use_mqtt").equalsIgnoreCase("true")) ? true : false;

    if (Settings::store()) {
      _respond(200, "Settings stored");
      delay(2000);
      ESP.restart();
      return;
    }

    _respond(500, "Unable to store settings");
  } // namespace WebServer

  void _control() {
    if (!LittleFS.exists("/index.html")) {
      _respond(404, "File not found");
      return;
    }

    File f = LittleFS.open("/index.html", "r");
    if (!f) {
      _respond(500, "Error opening file");
      return;
    }

    // read the file in chunks (not that much ram)
    server.streamFile(f, "text/html");
    f.close();
  }

  void _info() {
    _respond(200, Settings::getInfoJson().c_str());
  }

  void _clearCredentials() {
    if (Settings::clear()) {
      _respond(200, "EEPROM cleared!");
      return;
    }

    _respond(500, "Error clearing EEEPROM'}");
  }
} // namespace WebServer
