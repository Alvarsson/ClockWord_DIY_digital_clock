#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "clock.h"

#define ROWS ((2 * LEDS_PER_SEGMENT) + 2)
#define DISPLAY_COLUMN_GAP 2
#define DISPLAY_DOTS_GAP 5
#define ANIMATION_COLUMNS ((4 * COLUMNS_PER_DISPLAY) + (2 * DISPLAY_COLUMN_GAP) + DISPLAY_DOTS_GAP)

typedef struct {
  CRGB start_color;
  CRGB end_color;
  uint8_t start_pos;
  uint8_t end_pos;
  uint8_t len;
} rgb_wave;


CRGB calculate_offset_color(CRGB start_color, CRGB end_color, uint16_t total, uint16_t offset);
void init_waves(rgb_wave *wave_storage, CRGB *colors, uint8_t color_count, uint8_t total_len);

void display_rgb_wave(clock_t *clock, rgb_wave *wave, uint8_t progress);
void display_rgb_circle(clock_t *clock, rgb_wave *wave, uint8_t progress);



#endif
