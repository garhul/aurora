#include <Arduino.h>
#include <defaults.h>
#include <types.h>
#include <FS.h>
#include <Strip.h>
#include <Network.h>
#include <Mosquitto.h>
#include <Utils.h>
#include <Server.h>


Strip strip;

void messageHandler(String topic, String payload) {
  Serial.print("|Topic: " + topic + " |");
  Serial.print("|Payload:" + payload + " |");
  Serial.println("");

  // strip.cmd(/*implement*/ );
}

void setup ( void ) {
  settings_t settings = Utils::getSettings();
  Serial.begin(115200);
  digitalWrite(2, HIGH); // turn of device led

  #ifdef USE_FS
    SPIFFS.begin();
  #endif

  Network::init(settings.ssid, settings.pass);
  
  if (Network::mode == Network::MODES::ST)
    Mosquitto::init(settings.broker, settings.topic, messageHandler);
  
  Server::init(messageHandler);
}

void loop ( void ) {
  yield();
  Mosquitto::loop();
  Server::loop();
  strip.loop();
}
