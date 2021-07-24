#include "animations.h"
#include <SoftwareSerial.h>

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
#define COLOR_COUNT 4

SoftwareSerial radioSerial = SoftwareSerial(SW_RX, SW_TX);

CRGB wave_colors[] = {CRGB(0, 50, 0), CRGB(50, 0, 0), CRGB(50, 50, 0), CRGB(50, 0, 50)};
rgb_wave waves[COLOR_COUNT];
clock_t clock;
uint32_t current_time_seconds = 49020 - 60;
uint8_t timer_overflow_count;

ISR(TIMER1_COMPA_vect) {
  TCNT0 = 0;
  timer_overflow_count++;
  if (timer_overflow_count == 250) {
    clock.dots_on = !clock.dots_on;
    current_time_seconds++;
    timer_overflow_count = 0;
  }
}

void display_time(clock_t *clock, uint32_t seconds) {
  uint32_t days = (seconds / 86400);
  uint32_t hours = (seconds % 86400) / 3600;
  uint32_t minutes = (seconds % 3600) / 60;

  disable_incorrect_segments(&clock->displays[0], numbers[hours / 10]);
  disable_incorrect_segments(&clock->displays[1], numbers[hours % 10]);
  disable_incorrect_segments(&clock->displays[2], numbers[minutes / 10]);
  disable_incorrect_segments(&clock->displays[3], numbers[minutes % 10]);

}

void setup() {
  cli();
  // 125 overruns = 1 second (clock @ 16Mhz)
  OCR1A = 250;
  TCCR1B = (1 << CS12);
  TIMSK1 = (1 << OCIE1A);
  sei();

  clock.dots_on = 1;

  FastLED.addLeds<NEOPIXEL, DISPLAY0_PIN>(clock.displays[0].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY1_PIN>(clock.displays[1].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY2_PIN>(clock.displays[2].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY3_PIN>(clock.displays[3].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DOT_UP_PIN>(clock.dots, LEDS_PER_DOT);
  FastLED.addLeds<NEOPIXEL, DOT_DOWN_PIN>(clock.dots + LEDS_PER_DOT, LEDS_PER_DOT);
  FastLED.show();

  init_waves(waves, wave_colors, COLOR_COUNT, LEDS_PER_SEGMENT * 12);

}

void get_time() {
  uint8_t all_str[TIME_CHARS];

  radioSerial.print("AT+CIPSNTPTIME?\r\n");
  radioSerial.readBytes(all_str, START_CHARS);
  radioSerial.readBytes(all_str, TIME_CHARS);

  // Don't remove printouts: they make it work?
  Serial.println("Time chars: ");
  for (int i = 0; i < TIME_CHARS; i++) {
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

}

void loop() { 
  delay(16);
  for (uint8_t i = 0; i < COLOR_COUNT; i++){
      display_rgb_wave(&clock, waves + i, 1);     
  }
  display_time(&clock, current_time_seconds);
  cli();
  FastLED.show();
  sei();
}
