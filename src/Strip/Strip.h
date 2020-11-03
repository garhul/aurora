#include <NeoPixelBus.h>

#define TEST_DELAY 15
#define REL_UNIT_BYTE 0.0039f
#define REL_UNIT_SIZE 1.0f /  STRIP_SIZE
// #define BENCHMARK
#define MAX_LENGTH 300 //TODO:: temporal solution for dinamic array implementation

#define CMD_FX "fx"
#define CMD_OFF "off"
#define CMD_SETHSL "set"
#define CMD_PLAY "play"
#define CMD_PAUSE "pause"
#define CMD_TEST "test"
#define CMD_BR "br"
#define CMD_SPEED "spd"

// #define WEMOS_D1

#ifdef WEMOS_D1
typedef NeoPixelBus<NeoGrbFeature, NeoEsp8266AsyncUart1800KbpsMethod> NeoPixelBusType; // USES D4
#else
typedef NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> NeoPixelBusType;   // USES RX pin in nodemcu
#endif

typedef struct {
  byte hue;
  byte sat;
  byte br;
}  leds;

enum MODES {
  OFF,
  PAUSED,
  PLAYING
};

enum FX {
  NONE,
  RAINBOW,
  WAVEBOW,
  AURORA,
  OPPOSITES,
  HUE_SPLIT,
  CHASER,
  WHITE_AURORA,
  WHITE_CHASER,
  TRIP,
  FIRE,
  COUNT
};

class Strip {
public:
  Strip(uint16 length);
  void test();
  void setRGBRange(byte r, byte g, byte b, int start, int end);
  void setHSLRange(byte h, byte s, byte l, int start, int end);
  void clear();

  void fillRGB(uint8_t r, uint8_t g, uint8_t b);
  void fillHSL(uint8_t h, uint8_t s, uint8_t l);
  void setMaxBrightness(byte b);
  void resetFrameCount();
  void nextFrame(char index); //shows next frame if in animation mode
  void cmd(String cmd, String payload);
  void loop();

private:

  // NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma400KbpsMethod>* bus;
  // NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod>* bus;
  // NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* bus;
  // NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* bus;
  // NeoPixelBus<NeoGrbFeature, NeoEsp8266AsyncUart0800KbpsMethod>* bus;
  NeoPixelBusType* bus;

  byte _max_bright;
  uint16 size;
  int frame_index;
  char spd;
  char fx;
  byte mode;
  leds pixels[MAX_LENGTH];
  void randomize();
  void fx_aurora();
  void fx_rainbow();
  void fx_wavebow();
  void fx_opposites();
  void fx_hue_split();
  void fx_chaser();
  void fx_chaser_double();
  void fx_white_aurora();
  void fx_white_chaser();
  void fx_trip();
  void fx_fire();
};

