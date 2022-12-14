#include <Strip.h>

Strip::Strip(uint16 length) {
  _max_bright = 25;
  frame_index = 0;
  spd = 250;
  colorspace = COLORSPACE::HSB;
  spd_delay = 1 + (255 - spd);
  mode = MODES::OFF;
  size = (length < MAX_LENGTH) ? length : MAX_LENGTH;
  bus = new NeoPixelBusType(length);
  bus->Begin();
}

void Strip::fillRGB(uint8_t r, uint8_t g, uint8_t b) {
  bus->ClearTo(RgbColor(r, g, b));
  bus->Show();
}

void Strip::fillHSL(uint8_t h, uint8_t s, uint8_t l) {
  bus->ClearTo(RgbColor(HslColor((float)REL_UNIT_BYTE * h, (float)REL_UNIT_BYTE * s, (float)REL_UNIT_BYTE * l)));
  bus->Show();
}

void Strip::loop() {
  static unsigned long next_frame_t = 0;
  unsigned long t = millis();
  if (mode == MODES::PLAYING && (t > next_frame_t)) {
    nextFrame(this->fx);
    next_frame_t = t + (1000 / TARGET_FRAMERATE);
  }
}

void Strip::setStateHandler(void (*fn)(t_state st)) {
  this->stateHandler = fn;
}

t_state Strip::getState() {
  return {
      this->spd,
      this->_max_bright,
      this->mode,
      this->size,
      this->fx};
}

/**
 * CMD :
 *  - off | pause | play | test | br (n) | spd (n) | fx (n) | set (h,s,l) | setRgb (r,g,b)
 */
void Strip::cmd(String cmd, String payload) {
  Serial.print(cmd);
  Serial.print(" ");
  Serial.print(payload);
  Serial.println();
  if (cmd == CMD_OFF) {
    this->clear();
    this->mode = MODES::OFF;
  } else if (cmd == CMD_PAUSE) {
    this->mode = MODES::PAUSED;
  } else if (cmd == CMD_PLAY) {
    this->mode = MODES::PLAYING;
  } else if (cmd == CMD_TEST) {
    this->test();
    this->mode = MODES::OFF;
  } else if (cmd == CMD_BR) {
    this->resetFrameCount();
    this->_max_bright = atoi(payload.c_str());
  } else if (cmd == CMD_SPEED) {
    this->resetFrameCount();
    byte spd = atoi(payload.c_str());
    this->spd = spd;
    this->spd_delay = 1 + (255 - spd);
  } else if (cmd == CMD_FX) {
    this->resetFrameCount();
    this->mode = MODES::PLAYING;
    this->fx = atoi(payload.c_str());
  } else if (cmd == CMD_SETHSL) {
    this->mode = MODES::PAUSED;
    char* pl = (char*)payload.c_str();

    byte h = atoi(strtok_r(NULL, " ", &pl));
    byte s = atoi(strtok_r(NULL, " ", &pl));
    byte l = atoi(strtok_r(NULL, " ", &pl));

    Serial.print(h);
    Serial.print(" ");
    Serial.print(s);
    Serial.print(" ");
    Serial.println(l);
    Serial.println(this->size);
    this->setHSLRange(h, s, l, 0, this->size);
  } else if (cmd == CMD_SETRGB) {
    this->mode = MODES::PAUSED;
    char* pl = (char*)payload.c_str();

    uint8_t r = atoi(strtok_r(NULL, " ", &pl));
    uint8_t g = atoi(strtok_r(NULL, " ", &pl));
    uint8_t b = atoi(strtok_r(NULL, " ", &pl));

    Serial.printf("setRgb: %u %u %u\n", r, g, b);

    this->fillRGB(r, g, b);
  }

  this->stateHandler(this->getState());
}

void Strip::setMaxBrightness(byte b) {
  _max_bright = b;
}

