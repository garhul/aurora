#include <Arduino.h>
#include <defaults.h>
#include <types.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <NeoPixelBus.h>
#include <Strip.h>
#include <WiFiUdp.h>
#include <Hash.h>
#include <MQTT.h>
#include <Utils.h>

Strip strip;
ESP8266WebServer server(80);
MQTTClient client;
WiFiClient net;

settings_t settings = {0};


void setupAP() {
  //check post params exist
  if(
      !server.hasArg("pass") ||
      !server.hasArg("ssid") ||
      !server.hasArg("topic")||
      !server.hasArg("broker")
    ) {
    server.send(400, "application/json", "{message:'Invalid params'}");
    return;
  }
  
  if (!(Utils::clearStorage())) {
    server.send(500, "application/json", "{message:'error clearing credentials'}");
    return;
  };

  server.arg("ssid").toCharArray(settings.ssid, 32);
  server.arg("pass").toCharArray(settings.pass, 32);
  server.arg("topic").toCharArray(settings.topic, 32);
  server.arg("broker").toCharArray(settings.broker, 32);

  EEPROM.put(0x00, settings);

  if (EEPROM.commit()) {      
    server.send(200, "application/json", "{message:'settings stored'}");
    delay(2000);
    ESP.restart();
  } else {
    server.send(500, "application/json", "{message:'error storing credentials'}");
  }
  
}

void beginAP() {
  WiFi.mode(WIFI_AP);

  #if AP_USE_PWD
    WiFi.softAP(AP_SSID, AP_PWD);
  #else
    WiFi.softAP(AP_SSID);
  #endif

  IPAddress myIP = WiFi.softAPIP();
  WiFi.printDiag(Serial);
  Serial.println(WiFi.softAPIP());
}


bool beginST() {  
  int attempts = 0;
  String pwd = "";
  String ssid = "";

  WiFi.mode(WIFI_STA);
 
  // disconnect from any previously connected network (open networks?) 
  if (WiFi.status() == WL_CONNECTED) {    
    WiFi.disconnect(false);
    delay(5000);
  }

  while (WiFi.status() != WL_CONNECTED && attempts < CONN_RETRIES) {
    WiFi.begin ( ssid.c_str(), pwd.c_str() );
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

void clearCredentials() {
  if (Utils::clearStorage()) {
    server.send(200, "application/json", "{message:'Device eeprom cleared'}");
  } else {
    server.send(500, "application/json", "{message:'Error clearing eeprom'}");
  }
}

void mqttMessage(String &topic, String &payload) {
  Serial.println("rcv: " + topic + " - " + payload);
  strip.cmd(payload);
}

void initMQTT() {
  client.begin(settings.broker, net);

  Serial.print("\nConnecting to broker");
  Serial.println(settings.broker);

  while (!client.connect("aurora", "t", "t")) {
    Serial.print(".");
    delay(50);
  }

  Serial.print("Subscripto al topico: ");
  Serial.println(settings.topic);

  client.subscribe(settings.topic);
  client.onMessage(mqttMessage);
}

void setup ( void ) {
  settings = Utils::getSettings();

  String pwd = settings.pass;
  String ssid = settings.ssid;

  Serial.begin(115200);
  digitalWrite(2, HIGH); // turn of device led
  
  SPIFFS.begin();
  WiFi.persistent(false);
  delay(2000);

  if (!beginST()) {    
    beginAP();
  } else {
    initMQTT();
  } 

  // Set up endpoints
  server.on("/setup", HTTP_POST, setupAP);
  server.on("/clear", HTTP_POST, clearCredentials); //endpoint for clearing ssid / pwd
  server.begin();
}

void loop ( void ) {
  yield();
  client.loop();
  server.handleClient();
  strip.loop();
}
