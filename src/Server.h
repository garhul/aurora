#ifndef __SERVER_H__
#define __SERVER_H__

#include <types.h>
#include <Utils.h>
#include <ESP8266WebServer.h>

namespace Server {
  ESP8266WebServer server;
  void (*cmdHandler)(String cmd, String payload);
  
  void init(void (*f)(String,String));
  void loop();

  void _cmd();
  void _info();
  void _setup();
  void _clearCredentials();
}

#endif
