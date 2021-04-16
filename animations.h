#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "led_clock.h"

typedef struct {
  CRGB start_color;
  CRGB end_color;
  uint8_t start_pos;
  uint8_t end_pos;
  uint8_t len;
} rgb_wave;


CRGB calculate_offset_color(CRGB start_color, CRGB end_color, uint16_t total, uint16_t offset);
void init_waves(rgb_wave *wave_storage, CRGB *colors, uint8_t color_count, uint8_t total_len);

void display_rgb_wave(display_t *display_list, rgb_wave *wave, uint8_t progress);
void display_rgb_circle(display_t *display_list, rgb_wave *wave, uint8_t progress);

#endif