// cycle each led to test connections
void Strip::test() {
  bus->ClearTo(RgbColor(0, 0, 0));
  bus->Show();
  for (int i = 0; i < this->size; i++) {
    yield();
    bus->SetPixelColor(i, RgbColor(255, 0, 0));
    bus->Show();
    delay(TEST_DELAY);
    bus->SetPixelColor(i, RgbColor(0, 255, 0));
    bus->Show();
    delay(TEST_DELAY);
    bus->SetPixelColor(i, RgbColor(0, 0, 255));
    bus->Show();
    delay(TEST_DELAY);
    bus->SetPixelColor(i, RgbColor(0, 0, 0));
    bus->Show();
    delay(TEST_DELAY);
    digitalWrite(D7, LOW);
    delay(TEST_DELAY);
    digitalWrite(D7, HIGH);
    delay(TEST_DELAY);
  }
  // clear strip again
  bus->ClearTo(RgbColor(0, 0, 0));
  bus->Show();
};

void Strip::setRGBRange(byte r, byte g, byte b, int start, int end) {
  if (start < 0 || end > this->size)
    return;
  for (int i = start; i < end; i++) {
    bus->SetPixelColor(i, RgbColor(r, g, b));
  }
  bus->Show();
}

void Strip::setHSLRange(byte h, byte s, byte l, int start, int end) {
  if (start < 0 || end > this->size)
    return;

  HslColor color = HslColor((float)REL_UNIT_BYTE * h, (float)REL_UNIT_BYTE * s, (float)REL_UNIT_BYTE * l);
  for (int i = start; i < end; i++) {
    bus->SetPixelColor(i, RgbColor(color));
  }

  bus->Show();
}

void Strip::clear() {
  bus->ClearTo(RgbColor(0, 0, 0));
  bus->Show();
}

void Strip::randomize() {
  byte px = 0;
  for (px = 0; px < this->size; px++) {
    pixels[px].a = byte(random(255));
    pixels[px].c = byte(random(this->_max_bright));
    pixels[px].b = 255;
  }
}

void Strip::fx_chaser() {
  static byte hue = 90;
  static byte chaser_len = 7; // 7 pixels length
  static byte space = 7;
  static byte idx = 0;
  static byte adv = 0;
  byte n = 0;
  byte br = 0;
  leds tmp;

  if (frame_index == 0) {
    for (n = 0; n < this->size; n++) {
      idx = ++idx % (chaser_len + space);
      // if (idx == 0) hue+=;

      if (idx < chaser_len) {
        br = (_max_bright / chaser_len) * idx;
        pixels[n].b = 255;
      } else {
        br = 0;
      }

      pixels[n].c = br;
      pixels[n].a = hue;
    }
  }

  if (frame_index % this->spd_delay == 0) {
    adv = ++adv % this->size;
    if (adv == 0)
      hue += 8;

    tmp = pixels[0];
    for (n = 0; n < this->size - 1; n++) {
      pixels[n] = pixels[n + 1];
      pixels[n].a = hue;
    }
    pixels[this->size - 1] = tmp;
    pixels[this->size - 1].a = hue;
  }
}

void Strip::fx_white_chaser() {
  static byte chaser_len = 7; // 7 pixels length
  static byte space = 7;
  static byte idx = 0;
  byte n = 0;
  byte br = 0;
  leds tmp;

  if (frame_index == 0) {
    for (n = 0; n < this->size; n++) {
      idx = ++idx % (chaser_len + space);

      if (idx < chaser_len) {
        br = (_max_bright / chaser_len) * idx;
        pixels[n].b = 0;
      } else {
        br = 0;
      }
      pixels[n].c = br;
    }
  }

  if (frame_index % this->spd_delay == 0) {
    tmp = pixels[0];
    for (n = 0; n < this->size - 1; n++) {
      pixels[n] = pixels[n + 1];
    }
    pixels[this->size - 1] = tmp;
  }
}

