#include <Mosquitto.h>

namespace Mosquitto {
  WiFiClient wcli;
  MQTTClient client;
  void (*handler)(String topic, String payload);

  void handleMessage(String &topic, String &payload) {
    Serial.println("Received on topic [" + topic + "] payload  [" + payload + "]");

    //todo :: extract last token from mosquitto topic    
    char *last = strrchr(topic.c_str(), '/');
    
    if (last != NULL) {
      printf("Last token: '%s'\n", last+1);
    }

    handler(String(last + 1 ), payload);
  }

  void loop() {
    client.loop();
  }

  void init(const char* broker, const char* topic, void (*msgHandler)(String topic, String payload)) {
    String clientID  = Utils::getDeviceName();
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
  }
}
