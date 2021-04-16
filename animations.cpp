#include "animations.h"

CRGB calculate_offset_color(CRGB start_color, CRGB end_color, uint16_t total, uint16_t offset) {
  offset = offset * 255 / total;
  CRGB color;
  color.r = start_color.r + (((end_color.r - start_color.r) * offset) / 255);
  color.g = start_color.g + (((end_color.g - start_color.g) * offset) / 255);
  color.b = start_color.b + (((end_color.b - start_color.b) * offset) / 255);
  return color;
}

void display_rgb_circle(display_t *display_list, rgb_wave *wave, uint8_t progress) {
  wave->start_pos += progress;
  wave->end_pos += progress;

  if (wave->start_pos > wave->end_pos) {
    wave->start_pos -= wave->len;
    wave->end_pos -= wave->len;
  }

  uint8_t start = wave->start_pos;
  uint8_t end = wave->end_pos;

  CRGB sc = wave->start_color;
  CRGB ec = wave->end_color;
  uint16_t total_distance = end - start;

  for (uint8_t i = 0; i < total_distance; i++) {
    // Calculate the current
    CRGB color = calculate_offset_color(sc, ec, total_distance, i);
    set_circle_index(display_list, (start + i) % wave->len, color);
  }
}


void display_rgb_wave(display_t *display_list, rgb_wave *wave, uint8_t progress) {
  wave->start_pos += progress;
  wave->end_pos += progress;

  if (wave->start_pos > wave->end_pos) {
    wave->start_pos -= wave->len;
    wave->end_pos -= wave->len;
  }

  uint8_t start = wave->start_pos;
  uint8_t end = wave->end_pos;

  CRGB sc = wave->start_color;
  CRGB ec = wave->end_color;
  uint16_t total_distance = end - start;

  for (uint8_t i = 0; i < total_distance; i++) {
    // Calculate the current
    CRGB color = calculate_offset_color(sc, ec, total_distance, i);
    set_column(display_list, (start + i) % wave->len, color);
  }
}

// Storage must be able to hold color_count waves
void init_waves(rgb_wave *wave_storage, CRGB *colors, uint8_t color_count, uint8_t total_len) {
  for (uint8_t i = 0; i < color_count; i++) {
    uint8_t start = (i * total_len) / (color_count);
    uint8_t end = ((i + 1) * total_len) / (color_count);
    if (i != color_count - 1) {
      wave_storage[i] = {colors[i], colors[i + 1] , start, end, total_len};
    } else {
      wave_storage[i] = {colors[i], colors[0], start, end, total_len};
    }
  }
}
