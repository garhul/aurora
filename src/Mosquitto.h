#ifndef __MOSQUITTO_H__
#define __MOSQUITTO_H__

#include <MQTT.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Settings.h>
#include <Strip.h>
#define CONN_RETRIES 3

namespace Mosquitto {
  bool init(Strip* s);
  void handleMessage(String& topic, String& payload);
  bool connected();
  void announce();
  void updateState(t_state st);
  void loop();
} // namespace Mosquitto

#endif
