#include <Arduino.h>
#include <defaults.h>
#include <types.h>
#include <FS.h>
#include <Strip/Strip.h>
#include <Network.h>
#include <Mosquitto.h>
#include <Utils.h>
#include <WebServer.h>

Strip* strip;

bool mqttAtInit = false;

void messageHandler(String cmd, String payload) {
  Serial.print("|CMD: " + cmd + " |");
  Serial.print("|Payload:" + payload + " |");
  Serial.println("");

  strip->cmd(cmd, payload);
}

void setup ( void ) {
  Utils::initStorage();
  
  delay(3000);
  Serial.begin(115200);  
  Serial.println("wtf strip");

  if (!isnan(Utils::settings.strip_size) && Utils::settings.strip_size < MAX_LENGTH ) {
    strip = new Strip(Utils::settings.strip_size);
  } else {
    strip = new Strip(1);
  }
    
  digitalWrite(2, HIGH); // turn off device led

  SPIFFS.begin(); // TODO replace with littleFS
  
  Network::init(Utils::settings.ssid, Utils::settings.pass);

  if (Network::getMode() == Network::MODES::ST)
    mqttAtInit = Mosquitto::init(Utils::settings.broker, Utils::settings.topic, messageHandler);
  
  WebServer::init(messageHandler);
 
  strip->test();
}

void loop ( void ) {
  yield();
  Network::checkAlive();
  
  if ( !Mosquitto::connected() && mqttAtInit)  {
    // try to reconnect only if we could connect during setup
    Mosquitto::init(Utils::settings.broker, Utils::settings.topic, messageHandler);
  } else {
    Mosquitto::loop();
    WebServer::loop();
  }

  strip->loop();
}
