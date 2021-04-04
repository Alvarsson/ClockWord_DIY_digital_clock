#include "FastLED.h"
#include <SoftwareSerial.h>

#define DISPLAYS 4
#define SEGMENTS_PER_DISPLAY 7
#define LEDS_PER_SEGMENT 6
#define LEDS_PER_DISPLAY LEDS_PER_SEGMENT * SEGMENTS_PER_DISPLAY
#define COLUMNS (LEDS_PER_SEGMENT + 2) * DISPLAYS

#define DISPLAY0_PIN 2
#define DISPLAY1_PIN 3
#define DISPLAY2_PIN 4
#define DISPLAY3_PIN 5

#define SW_RX 6
#define SW_TX 7

#define START_CHARS 17
#define TIME_CHARS 42
#define HOURS_MSD_OFFSET 24
#define HOURS_LSD_OFFSET 25
#define MINUTES_MSD_OFFSET 27
#define MINUTES_LSD_OFFSET 28
#define SECONDS_MSD_OFFSET 30
#define SECONDS_LSD_OFFSET 31

const char *all = "abcdefg";
const char *zero = "abcdef";
const char *one = "cd";
const char *two = "bcefg";
const char *three = "bcdeg";
const char *four = "acdg";
const char *five = "abdeg";
const char *six = "abdefg";
const char *seven = "bcd";
const char *eight = all;
const char *nine = "abcdg";

SoftwareSerial radioSerial = SoftwareSerial(SW_RX, SW_TX);

typedef struct {
  CRGB leds[LEDS_PER_DISPLAY];
} display_t;

typedef struct {
  CRGB start_color;
  CRGB end_color;
  uint8_t start_col;
  uint8_t end_col;
} rgb_wave;

void disable_incorrect_segments(display_t *display, char *number);
void set_column(uint8_t column, display_t *display_list, CRGB color);
void set_segments(display_t *display, char *segments, CRGB color);
void set_inverted_segments(display_t *display, char *segments, CRGB color);
void clear_display(display_t *display);
void display_time(display_t *display_list, uint32_t seconds);
void show_rgb_wave(rgb_wave *wave, display_t *display_list);

display_t displays[DISPLAYS];
uint32_t current_time_seconds = 0;