void Strip::fx_trip() {
  static byte chaser_len = 10; // 7 pixels length
  static byte space = 20;
  byte idx = 0;
  byte n = 0;
  leds tmp;

  if (frame_index == 0) {
    this->randomize();

    for (n = 0; n < this->size; n++) {
      idx = ++idx % (chaser_len + space);
      if (idx < chaser_len) {
        pixels[n].c = (_max_bright / chaser_len) * idx;
      } else {
        pixels[n].c = _max_bright / chaser_len;
      }
    }
  }

  if (frame_index % this->spd_delay == 0) {
    tmp = pixels[0];
    for (n = 0; n < this->size - 1; n++) {
      pixels[n].c = pixels[n + 1].c;
    }
    pixels[this->size - 1].c = tmp.c;
  }
}
// simple hue transition
void Strip::fx_rainbow() {
  static byte hue_inc = 0;

  if (frame_index % this->spd_delay == 0) {
    hue_inc++;
  }

  byte n = 0;
  for (n = 0; n < this->size; n++) {
    pixels[n].a = (n * 2) + hue_inc;
    pixels[n].c = _max_bright;
    pixels[n].b = 255;
  }
}

// TODO:: check the effect
void Strip::fx_wavebow() {
  static byte h_center = 0;
  static byte br_center = 0;
  static int dir = -1;
  byte n = 0;

  if (frame_index == 0) {
    // set the base for the effect
    for (n = 0; n < this->size; n++) {
      pixels[n].c = _max_bright;
      pixels[n].b = 255;
      pixels[n].a = h_center + n;
    }
  }

  for (n = 0; n < this->size; n++) {
    pixels[n].a = h_center + n;
    pixels[n].b = 255;
    pixels[n].c = (br_center + n) * ((float)_max_bright / this->size);
  }

  if (frame_index % this->spd_delay == 0) {
    h_center++;
  }

  if (frame_index % this->spd_delay == 0) {
    if (br_center == this->size || br_center == 0) {
      dir = dir * -1;
    }
    br_center += dir;
  }
}

/** Split strip in two with a 180 degree in between hues */
void Strip::fx_opposites() {
  static byte hue = 0;

  if (frame_index % this->spd_delay == 0) {
    hue++;
  }

  byte n = 0;

  for (n = 0; n < this->size; n++) {
    if (n < this->size / 2) {
      pixels[n].a = hue + 128;
      pixels[n].c = _max_bright;
      pixels[n].b = 255;
    } else {
      pixels[n].a = hue;
      pixels[n].c = _max_bright;
      pixels[n].b = 255;
    }
  }
}

// area efect (hue from 0 + increment on half of the strip)
void Strip::fx_hue_split() {
  byte n = 0;
  static byte h = 0;

  for (n = 0; n < this->size; n++) {
    if (n < (this->size / 2)) {
      pixels[n].a = n * (255 / this->size) + h;
    } else {
      pixels[n].a = (this->size - n - 1) * (255 / this->size) + h;
    }

    pixels[n].c = _max_bright;
    pixels[n].b = 255;
  }

  if (frame_index % this->spd_delay == 0) {
    h++;
  }
}

void Strip::fx_aurora() {
  static byte hue_inc = 0;
  static int dirs[MAX_LENGTH];

  if (frame_index == 0) {
    for (int i = 0; i < this->size; i++) {
      pixels[i].c = i % 2 * _max_bright;
    }
  }

  if (frame_index % this->spd_delay == 0) {
    hue_inc++;
  }

  for (int n = 0; n < this->size; n++) {
    if (frame_index % this->spd_delay == 0) {
      if (pixels[n].c == _max_bright) { // we should start reducing
        dirs[n] = -1;
      } else if (pixels[n].c <= 0) {
        dirs[n] = 1;
      }
      pixels[n].c += dirs[n];
    }

    pixels[n].a = (n * 2) + hue_inc;
    pixels[n].b = 255;
  }
}

void Strip::fx_white_aurora() {
  static int dirs[MAX_LENGTH];

  if (frame_index == 0) {
    for (int i = 0; i < this->size; i++) {
      pixels[i].c = i % 2 * _max_bright;
    }
  }

  for (int n = 0; n < this->size; n++) {
    if (frame_index % this->spd_delay == 0) {
      if (pixels[n].c == _max_bright) { // we should start reducing
        dirs[n] = -1;
      } else if (pixels[n].c <= 2) {
        dirs[n] = 1;
      }
      pixels[n].c += dirs[n];
    }

    pixels[n].a = 0;
    pixels[n].b = 0;
  }
}

