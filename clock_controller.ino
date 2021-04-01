#include "FastLED.h"

#define DISPLAYS 4
#define SEGMENTS_PER_DISPLAY 7
#define LEDS_PER_SEGMENT 6
#define LEDS_PER_DISPLAY LEDS_PER_SEGMENT * SEGMENTS_PER_DISPLAY

#define DISPLAY0_PIN 2
#define DISPLAY1_PIN 3
#define DISPLAY2_PIN 4
#define DISPLAY3_PIN 5

const char *all = "abcdefg";
const char *zero = "abcdef";
const char *one = "cd";
const char *two = "bcefg";
const char *three = "bcdeg";
const char *four = "acdg";
const char *five = "abdeg";
const char *six = "abdefg";
const char *seven = "bcd";
const char *eight = "abcdefg";
const char *nine = "abcdg";

const char *numbers[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

typedef struct {
  CRGB leds[LEDS_PER_DISPLAY];
} display_t;

display_t displays[DISPLAYS];

void filter_display_segments(display_t *display, const char* segments) {
  char segments_to_filter[SEGMENTS_PER_DISPLAY + 1];
  
  uint8_t segment_count;
  const char *segment_ptr = segments;
  for (segment_count = 0; *segment_ptr++ != 0; segment_count++);

  uint8_t segments_out = SEGMENTS_PER_DISPLAY - segment_count;
  segments_to_filter[segments_out] = 0;

  char *filtered = segments_to_filter;  

  for (int i = 0; i < SEGMENTS_PER_DISPLAY; i++) {
    for (int x = 0; x < segment_count; x++){
      if (segments[x] - 'a' == x) {
        *filtered++ = x + 'a';
      }
    }
  }

  set_number(display, segments_to_filter, CRGB(0, 0, 0));
  
}

void clear_display(display_t *display) {
  for (uint8_t i = 0; i < LEDS_PER_DISPLAY; i++){
    display->leds[i] = CRGB(0, 0, 0);
  }
}

void set_number(display_t *display, const char* segments, CRGB color) {
  do {
    uint8_t actual_segment = *segments;
    if (actual_segment >= 'a' && actual_segment <= 'g') {
      uint8_t segment_num = actual_segment - 'a';
      uint8_t start = segment_num * LEDS_PER_SEGMENT;
      uint8_t stop = (segment_num + 1) * LEDS_PER_SEGMENT;
    
      for (uint8_t i = start; i < stop; i++) {
        display->leds[i] = color;
      }
    
    }
  } while (*segments++ != 0);
}

void setup() {
  FastLED.addLeds<NEOPIXEL, DISPLAY0_PIN>(displays[0].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY1_PIN>(displays[1].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY2_PIN>(displays[2].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY3_PIN>(displays[3].leds, LEDS_PER_DISPLAY);
  FastLED.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  static int current_num = 0;
  current_num = (current_num + 1) % 10;
  delay(50);
  for (int i = 0; i < DISPLAYS; i++){
    clear_display(&displays[i]);
  }   
  FastLED.show(); 
}
