#include "led_clock.h"
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

SoftwareSerial radioSerial = SoftwareSerial(SW_RX, SW_TX);

display_t displays[DISPLAYS];
uint32_t current_time_seconds = 49020 - 60;

void display_time(display_t *display_list, uint32_t seconds) {
  uint32_t days = (seconds / 86400);
  uint32_t hours = (seconds % 86400) / 3600;
  uint32_t minutes = (seconds % 3600) / 60;

  disable_incorrect_segments(&displays[0], numbers[hours / 10]);
  disable_incorrect_segments(&displays[1], numbers[hours % 10]);
  disable_incorrect_segments(&displays[2], numbers[minutes / 10]);
  disable_incorrect_segments(&displays[3], numbers[minutes % 10]);

}

#define COLOR_COUNT 4
CRGB wave_colors[] = {CRGB(0, 50, 0), CRGB(50, 0, 0), CRGB(50, 50, 0), CRGB(50, 0, 50)};
rgb_wave waves[COLOR_COUNT];

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
  while (radioSerial.available()) radioSerial.read();

  delay(100);

  while (radioSerial.available()) radioSerial.read();

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
  // put your main code here, to run repeatedly:
  static int count = 0;
  delay(10);

  if (current_time_seconds % 60 == 0) {
    get_time();
  }

  if (count++ == 100) {
    current_time_seconds += 1;
    count = 0;
  }

  for (int i = 0; i < COLOR_COUNT; i++) {
    display_rgb_circle(displays, waves + i,  1);
  }

  FastLED.show();
}
