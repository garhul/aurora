#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <types.h>
#include <Utils.h>
#include <ESP8266WebServer.h>
#include <FS.h>

namespace WebServer {  
  void init(void (*f)(String,String));
  void loop();

  void _cmd();
  void _control();
  void _info();
  void _setup();
  void _clearCredentials();
}

#endif
