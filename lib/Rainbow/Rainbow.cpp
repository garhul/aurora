#include "Rainbow.h"

Rainbow::Rainbow() {
  Strip Panel();
  playing = false;
}

void Rainbow::begin() {
  #if USE_UDP
    UDP.begin(UDP_PORT);
  #endif
}

void Rainbow::run(byte data[], byte length) {
  #if DEBUG
    for (byte i = 0; i < length; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  #endif

  if (data[0] == CMD_OFF) {
    Panel.clear();
    playing = false;
  } else if (data[0] == CMD_TEST) {
    Panel.test();
    playing = false;
  } else if (data[0] == CMD_FILL_HSL) {
    Panel.fillHSL(data[1],data[2],data[3]);
    playing = false;
  } else if (data[0] == CMD_FILL) {
    Panel.fillRGB(data[1],data[2],data[3]);
    playing = false;
  } else if (data[0] == CMD_SET_BRIGHTNESS) {
    // Panel.setMaxBrightness(data[1]);
    analogWrite(5, data[1]);
  } else if (data[0] == CMD_SET_RANGE) {
    Panel.setRGBRange(data[3], data[4], data[5], data[1], data[2]);
    playing = false;
  } else if (data[0] == CMD_RUN_EFFECT) {
    Panel.resetFrameCount();
    playing = true;
    eff_playing = data[1];
  }

}

void Rainbow::service() {
  #if USE_UDP
    int dataLength = UDP.parsePacket();

    if (dataLength) {
      UDP.read(data, dataLength); // read the packet into the buffer
      run(data, dataLength);

      // send a response
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.write("cmd ok");
      UDP.endPacket();

    }
  #endif

  if (playing) {
    Panel.nextFrame(eff_playing);
    delay(30); //around 30 fps is ok
  }
}
