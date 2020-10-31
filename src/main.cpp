#include <Arduino.h>
#include <FS.h>
#include <Mosquitto.h>
#include <Network.h>
#include <Settings.h>
#include <Strip/Strip.h>
#include <WebServer.h>

Strip* strip;

void messageHandler(String cmd , String payload) {
  Serial.print("|CMD: " + cmd + " |");
  Serial.print("|Payload:" + payload + " |");
  Serial.println("");

  strip->cmd(cmd , payload);
}

void setup(void) {
  Serial.begin(115200);
  Settings::init();
  digitalWrite(2 , HIGH); // turn off device led

  Network::init();

  if (Network::getMode() == Network::MODES::ST) {
    if (Settings::use_mqtt && Mosquitto::init(messageHandler)) {
      Mosquitto::announce();
    };
  }

  WebServer::init(messageHandler);
  strip->test();
}

void loop(void) {
  yield();
  Network::checkAlive();

  if (Settings::use_mqtt && Mosquitto::connected()) {
    Mosquitto::loop();
  }
  else {
    Mosquitto::init(messageHandler);
  }

  WebServer::loop();
  strip->loop();
}
