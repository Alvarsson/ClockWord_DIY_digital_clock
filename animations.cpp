#include "animations.h"

CRGB calculate_offset_color(CRGB start_color, CRGB end_color, uint16_t total, uint16_t offset) {
  offset = offset * 255 / total;
  CRGB color;
  color.r = start_color.r + (((end_color.r - start_color.r) * offset) / 255);
  color.g = start_color.g + (((end_color.g - start_color.g) * offset) / 255);
  color.b = start_color.b + (((end_color.b - start_color.b) * offset) / 255);
  return color;
}

void display_rgb_circle(clock_t *clock, rgb_wave *wave, uint8_t progress) {
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
    set_circle_index(clock, (start + i) % wave->len, color);
  }
}

uint8_t get_display_column(uint8_t animation_column, uint8_t *display_column) {
  if (animation_column < COLUMNS_PER_DISPLAY) {
    *display_column = animation_column;
  } else if (animation_column >= COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP && animation_column < 2 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP){
    *display_column = animation_column - DISPLAY_COLUMN_GAP;
  } else if (animation_column >= 2 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP && animation_column < 3 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP){
    *display_column = animation_column - (DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP);
  } else if (animation_column >= 3 * COLUMNS_PER_DISPLAY + 2 * DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP && animation_column < 4 * COLUMNS_PER_DISPLAY + 2 * DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP) {
    *display_column = animation_column - (2 * DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP);
  } else {
    return 0;
  }
  return 1;
}


void display_rgb_wave(clock_t *clock, rgb_wave *wave, uint8_t progress) {
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
    uint8_t display_column;
    uint8_t offset = (start + i) % wave->len;
    /// The dots
    if (offset == 2 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP/2) {
      for (int x = 0; x <  LEDS_PER_DOT * DOTS; x++){
        if (clock->dots_on) {
          clock->dots[x] = color;
        } else {
          clock->dots[x] = CRGB(0, 0, 0);
        }
      }
    } else if (get_display_column((start + i) % wave->len, &display_column)) {
      display_t *display = clock->displays + (display_column / COLUMNS_PER_DISPLAY); 
      set_column(display, (display_column) % COLUMNS_PER_DISPLAY, color);
    }
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
