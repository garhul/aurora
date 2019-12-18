#ifndef __MOSQUITTO_H__
#define __MOSQUITTO_H__

#include <MQTT.h>
#include <WiFiClient.h>

#define CONN_RETRIES 3

namespace Mosquitto {
  WiFiClient wcli;
  void (*handler)(String topic, String payload);

  //TODO:: add function prototype for message handling
  void init(const char* broker, const char* topic, void (*handler)(String topic, String payload));
  void handleMessage(String &topic, String &payload);
  void loop();
}

#endif
