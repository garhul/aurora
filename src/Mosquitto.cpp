#include <Mosquitto.h>

namespace Mosquitto {
  MQTTClient client;
  WiFiClient wcli;
  void (*handler)(String topic, String payload);

  void handleMessage(String &topic, String &payload) {
    Serial.println("Received on topic [" + topic + "] payload  [" + payload + "]");
    handler(topic, payload);
  }

  void loop() {
    client.loop();
  }

  void init(const char* broker, const char* topic, void (*handler)(String topic, String payload)) {
    String clientID  = String("aurora_") + String(ESP.getChipId(), HEX);
    byte attempts = 0;

    client.begin(broker, wcli);
    Serial.print("\n Connecting to broker");
    Serial.println(broker);

    //TODO:: add timeout
    while (!client.connect((clientID.c_str(), "", "")) && attempts < CONN_RETRIES) {
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

    client.subscribe(topic);
    client.onMessage(handleMessage);
  }
}
