#ifndef _TYPES_H_
#define _TYPES_H_

typedef struct  {
  char ssid[32];
  char pass[32];
  char topic[32];
  char broker[32];
  unsigned int strip_size;
} settings_t;

#endif
