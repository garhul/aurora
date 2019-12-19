#include <Strip/Strip.h>

Strip::Strip(uint16 length) {
  _max_bright = 25;
  frame_index = 0;
  spd = 10;
  mode = MODES::OFF;
  size = length;  
  bus = new NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod>(length);
  bus->Begin();
}

void Strip::fillRGB(uint8_t r, uint8_t g, uint8_t b){
  bus->ClearTo(RgbColor(r, g, b));
  bus->Show();
}

void Strip::fillHSL(uint8_t h, uint8_t s, uint8_t l){
  bus->ClearTo(RgbColor(HslColor((float) REL_UNIT_BYTE *  h, (float) REL_UNIT_BYTE * s, (float) REL_UNIT_BYTE * l)));
  bus->Show();
}

void Strip::loop() {
  #ifdef BENCHMARK
    static long t_frame;
    static long counter = 0;

    if (counter % 300 == 0) {
      Serial.print("Time for 300 frames: ")
      Serial.println(millis() - t_frame);
      t_frame = millis();
    }
    
    counter++;
  #endif
  //TODO:: benchmark frame rate 
  if (mode == MODES::PLAYING)
    nextFrame(this->fx);
    delay(25);
}

/**
 * CMD :
 *  - off | pause | play | test | br (n) | spd (n) | fx (n) | set (h,s,l)
 */
void Strip::cmd(String cmd, String payload) {
  Serial.println(cmd);
  Serial.println(payload);

  if (cmd == "off") {
    this->clear();
    this->mode = MODES::OFF;
  } else if (cmd == "pause") {
    this->mode = MODES::PAUSED;
  } else if (cmd == "play") {
    this->mode = MODES::PLAYING;
  } else if (cmd == "test") {
    this->test();
    this->mode =  MODES::OFF;
  } else if (cmd == "br") {
    this->_max_bright = atoi(payload.c_str());
  } else if (cmd == "spd") {
    byte spd = atoi(payload.c_str());
    if (spd == 0) spd = 1;
    this->spd = spd;
  } else if (cmd == "fx") {
    this->mode = MODES::PLAYING;
    this->fx = atoi(payload.c_str());
    char* spd ;
  } else if (cmd == "set") {
    this->mode = MODES::PAUSED;
    char* pl = (char*) payload.c_str();
  
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
  };
}

void Strip::setMaxBrightness(byte b){
  _max_bright = b;
}
//cycle each led to test connections
void Strip::test(){
  bus->ClearTo(RgbColor(0,0,0));
  bus->Show();
  for (int i = 0; i < this->size; i++ ) {
    bus->SetPixelColor(i, RgbColor(255,0,0));
    bus->Show();
    delay(TEST_DELAY);
    bus->SetPixelColor(i, RgbColor(0,255,0));
    bus->Show();
    delay(TEST_DELAY);
    bus->SetPixelColor(i, RgbColor(0,0,255));
    bus->Show();
    delay(TEST_DELAY);
    bus->SetPixelColor(i, RgbColor(0,0,0));
    bus->Show();
    delay(TEST_DELAY);
    digitalWrite(D7, LOW);
    delay(TEST_DELAY);
    digitalWrite(D7, HIGH);
    delay(TEST_DELAY);
  }
  //clear strip again
  bus->ClearTo(RgbColor(0,0,0));
  bus->Show();
};

void Strip::setRGBRange(byte r, byte g, byte b, int start, int end) {
  if (start < 0 || end > this->size)
    return;

  for (int i = start; i < end; i++ ) {
    bus->SetPixelColor(i , RgbColor(r,g,b));
  }

  bus->Show();
}

void Strip::setHSLRange(byte h, byte s, byte l, int start, int end) {
  if (start < 0 || end > this->size)
    return;

  HslColor color = HslColor((float) REL_UNIT_BYTE *  h, (float) REL_UNIT_BYTE * s, (float) REL_UNIT_BYTE * l);
  for (int i = start; i < end; i++ ) {
    bus->SetPixelColor(i , RgbColor(color));
  }

  bus->Show();
}

void Strip::clear() {
  bus->ClearTo(RgbColor(0,0,0));
  bus->Show();
}

