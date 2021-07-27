#include "clock.h"

void set_circle_index(clock_t *clock, uint8_t index, CRGB color) {
  index = index % (12 * LEDS_PER_SEGMENT);
  display_t *display = clock->displays;
  if (index < LEDS_PER_SEGMENT) {
    // Top left column
  } else if (index >= LEDS_PER_SEGMENT && index < 5 * LEDS_PER_SEGMENT) {
    // Top row
    display += ((index - LEDS_PER_SEGMENT) / LEDS_PER_SEGMENT);
    index = (index % LEDS_PER_SEGMENT) + LEDS_PER_SEGMENT;
  } else if (index >= 5 * LEDS_PER_SEGMENT && index < 7 * LEDS_PER_SEGMENT) {
    // Right column
    display += 3;
    index = (index - 5 * LEDS_PER_SEGMENT) + (LEDS_PER_SEGMENT * 2);
  } else if (index >= 7 * LEDS_PER_SEGMENT && index < 11 * LEDS_PER_SEGMENT) {
    // Bottom row
    display += (3 - ((index - (LEDS_PER_SEGMENT * 7)) / LEDS_PER_SEGMENT));
    index = (index % LEDS_PER_SEGMENT) + (LEDS_PER_SEGMENT * 4);
  } else {
    // Bottom left column
    index = (index % LEDS_PER_SEGMENT) + (LEDS_PER_SEGMENT * 5);
  }
  display->leds[index] = color;
}

void set_clock_color(clock_t *clock, CRGB color) {
  for (int i = 0; i < DISPLAYS; i++){
    set_display_color(&clock->displays[i], color);
  }
  for (int i = 0; i < DOTS * LEDS_PER_DOT; i++){
    if (clock->dots_on) {
      clock->dots[i] = color;
    } else {
      clock->dots[i] = CRGB(0, 0, 0);
    }
  }
}
