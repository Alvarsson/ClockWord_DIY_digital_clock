#include "display.h"

#define DISPLAYS 4
#define LEDS_PER_DOT 3
#define DOTS 2
#define DOT_UP_PIN 6
#define DOT_DOWN_PIN 7

typedef struct {
  display_t displays[DISPLAYS];
  uint8_t dots_on;
  CRGB dots[LEDS_PER_DOT * DOTS];
} clock_t;

void set_circle_index(clock_t *clock, uint8_t index, CRGB color);
