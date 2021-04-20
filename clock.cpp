#include "clock.h"

void set_circle_index(display_t *display_list, uint8_t index, CRGB color) {
  index = index % (12 * LEDS_PER_SEGMENT);
  display_t *display = display_list;
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
