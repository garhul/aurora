void serveFile(const char *filepath, const char *doctype = "text/html") {
  if (! SPIFFS.exists(filepath)) {
    server.send(404,"text/plain", "File not found");
    // USE_SERIAL.println("file not found");
    // USE_SERIAL.println(filepath);
    return ;
  }

  File f = SPIFFS.open(filepath, "r");
  if (!f) {
    server.send(500, "text/plain", "Error opening file");
    return;
  }

  //read the file in chunks (not that much ram)
  server.streamFile(f, doctype);
  f.close();
}

/** Station & Access point service endpoints **/
void homeAP() {
  serveFile("/main.html");
}

void indexST() {
  serveFile("/index.html");
}

void descriptor() {
  serveFile("/descriptor.json");
}

void controls() {
  serveFile("/controls.html");
}

void reboot() {
  ESP.restart();
}

/**
setup all endpoints for any mode we're running in
*/
void setupEndpoints() {
  //setup basic endpoints

  server.on("/clear", clearCredentials); //endpoint for clearing ssid / pwd
  server.on("/descriptor" , descriptor); //endpoint for serving the descriptor
  server.on("/announce" , announce); //request device to announce itself
  server.on("/reboot", reboot); //reboot the device
  server.on("/update", setup); //update descriptor info
  server.on("/cmd", HTTP_POST, cmd);
  server.on("/ap", homeAP);
  server.on("/", indexST);
  server.on("/controls", controls);

}

void cmd() {
  if (server.hasArg("cmd")) {
    String c = server.arg("cmd");
    byte b[c.length()];
    byte n[3] = {0x07,0x02,0x00};
    c.getBytes(b, c.length() + 1);
    rb.run(n, 3);
    server.send(200, "application/json", "{message:'ok'}");
  } else {
    server.send(404,"application/json", "{message:'no cmd provided'}");
  }
}