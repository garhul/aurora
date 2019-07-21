#include <NeoPixelBus.h>
#include <WiFiUdp.h>
#include <Strip.h>


#define USE_UDP false
#define UDP_PORT 5000
#define BUFFER_SIZE 512 // in order to receive frames we need to specify such frames
#define DEBUG false

//*==== define commands =====**//
#define CMD_SET_RANGE 0x01
#define CMD_FILL      0x02
#define CMD_OFF       0x03
#define CMD_TEST      0x04
#define CMD_FILL_HSL  0x08
#define CMD_SET_BRIGHTNESS 0x05
#define CMD_PLAY_ANIMATION 0x06
#define CMD_RUN_EFFECT 0x07

class Rainbow {
  public:
    Rainbow();
    void begin();
    void service(); //runs service inside the loop method
    void run(byte cmd[], byte len);

  private:
    bool playing;
    byte eff_playing;
    byte data[BUFFER_SIZE];
    Strip Panel;
    WiFiUDP UDP;
};
