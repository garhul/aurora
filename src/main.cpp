#include <Arduino.h>
#include <FS.h>
#include <Mosquitto.h>
#include <Network.h>
#include <Settings.h>
#include <Strip.h>
#include <WebServer.h>

Strip* strip;

void onStripStateChange(t_state st) {
  if (Settings::use_mqtt) {
    Mosquitto::updateState(st);
  }
}

void setup(void) {
  Serial.begin(115200);
  Settings::init();
  digitalWrite(2, HIGH); // turn off device led

  Network::init();
  if (Network::getMode() == Network::MODES::ST) {
    if (Settings::use_mqtt && Mosquitto::init(strip)) {
      Mosquitto::announce();
    };
  }
  strip = new Strip(Settings::strip_size);
  strip->setStateHandler(onStripStateChange);

  WebServer::init(strip);
  strip->test();
}

void loop(void) {
  static unsigned long lastCheck = 0;
  yield();

  if (millis() - lastCheck > WIFI_CHECK_PERIOD) {
    Network::checkAlive();
    lastCheck = millis();
  }

  if (Settings::use_mqtt) {
    if (Mosquitto::connected()) {
      Mosquitto::loop();
    }
    else {
      Mosquitto::init(strip);
    }
  }

  WebServer::loop();
  strip->loop();
}
