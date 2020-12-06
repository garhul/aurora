#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <Settings.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Strip.h>

namespace WebServer {
  void init(Strip* s);
  void loop();

  void _cmd();
  void _control();
  void _info();
  void _setup();
  void _clearCredentials();
  void _getState();
  void _serveFile(const char* filepath, const char* doctype);
}

#endif
