#include <NeoPixelBus.h>

#define STRIP_SIZE 30
#define TEST_DELAY 15

#define REL_UNIT_BYTE 0.0039f
#define REL_UNIT_SIZE 1.0f /  STRIP_SIZE

#define CMD_TOGGLE "toggle"
#define CMD_FX "fx"
#define CMD_BR "br"
#define CMD_HU "hu"
#define CMD_SPD "spd"

#define CMD_L_TOGGLE "ltoggle"
#define CMD_L_BR  "lbr"
#define CMD_L_HU  "lhu"

#define CMD_R_TOGGLE "rtoggle"
#define CMD_R_BR  "rbr"
#define CMD_R_HU  "rhu"

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

class Strip {
  public:
    Strip();
    void test();
    void setRGBRange(byte r, byte g, byte b, int start, int end);
    void setHSLRange(byte h, byte s, byte l, int start, int end);
    void setHSBRange(byte h, byte s, byte b, int start, int end);
    
    void clear();

    void fillRGB(uint8_t r, uint8_t g, uint8_t b);
    void fillHSL(uint8_t h, uint8_t s, uint8_t l);    

    void resetFrameCount();
    void nextFrame(char index); //shows next frame if in animation mode
    void cmd(String cmd, String payload);
    void loop();

  private:
    byte _max_bright;
    byte _l_bright;
    byte _r_bright;
    byte _hue;
    byte _r_hue;
    byte _l_hue;
    byte _l_on;
    byte _r_on;

    int frame_index;
    char spd;
    char eff_num;
    byte playing;

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
