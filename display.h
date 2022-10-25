#ifndef DISPLAY_H
#define DISPLAY_H

#include <FastLED.h>

#define SEGMENTS_PER_DISPLAY 7
#define LEDS_PER_SEGMENT 10
#define LEDS_PER_DOT 2
#define LEDS_PER_DISPLAY (LEDS_PER_SEGMENT * SEGMENTS_PER_DISPLAY)
#define COLUMNS_PER_DISPLAY (LEDS_PER_SEGMENT + 2)



#define DISPLAY0_PIN 58 // C0
#define DISPLAY1_PIN 56 // C1
#define DISPLAY2_PIN 55 // C2
#define DISPLAY3_PIN 54 // C3

typedef struct {
  CRGB leds[LEDS_PER_DISPLAY];
} display_t;

extern const char *all;
extern const char *zero;
extern const char *one;
extern const char *two;
extern const char *three;
extern const char *four;
extern const char *five;
extern const char *six;
extern const char *seven;
extern const char *eight;
extern const char *nine;
extern const char *numbers[10];

void set_segments(display_t *display, const char *segments, CRGB color);
void disable_incorrect_segments(display_t *display, const char *number);
void set_inverted_segments(display_t *display, const char *segments, CRGB color);
void clear_display(display_t *display);
void set_display_color(display_t *display, CRGB color);
uint8_t is_full_column(uint8_t column);
uint8_t get_led_at_xy(display_t *display, uint8_t horizontal, uint8_t vertical, CRGB **led);
void set_column(display_t *display, uint8_t column, CRGB color);
#endif
