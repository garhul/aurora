#include <Arduino.h>
#include <FS.h>
#include <Mosquitto.h>
#include <Network.h>
#include <Settings.h>
#include <Strip/Strip.h>
#include <WebServer.h>
#include <defaults.h>
#include <types.h>

Strip *strip;

bool mqttAtInit = false;

void messageHandler(String cmd, String payload) {
  Serial.print("|CMD: " + cmd + " |");
  Serial.print("|Payload:" + payload + " |");
  Serial.println("");

  strip->cmd(cmd, payload);
}

void setup(void) {
  Settings::init();
  delay(3000);

  Serial.begin(115200);
  Serial.println("Init");

  if (!isnan(Settings::settings.strip_size) &&
      Settings::settings.strip_size < MAX_LENGTH) {
    strip = new Strip(Settings::settings.strip_size);
  } else {
    strip = new Strip(1);
  }

  digitalWrite(2, HIGH);  // turn off device led

  SPIFFS.begin();  // TODO replace with littleFS

  Network::init(Settings::settings.ssid, Settings::settings.pass);

  if (Network::getMode() == Network::MODES::ST) {
    mqttAtInit = Mosquitto::init(Settings::settings.broker,
                                 Settings::settings.topic, messageHandler);
  }
  WebServer::init(messageHandler);

  strip->test();
}

void loop(void) {
  yield();
  Network::checkAlive();

  if (!Mosquitto::connected() && mqttAtInit) {
    // try to reconnect only if we could connect during setup
    Mosquitto::init(Settings::settings.broker, Settings::settings.topic,
                    messageHandler);
  } else {
    Mosquitto::loop();
    WebServer::loop();
  }

  strip->loop();
}
