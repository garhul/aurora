#include <Mosquitto.h>

namespace Mosquitto {
  WiFiClient wcli;
  MQTTClient client;
  void (*handler)(String topic, String payload);

  void handleMessage(String& topic, String& message) {
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

  bool init(void (*msgHandler)(String topic, String payload)) {
    byte attempts = 0;

    Serial.println("\n Connecting to broker " + String(Settings::broker));
    client.begin(Settings::broker, wcli);

    while (!client.connect(Settings::getDeviceId().c_str()) && attempts < CONN_RETRIES) {
      Serial.print(".");
      attempts++;
      delay(1000);
    }

    if (!client.connected()) {
      Serial.println("Unable to connect to broker");
      return false;
    }
    Mosquitto::loop();

    Serial.println("Subcribed to topic " + String(Settings::topic));

    handler = msgHandler;

    client.subscribe(Settings::topic);
    client.onMessage(handleMessage);

    delay(500);
    return true;
  }

  void announce() {
    // message size must remain small
    Serial.println("Announcing on " + String(Settings::announce_topic) + "   " + Settings::getAnnounceInfo());
    client.publish(Settings::announce_topic, Settings::getAnnounceInfo());
  }

  bool connected() {
    return client.connected();
  }
}
