#ifndef __MOSQUITTO_H__
#define __MOSQUITTO_H__

#include <MQTT.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Settings.h>
#define CONN_RETRIES 3

namespace Mosquitto {

  //TODO:: add function prototype for message handling
  bool init(void (*handler)(String topic , String payload));
  void handleMessage(String& topic , String& payload);
  bool connected();
  void announce();
  void loop();
} // namespace Mosquitto

#endif
