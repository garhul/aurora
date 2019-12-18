#include <NeoPixelBus.h>

#define STRIP_SIZE 30
#define TEST_DELAY 15

#define REL_UNIT_BYTE 0.0039f
#define REL_UNIT_SIZE 1.0f /  STRIP_SIZE

struct leds {
  byte hue;
  byte sat;
  byte br;
};

enum FX {
  NONE,
  RAINBOW,
  WAVEBOW,
  AURORA,
  OPPOSITES,
  OPPOSITES_SEGMENTED,
  COUNT,
};

typedef NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> bus_t;

class Strip {
  public:
    Strip(int size);
    void test();
    void setRGBRange(byte r, byte g, byte b, int start, int end);
    void setHSLRange(byte h, byte s, byte l, int start, int end);
    void clear();

    void fillRGB(uint8_t r, uint8_t g, uint8_t b);
    void fillHSL(uint8_t h, uint8_t s, uint8_t l);
    void setMaxBrightness(byte b);
    void resetFrameCount();
    void nextFrame(char index); //shows next frame if in animation mode
    void cmd(String cmd);
    void loop();

  private:
    bus_t bus;
    byte _max_bright;
    int frame_index;
    char spd;
    char eff_num;

    leds pixels[];
    byte oldh_list[];
    void randomize();
    void fx_aurora();
    void fx_rainbow();
    void fx_wavebow();
    void fx_opposites();
    void fx_opp_seg();
};
