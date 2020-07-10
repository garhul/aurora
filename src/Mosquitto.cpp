#include <Mosquitto.h>
#include <ArduinoJson.h>

namespace Mosquitto {
  WiFiClient wcli;
  MQTTClient client;
  void (*handler)(String topic, String payload);

  void handleMessage(String &topic, String &message) {
    Serial.println("Received on topic [" + topic + "] payload  [" + message + "]");
      const size_t capacity = JSON_OBJECT_SIZE(2) + 30;      
      DynamicJsonDocument doc(capacity);
      
      // const char* json = "{\"cmd\":\"setoff\",\"payload\":\"30f\"}";
      deserializeJson(doc, message);
      const char* cmd = doc["cmd"];
      const char* payload = doc["payload"]; 
      
    handler(cmd, payload);
  }

  void loop() {
    client.loop();
  }

  void init(const char* broker, const char* topic, void (*msgHandler)(String topic, String payload)) {
    String clientID  = Utils::getDeviceId();
    byte attempts = 0;

    client.begin(broker, wcli);
    Serial.print("\n Connecting to broker");
    Serial.println(broker);

    //TODO:: add timeout
    while (!client.connect(clientID.c_str()) && attempts < CONN_RETRIES) {
      Serial.print(".");
      attempts++;
      delay(1000);
    }

    if (!client.connected()) {
      Serial.println("Unable to connect to broker");
      return;
    }

    Serial.print("Subcribed to topic");
    Serial.println(topic);

    handler = msgHandler;
    client.subscribe(topic);
    client.onMessage(handleMessage);

    announce();
  }

  void announce() {
    settings_t st  = Utils::getSettings();    
    client.publish(String(st.announce_topic), Utils::getInfoJson());
  }

  bool connected() {
    return client.connected();
  }
}
