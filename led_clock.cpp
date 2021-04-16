#include "led_clock.h"

const char *zero = "abcdef";
const char *one = "cd";
const char *two = "bcefg";
const char *three = "bcdeg";
const char *four = "acdg";
const char *five = "abdeg";
const char *six = "abdefg";
const char *seven = "bcd";
const char *eight = "abcdefg";
const char *nine = "abcdeg";
const char *all = eight;

const char *numbers[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

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

void set_column(uint8_t column, display_t *displays_list, CRGB color) {

  uint8_t should_display = 0;
  if (column < COLUMNS_PER_DISPLAY) {
    should_display = 1;
  } else if (column >= COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP && column < 2 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP) {
    column -= DISPLAY_COLUMN_GAP;
    should_display = 1;
  } else if (column >= 2 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP && column < 3 * COLUMNS_PER_DISPLAY + DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP) {
    column -= DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP;
    should_display = 1;
  } else if (column >= 3 * COLUMNS_PER_DISPLAY + 2 * DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP && column < 4 * COLUMNS_PER_DISPLAY + 2 * DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP) {
    column -= (2 * DISPLAY_COLUMN_GAP + DISPLAY_DOTS_GAP);
    should_display = 1;
  }

  if (should_display) {
    display_t *display = &displays_list[column / COLUMNS_PER_DISPLAY];
    column = column % COLUMNS_PER_DISPLAY;
    if (column == 0) {
      set_segments(display, "af", color);
    } else if (column == LEDS_PER_SEGMENT + 1) {
      set_segments(display, "cd", color);
    } else {
      display->leds[LEDS_PER_SEGMENT + column - 1] = color;
      display->leds[(5 * LEDS_PER_SEGMENT) - column] = color;
      display->leds[(6 * LEDS_PER_SEGMENT) + column - 1] = color;
    }
  }
}

void set_circle_index(display_t *display_list, uint8_t index, CRGB color) {
  index = index % (12 * LEDS_PER_SEGMENT);
  display_t *display;
  if (index < LEDS_PER_SEGMENT) {
    display = display_list;
  } else if (index >= LEDS_PER_SEGMENT && index < 5 * LEDS_PER_SEGMENT) {
    display = display_list + ((index - LEDS_PER_SEGMENT) / LEDS_PER_SEGMENT);
    index = (index % LEDS_PER_SEGMENT) + LEDS_PER_SEGMENT;
  } else if (index >= 5 * LEDS_PER_SEGMENT && index < 7 * LEDS_PER_SEGMENT) {
    display = display_list + 3;
    index = (index % (2 * LEDS_PER_SEGMENT)) + (LEDS_PER_SEGMENT * 2);
  } else if (index >= 7 * LEDS_PER_SEGMENT && index < 11 * LEDS_PER_SEGMENT) {
    display = display_list + (3 - ((index - (LEDS_PER_SEGMENT * 7)) / LEDS_PER_SEGMENT));
    index = (index % LEDS_PER_SEGMENT) + (LEDS_PER_SEGMENT * 4);
  } else {
    display = display_list;
    index = (index % LEDS_PER_SEGMENT) + (LEDS_PER_SEGMENT * 5);
  }
  display->leds[index] = color;
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