// oscilate brightness up and down within 50% of maximum range, (if max bright is 50 go from 40 to 50 and back)
void Strip::fx_albiCeleste() {
  static byte dir = 1;
  static uint8 br = 0;
  uint8 c = br / 3;
  uint8 upper_limit = _max_bright;
  uint8 lower_limit = ceil(_max_bright * .5);
  uint8 inc = ceil((upper_limit) / 24);

  if (frame_index == 0) {
    br = lower_limit;
  }

  if (frame_index % this->spd_delay == 0) {
    if (br > upper_limit) {
      br = upper_limit;
    } else if (br < lower_limit) {
      br = lower_limit;
    }

    if (br == upper_limit) {
      dir = -1;
    } else if (br == lower_limit) {
      dir = 1;
    }

    br += (inc * dir);
    c = br / 3;

    // Serial.print(frame_index);
    // Serial.print("    inc: " + String(inc));
    // Serial.print("    br: " + String(br));
    // Serial.print("    ll: " + String(lower_limit));
    // Serial.println("    ul: " + String(upper_limit));
  }

  for (unsigned int n = 0; n < this->size; n++) {
    if (n < this->size / 3) {
      pixels[n].a = c;
      pixels[n].b = c;
      pixels[n].c = br;
    } else if (n < ((this->size / 3) * 2)) {
      pixels[n].a = br;
      pixels[n].b = br;
      pixels[n].c = br;
    } else {
      pixels[n].a = c;
      pixels[n].b = c;
      pixels[n].c = br;
    }
  }
}

void Strip::resetFrameCount() {
  frame_index = 0;
}

inline bool shallrun_ms(unsigned long delay_time, unsigned long& last_check) {
  unsigned long t = millis();
  bool retVal = (t > const_cast<unsigned long&>(last_check));
  if (retVal)
    last_check = delay_time + t;

  return retVal;
}

void Strip::nextFrame(char eff_index) {
  switch (eff_index) {
    case FX::AURORA:
      this->colorspace = COLORSPACE::HSB;
      this->fx_aurora();
      break;

    case FX::WHITE_AURORA:
      this->colorspace = COLORSPACE::HSB;
      this->fx_white_aurora();
      break;

    case FX::RAINBOW:
      this->colorspace = COLORSPACE::HSB;
      this->fx_rainbow();
      break;

    case FX::WAVEBOW:
      this->colorspace = COLORSPACE::HSB;
      this->fx_wavebow();
      break;

    case FX::OPPOSITES:
      this->colorspace = COLORSPACE::HSB;
      this->fx_opposites();
      break;

    case FX::HUE_SPLIT:
      this->colorspace = COLORSPACE::HSB;
      this->fx_hue_split();
      break;

    case FX::CHASER:
      this->colorspace = COLORSPACE::HSB;
      this->fx_chaser();
      break;

    case FX::WHITE_CHASER:
      this->colorspace = COLORSPACE::HSB;
      this->fx_white_chaser();
      break;

    case FX::TRIP:
      this->colorspace = COLORSPACE::HSB;
      this->fx_trip();
      break;

    case FX::ALBI:
      this->colorspace = COLORSPACE::RGB;
      this->fx_albiCeleste();
      break;
  }

  if (this->colorspace == COLORSPACE::HSB) {
    for (unsigned int n = 0; n < this->size; n++) {
      bus->SetPixelColor(n, HsbColor(pixels[n].a * REL_UNIT_BYTE,
                                     pixels[n].b * REL_UNIT_BYTE,
                                     pixels[n].c * REL_UNIT_BYTE));
    }
  } else {
    for (unsigned int n = 0; n < this->size; n++) {
      bus->SetPixelColor(n, RgbColor(
                                pixels[n].a,
                                pixels[n].b,
                                pixels[n].c));
    }
  }

  bus->Show();
  frame_index++;
}
