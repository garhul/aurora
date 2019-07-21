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


class Strip {
  public:
    Strip();
    void test();
    void setRGBRange(byte r, byte g, byte b, int start, int end);
    void setHSLRange(byte h, byte s, byte l, int start, int end);
    void clear();

    void fillRGB(uint8_t r, uint8_t g, uint8_t b);
    void fillHSL(uint8_t h, uint8_t s, uint8_t l);
    void setMaxBrightness(byte b);
    void resetFrameCount();
    void nextFrame(char index); //shows next frame if in animation mode

  private:
    byte _max_bright;
    int frame_index;

    leds pixels[STRIP_SIZE];
    byte oldh_list[STRIP_SIZE];

    void _randomize();
    void _aurora();
    void _aurora2();
    void _eff_0();
    void _eff_1();
    void _eff_2();
    void _eff_4();
    void _eff_3();
    void _eff_5();
    void _eff_6();
    void _eff_7();
};
