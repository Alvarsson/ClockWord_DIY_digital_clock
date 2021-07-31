#include "animations.h"

#define SW_RX 6
#define SW_TX 7
#define COLOR_COUNT 4

#define BRIGHTNESS 100

CRGB wave_colors[] = {CRGB(0, BRIGHTNESS, 0), CRGB(BRIGHTNESS, 0, 0), CRGB(BRIGHTNESS, BRIGHTNESS, 0), CRGB(BRIGHTNESS, 0, BRIGHTNESS)};
rgb_wave waves[COLOR_COUNT];
clock_t clock;

uint32_t leet_start = 49020;
uint32_t leet_end = 49020 + 60;
uint32_t yes_start = 15600;
uint32_t yes_end = 15600 + 60;
uint32_t current_time_seconds = 10;

uint8_t timer_overflow_count;

// Serial and FastLED also use
// interrupts, so don't use them/call
// functions that use them from this
// interrupt handler
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

#define TIME_CHARS 8
#define HOURS_MSD_OFFSET 0
#define HOURS_LSD_OFFSET 1
#define MINUTES_MSD_OFFSET 3
#define MINUTES_LSD_OFFSET 4
#define SECONDS_MSD_OFFSET 6
#define SECONDS_LSD_OFFSET 7
void get_time() {
  uint32_t now = current_time_seconds;
  
  Serial2.write("AT+CIPSNTPTIME?\r\n", 18);

  int space_count = 0;
  uint32_t loop_count = 0;
  while (space_count < 3) {
    int input = Serial2.read();
    Serial.write(input);
    if (input == ' ') {
      space_count++;
    }
    loop_count++;
    if (loop_count == 400000) {
      return;
    }
  }

  uint8_t all_str[TIME_CHARS];
  Serial2.readBytes(all_str, TIME_CHARS);

  uint32_t hours = ((all_str[HOURS_MSD_OFFSET] - '0') * 10) + (all_str[HOURS_LSD_OFFSET] - '0');
  uint32_t minutes = ((all_str[MINUTES_MSD_OFFSET] - '0') * 10) + (all_str[MINUTES_LSD_OFFSET] - '0');
  uint32_t seconds = ((all_str[SECONDS_MSD_OFFSET] - '0') * 10) + (all_str[SECONDS_LSD_OFFSET] - '0');

  Serial2.readBytes(all_str, 6);

  current_time_seconds = (hours * 3600) + (minutes * 60) + seconds;

}

#undef ECHO
#ifdef ECHO
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("Begin!");
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t val = Serial.read();
    Serial2.write(val);
  }
  while (Serial2.available() > 0) {
    uint8_t val = Serial2.read();
    Serial.write(val);
  }
}

#else

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);  

  Serial2.print("AT+CIPSNTPCFG=1,2,\"se.pool.ntp.org\"\r\n");

  uint8_t read_bytes = 0;
  
  while (read_bytes < 4) {
    int value = Serial2.read();
    if (value > 0) {
      read_bytes++;
    }
  }


  FastLED.addLeds<NEOPIXEL, DISPLAY0_PIN>(clock.displays[0].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY1_PIN>(clock.displays[1].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY2_PIN>(clock.displays[2].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY3_PIN>(clock.displays[3].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DOT_UP_PIN>(clock.dots, LEDS_PER_DOT);
  FastLED.addLeds<NEOPIXEL, DOT_DOWN_PIN>(clock.dots + LEDS_PER_DOT, LEDS_PER_DOT);
  FastLED.show();

  OCR1A = 200;
  TCCR1B = (1 << CS12);
  TIMSK1 = (1 << OCIE1A);

  init_waves(waves, wave_colors, COLOR_COUNT, LEDS_PER_SEGMENT * 12);
}

uint8_t direction = 0;
#define STEP 5
#define BRIGHTNESS_MIN 10
#define BRIGHTNESS_MAX 255
uint8_t brightness = BRIGHTNESS_MIN;

void loop() {
  while (Serial2.available()) Serial2.read();
  _delay_ms(10);
  if (current_time_seconds % 30 == 0) {
    get_time();
  }

  uint8_t is_first_minute = 0;
  uint16_t seconds_into_hour = current_time_seconds % 3600;
  int32_t data = current_time_seconds - 43200;
  if (data < 0) {
    data *= -1;
  }
  data = 102 - ((102 * data) / 43200);
  if (seconds_into_hour >= 0 && seconds_into_hour < 60) {
    is_first_minute = 1;
  }
  
  if (direction == 0){
    brightness += STEP;
  } else {
    brightness -= STEP;
  }
  if (brightness == BRIGHTNESS_MAX || brightness == BRIGHTNESS_MIN) {
    direction = !direction;
  }

  if (current_time_seconds >= 0 && current_time_seconds < 60) {
    set_clock_color(&clock, CRGB(brightness, 0, 0));
  } else {
    if (current_time_seconds >= yes_start && current_time_seconds < yes_end) {
      set_clock_color(&clock, CRGB(0, brightness, 0));
    } else if ((current_time_seconds >= leet_start && current_time_seconds < leet_end) || is_first_minute){
      for (uint8_t i = 0; i < COLOR_COUNT; i++){
        display_rgb_wave(&clock, waves + i, 3);     
      }
    } else {
      set_clock_color(&clock, CRGB(50, data, 0));    
    }
    display_time(&clock, current_time_seconds);
  }
  FastLED.show();

}
#endif
