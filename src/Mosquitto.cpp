#include <Mosquitto.h>

namespace Mosquitto {
  WiFiClient wcli;
  MQTTClient client;
  Strip* strip;

  void handleMessage(String& topic, String& message) {
    Serial.println("Received on topic [" + topic + "] payload  [" + message + "]");
    const size_t capacity = JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument doc(capacity);

    // const char* json = "{\"cmd\":\"setoff\",\"payload\":\"30f\"}";
    deserializeJson(doc, message);
    const char* cmd = doc["cmd"];
    const char* payload = doc["payload"];

    strip->cmd(cmd, payload);
  }

  void loop() {
    static unsigned long retry_t = 0;
    unsigned long t = millis();

    if (client.connected()) {
      client.loop();
      retry_t = 0;
    } else if (t > retry_t) {
      retry_t = t + MQTT_RECONNECT_COOLDOWN;
      init(strip);
    }
  }

  bool init(Strip* s) {
    byte attempts = 0;
    strip = s;

    client.setWill(Settings::announce_topic, Settings::getLastWillInfo().c_str());
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
    client.subscribe(Settings::topic);
    client.onMessage(handleMessage);
    delay(100);
    announce();
    return true;
  }

  void announce() {
    Serial.println("Announcing on " + String(Settings::announce_topic) + "   " + Settings::getAnnounceInfo());
    client.publish(Settings::announce_topic, Settings::getAnnounceInfo());
  }

  void updateState(t_state st) {
    const size_t capacity = JSON_OBJECT_SIZE(7) + 200;
    DynamicJsonDocument doc(capacity);
    String buff;
    doc["ev"] = "stateChange";
    doc["id"] = Settings::getDeviceId();
    doc["br"] = st.br;
    doc["spd"] = st.spd;
    doc["fx"] = st.fx;
    doc["mode"] = st.mode;
    doc["size"] = st.size;

    serializeJson(doc, buff);
    Serial.println("Sending state update " + buff);
    client.publish(Settings::announce_topic, buff);
  }

  bool connected() {
    return client.connected();
  }

} // namespace Mosquitto
