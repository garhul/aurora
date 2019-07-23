#include <Arduino.h>
#include <defaults.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <EEPROM.h>
#include <NeoPixelBus.h>
#include <Strip.h>
#include <WiFiUdp.h>
#include <Hash.h>
#include <MQTT.h>

Strip strip;
ESP8266WebServer server(80);
MQTTClient client;
WiFiClient net;

// struct {
//   char ssid[32] = "";
//   char pass[32] = "";
//   char topic[32] = "";
//   char broker[32] = "";
// } t_settings;

// typedef struct t_settings settings;

struct Settings {
  char ssid[32];
  char pass[32];
  char topic[32];
  char broker[32];
};

Settings settings = {0};

boolean clearEEPROM() {   
  for (int i = 0; i < EEPROM_SIZE; ++i) {
    EEPROM.write(i, 1);
  }

  return EEPROM.commit();
}

void setupAP() {
  //check post params exist
  if(!server.hasArg("pass") || !server.hasArg("ssid")  || !server.hasArg("topic") || !server.hasArg("broker")) {
    server.send(400, "application/json", "{message:'Invalid params'}");
    return;
  }
  
  if (!clearEEPROM()) {
    server.send(500, "application/json", "{message:'error clearing credentials'}");
    return;
  };

  server.arg("ssid").toCharArray(settings.ssid, 32);
  server.arg("pass").toCharArray(settings.pass, 32);
  server.arg("topic").toCharArray(settings.topic, 32);
  server.arg("broker").toCharArray(settings.broker, 32);

  EEPROM.put(0x00, settings);

  // settings->ssid = server.arg("ssid");
  // settings.pass = server.arg("pass");
  // settings.ssid = server.arg("topic");
  // settings.ssid = server.arg("broker");


  // // ssid is stored in addr between 0 and 32 
  // for (int i = 0; i < server.arg("ssid").length(); ++i) {
  //   EEPROM.write(i, server.arg("ssid")[i]);
  // }
  
  // // pwd is stored in addr between 33 and 64 
  // for (int i = 0; i < server.arg("pwd").length(); ++i) {
  //   EEPROM.write(32+i, server.arg("pwd")[i]);
  // }
  
  // // topic starts at position 64
  // for (int i = 0; i < server.arg("topic").length(); ++i) {
  //   EEPROM.write(64+i, server.arg("topic")[i]);
  // }

  // // broker starts at position 100
  // for (int i = 0; i < server.arg("broker").length(); ++i) {
  //   EEPROM.write(100+i, server.arg("broker")[i]);
  // }

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

void clearCredentials() {
  for (int i = 0; i < 250; ++i) {
    EEPROM.write(i, 1);
  }

  if (EEPROM.commit()) {
    server.send(200, "application/json", "{message:'Device eeprom cleared'}");
  } else {
    server.send(500, "application/json", "{message:'Error clearing eeprom'}");
  }
}

bool beginST() {  
  WiFi.mode(WIFI_STA);
  int attempts = 0;

  EEPROM.get(0x00, settings);

  String pwd = settings.pass;
  String ssid = settings.ssid;

  // // try to load eeprom data for SSID and pwd
  // for (int i = 0; i < 32; ++i) {
  //   ssid += char(EEPROM.read(i));
  // }

  // for (int i = 32; i < 64; ++i) {
  //   pwd += char(EEPROM.read(i));
  // }

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
  Serial.begin(115200);
  digitalWrite(2, HIGH); // turn of device led
  EEPROM.begin(EEPROM_SIZE);
  SPIFFS.begin();
  WiFi.persistent(false);
  delay(2000);

  if (!beginST()) {    
    beginAP();
  } else {
    initMQTT();
  } 

  // Set up endpoints for network configuration
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
