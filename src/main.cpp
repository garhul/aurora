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
  strip = new Strip(Settings::strip_size);
  digitalWrite(2, HIGH); // turn off device led

  Network::init();

  if (Network::getMode() == Network::MODES::ST && Settings::use_mqtt) {
    Mosquitto::init(strip);
  }

  strip->setStateHandler(onStripStateChange);
  WebServer::init(strip);
  strip->test();
}

void loop(void) {
  yield();
  Network::checkAlive();

  if ((Network::getMode() == Network::MODES::ST) && Settings::use_mqtt) {
    Mosquitto::loop();
  }

  WebServer::loop();
  strip->loop();
}
