#include <WebServer.h>

namespace WebServer {
  ESP8266WebServer server(80);
  Strip* strip;

  void init(Strip* s) {
    server.on("/setup", HTTP_POST, _setup);
    server.on("/cmd", HTTP_POST, _cmd);
    server.on("/clear", HTTP_POST, _clearCredentials); // endpoint for clearing ssid / pwd
    server.on("/info", HTTP_ANY, _info);
    server.on("/state", HTTP_ANY, _getState);
    server.on("/restart", HTTP_ANY, _restart);
    server.on(SETTINGS_FILE, HTTP_ANY, _FORBIDDEN);
    server.serveStatic("/", LittleFS, "/");
    server.onNotFound(_info);
    server.begin();
    strip = s;
  }

  void loop() {
    server.handleClient();
  }

  void _respond(int code, const char* message) {
    String msg = "{\"msg\":\"" + String(message) + "\"}";
    server.send(code, "application/json", msg.c_str());
  }

  void _restart() {
    _respond(200, "Booting now");
    delay(2000);
    ESP.restart();
  }

  void _serveFile(const char* filepath, const char* doctype = "text/html") {
    if (!LittleFS.exists(filepath)) {
      _respond(404, "File not found");
      Serial.println("file not found");
      Serial.println(filepath);
      return;
    }

    File f = LittleFS.open(filepath, "r");
    if (!f) {
      _respond(500, "Error opening file");
      return;
    }

    // read the file in chunks (not that much ram)
    server.streamFile(f, doctype);
    f.close();
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
    strip->cmd(String(cmd), String(payload));
    _getState();
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
        "use_mqtt"};

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

  void _FORBIDDEN() {
    _respond(403, "Access Forbidden");
  }

  void _getState() {
    const size_t capacity = JSON_OBJECT_SIZE(5) + 40;
    DynamicJsonDocument doc(capacity);
    String buff;
    t_state st = strip->getState();
    doc["br"] = st.br;
    doc["spd"] = st.spd;
    doc["fx"] = st.fx;
    doc["mode"] = st.mode;
    doc["size"] = st.size;

    serializeJson(doc, buff);
    server.send(200, "application/json", buff.c_str());
  }

  void _info() {
    server.send(200, "application/json", Settings::getInfoJson().c_str());
  }

  void _clearCredentials() {
    if (Settings::clear()) {
      _respond(200, "EEPROM cleared!");
      return;
    }

    _respond(500, "Error clearing EEEPROM'}");
  }
} // namespace WebServer