const char *numbers[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

void setup() {
  FastLED.addLeds<NEOPIXEL, DISPLAY0_PIN>(displays[0].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY1_PIN>(displays[1].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY2_PIN>(displays[2].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY3_PIN>(displays[3].leds, LEDS_PER_DISPLAY);
  FastLED.show();


  pinMode(SW_RX, INPUT);
  pinMode(SW_TX, OUTPUT);
  
  Serial.begin(4800);

  radioSerial.begin(4800);
  radioSerial.print("AT+CIPSNTPCFG=1,2,\"se.pool.ntp.org\"\r\n");
  // Discard response
  while (radioSerial.available()) Serial.println((char) radioSerial.read());

  delay(100);

  while (radioSerial.available()) Serial.println((char) radioSerial.read());

}

void getTime(){
  uint8_t all_str[TIME_CHARS];

  radioSerial.print("AT+CIPSNTPTIME?\r\n");
  radioSerial.readBytes(all_str, START_CHARS);
  radioSerial.readBytes(all_str, TIME_CHARS);

  // Don't remove printouts: they make it work?
  Serial.println("Time chars: ");
  for (int i = 0; i < TIME_CHARS; i++){
     Serial.print((char) all_str[i]);
  }
  Serial.println("");
  Serial.println("Time chars done");
  
  Serial.println("-----");
  Serial.println((char) all_str[HOURS_LSD_OFFSET]);
  Serial.println("-----");

  uint32_t hours = ((all_str[HOURS_MSD_OFFSET] - '0') * 10) + (all_str[HOURS_LSD_OFFSET] - '0');
  uint32_t minutes = ((all_str[MINUTES_MSD_OFFSET] - '0') * 10) + (all_str[MINUTES_LSD_OFFSET] - '0');
  uint32_t seconds = ((all_str[SECONDS_MSD_OFFSET] - '0') * 10) + (all_str[SECONDS_LSD_OFFSET] - '0');

  current_time_seconds = (hours * 3600) + (minutes * 60) + seconds;

  while (radioSerial.available()) radioSerial.read();

  Serial.println(current_time_seconds);
}

void loop() {
  // put your main code here, to run repeatedly:
  static int column_count = 0;
  static int down = 0;
  static rgb_wave wave = {CRGB(0, 255, 0), CRGB(255, 0, 0), 0, COLUMNS};

  delay(10);

  show_rgb_wave(&wave, displays);

//  if (column_count == 0){
//    down = 0;
//  } else if (column_count == COLUMNS){
//    down = 1;
//  }
//
//  if (down){
//    column_count--;
//    set_column(column_count, displays, CRGB(0, 0, 0));
//  } else {
//    set_column(column_count, displays, CRGB(0, 255, 0));
//    column_count++;
//  }
  
  FastLED.show();
}

/** Disable those segments that are not part of the given group of segments.
    For example: if you have a nice animation playing and want to show the
    number "1", run this function with numbers[1] and it should extinguish the
    segments that don't make "1"
*/
void disable_incorrect_segments(display_t *display, const char* segments) {
  set_inverted_segments(display, segments, CRGB(0, 0, 0));
}

void set_inverted_segments(display_t *display, const char* segments, CRGB color) {
  char segments_to_filter[SEGMENTS_PER_DISPLAY + 1];

  uint8_t segment_count;
  const char *segment_ptr = segments;
  for (segment_count = 0; *segment_ptr++ != 0; segment_count++);

  uint8_t segments_out = SEGMENTS_PER_DISPLAY - segment_count;
  segments_to_filter[segments_out] = 0;

  char *filtered = segments_to_filter;

  for (uint8_t i = 'a'; i < SEGMENTS_PER_DISPLAY + 'a'; i++) {
    uint8_t contains = 0;
    for (uint8_t x = 0; x < segment_count; x++) {
      if (segments[x] == i) {
        contains = 1;
      }
    }
    if (!contains) {
      *filtered++ = i;
    }
  }
  set_segments(display, segments_to_filter, color);

}

void display_time(display_t *displays, uint32_t seconds) {
  uint32_t days = (seconds / 86400);
  uint32_t hours = (seconds % 86400) / 3600;
  uint32_t minutes = (seconds % 3600) / 60;
  
  disable_incorrect_segments(&displays[0], numbers[hours / 10]);
  disable_incorrect_segments(&displays[1], numbers[hours % 10]);
  disable_incorrect_segments(&displays[2], numbers[minutes / 10]);
  disable_incorrect_segments(&displays[3], numbers[minutes % 10]);

}

void clear_display(display_t *display) {
  for (uint8_t i = 0; i < LEDS_PER_DISPLAY; i++) {
    display->leds[i] = CRGB(0, 0, 0);
  }
}

void set_segments(display_t *display, const char* segments, CRGB color) {
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

void set_column(int column, display_t *displays_list, CRGB color){
  display_t *display = &displays_list[column / (LEDS_PER_SEGMENT + 2)];
  column = column % (LEDS_PER_SEGMENT + 2);
  
  CRGB *leds[2 * LEDS_PER_SEGMENT];
  int led_count;
  if (column == 0) {
    set_segments(display, "af", color);
  } else if (column == LEDS_PER_SEGMENT + 1){
    set_segments(display, "cd", color);
  } else {
    led_count = 3;
    display->leds[LEDS_PER_SEGMENT + column - 1] = color;
    display->leds[(5 * LEDS_PER_SEGMENT) - column] = color;
    display->leds[(6 * LEDS_PER_SEGMENT) + column - 1] = color;
  }
}

void show_rgb_wave(rgb_wave *wave, display_t *display_list) {
  uint8_t start = wave->start_col;
  uint8_t end = wave->end_col;
  CRGB sc = wave->start_color;
  CRGB ec = wave->end_color;
  float total_distance = end - start;
  for (int i = start; i < end; i++){
    // Calculate the current 
    float current_distance = ((float) (start + i)) / total_distance; 
    CRGB color;
    color.r = sc.r + (ec.r - sc.r) * current_distance;
    color.g = sc.g + (ec.g - sc.g) * current_distance;
    color.b = sc.b + (ec.b - sc.b) * current_distance;
    set_column(i, display_list, color);
  }
}
