#include <NeoPixelBus.h>

#define STRIP_SIZE 50
#define TEST_DELAY 15

#define REL_UNIT_BYTE 0.0039f
#define REL_UNIT_SIZE 1.0f /  STRIP_SIZE

#define CMD_ON "on"
#define CMD_OFF "off"
#define CMD_FX "fx"
#define CMD_BR "br"
#define CMD_HU "hsl"
#define CMD_SPD "spd"

#define CMD_PLAY "play"
#define CMD_PAUSE "pause"

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
  WAVE
};

typedef NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> NeoPixelBusType;

class Strip {
  public:
    Strip(uint16 length) : size() {};
    void test();
    void setRGBRange(byte r, byte g, byte b, int start, int end);
    void setHSLRange(byte h, byte s, byte l, int start, int end);
    void setHSBRange(byte h, byte s, byte b, int start, int end);
    void setLength();
    void clear();

    void fillRGB(uint8_t r, uint8_t g, uint8_t b);
    void fillHSL(uint8_t h, uint8_t s, uint8_t l);    

    void resetFrameCount();
    void nextFrame(char index); //shows next frame if in animation mode
    void cmd(String cmd, String payload);
    void loop();

  private:
    byte _max_bright;
    NeoPixelBusType bus;
    int frame_index;
    char spd;
    char eff_num;
    byte playing;
    const uint16 size;

    leds pixels[STRIP_SIZE];
    byte oldh_list[STRIP_SIZE];
    void randomize();
    void fx_aurora();
    void fx_rainbow();
    void fx_wavebow();
    void fx_opposites();
    void fx_opp_seg();
    void fx_wave();
    void pause();
    void play();
};