void  Strip::randomize() {
  byte px = 0;
  for (px = 0; px < this->size ; px++) {
    pixels[px].hue = byte(random(255));
    pixels[px].br = byte(random(this->_max_bright));
    pixels[px].sat = 255;
  }
}

void Strip::fx_rainbow() { //rainbow 2
  static byte hue_inc = 0;

  if (frame_index % spd == 0) {
    hue_inc++;
  }

  byte n = 0;
  for (n = 0; n < this->size ; n++ ) {
    pixels[n].hue = (n * 2)  + hue_inc;
    pixels[n].br = _max_bright;
    pixels[n].sat = 255;
  }
}

void Strip::fx_wavebow() {
  static byte h_center = 0;
  static byte br_center = 0;
  static int dir = -1;
  byte n = 0;

  if (frame_index == 0 ) {
    //set the base for the effect
    for (n = 0; n < this->size; n++) {
      pixels[n].br = _max_bright;
      pixels[n].sat = 255;
      pixels[n].hue = h_center + n;
    }
  }

  for (n = 0; n < this->size; n++) {
      pixels[n].hue = h_center + n;
      pixels[n].sat = 255;
      pixels[n].br = (br_center + n) * ((float)_max_bright / this->size);
  }

  if (frame_index % spd == 0) {
    h_center++;
  }

  if (frame_index % spd == 0) {
    if (br_center == this->size || br_center == 0) {
      dir = dir * -1;
    }
    br_center += dir;
  }
}

void Strip::fx_opposites() {
  static byte hue = 0;

  if (frame_index % spd == 0) {
    hue++;
  }

  byte n = 0;
  byte br = 0;

    for (n = 0; n < this->size; n++ ) {
    if (n < this->size / 2) {
      pixels[n].hue = hue + 128;
      pixels[n].br = _max_bright;
      pixels[n].sat = 255;
    } else {
      pixels[n].hue = hue;
      pixels[n].br = _max_bright;
      pixels[n].sat = 255;
    }

  }
}

void Strip::fx_opp_seg() { //area efect (hue from 0 + increment on half of the strip)
  byte n = 0;
  static byte h = 0;

  for (n = 0; n < this->size ; n++) {
    if( n < (this->size / 2)) {
      pixels[n].hue = n * (255 / this->size) + h;
    } else {
      pixels[n].hue = (this->size - n -1) * (255 / this->size) +  h;
    }

    pixels[n].br = _max_bright;
    pixels[n].sat = 255;
  }

  if (frame_index % spd == 0) {
    h++;
  }

}

void Strip::fx_aurora() {
  static byte first_run = 1;
  static byte hue_inc = 0;
  static int dirs[MAX_LENGTH];

  if (first_run == 1) {
    for (int i = 0; i < this->size; i++) {
      pixels[i].br = i % 2 * _max_bright;
    }
    first_run = 0;
  }

  if (frame_index % spd == 0) {
    hue_inc++;
  }

  for (int n = 0; n < this->size; n++ ) {
    //if (frame_index % 1 == 0) {
    if (pixels[n].br == _max_bright) { //we should start reducing
      dirs[n] = -1;
    } else if (pixels[n].br <= 0) {
      dirs[n] = 1;
    }
    pixels[n].br += dirs[n];
  //}

    pixels[n].hue = (n * 2)  + hue_inc;
    pixels[n].sat = 255;
  }
 }

void Strip::resetFrameCount() {
  frame_index = 0;
}

void Strip::nextFrame(char eff_index) {  
  switch (eff_index) {
    case FX::AURORA :
      this->fx_aurora();
      break;
    
    case FX::RAINBOW :
      this->fx_rainbow();
      break;
    
    case FX::WAVEBOW :
      this->fx_wavebow();
      break;
    
    case FX::OPPOSITES :
      this->fx_opposites();
      break;
    
    case FX::OPPOSITES_SEGMENTED :
      this->fx_opp_seg();
      break;
  }
  byte n = 0;
  for (n = 0; n < this->size; n++ ) {
    bus->SetPixelColor(n, RgbColor(
      HsbColor( pixels[n].hue * REL_UNIT_BYTE,
                pixels[n].sat * REL_UNIT_BYTE,
                pixels[n].br * REL_UNIT_BYTE)));
  }

  bus->Show();
  frame_index++;
}
