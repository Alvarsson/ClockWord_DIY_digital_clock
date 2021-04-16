#ifndef LED_CLOCK_H
#define LED_CLOCK_H

#include <FastLED.h>

#define DISPLAYS 4
#define SEGMENTS_PER_DISPLAY 7
#define LEDS_PER_SEGMENT 6
#define LEDS_PER_DISPLAY LEDS_PER_SEGMENT * SEGMENTS_PER_DISPLAY
#define COLUMNS ((LEDS_PER_SEGMENT + 2) * DISPLAYS)
#define COLUMNS_PER_DISPLAY (LEDS_PER_SEGMENT + 2)
#define DISPLAY_COLUMN_GAP 1
#define DISPLAY_DOTS_GAP 4
#define ANIMATION_COLUMNS ((4 * COLUMNS_PER_DISPLAY) + (2 * DISPLAY_COLUMN_GAP) + DISPLAY_DOTS_GAP)

#define DISPLAY0_PIN 2
#define DISPLAY1_PIN 3
#define DISPLAY2_PIN 4
#define DISPLAY3_PIN 5

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
void set_column(display_t *display_list, uint8_t column, CRGB color);
void set_circle_index(display_t *display_list, uint8_t index, CRGB color);

#endif
