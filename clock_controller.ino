#include "animations.h"


#define SW_RX 6
#define SW_TX 7
#define COLOR_COUNT 4

CRGB wave_colors[] = {CRGB(0, 50, 0), CRGB(50, 0, 0), CRGB(50, 50, 0), CRGB(50, 0, 50)};
rgb_wave waves[COLOR_COUNT];
clock_t clock;
uint32_t current_time_seconds = 49020 - 60;
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
  uint8_t all_str[TIME_CHARS];
  Serial.println("Getting time");
  Serial2.write("AT+CIPSNTPTIME?\r\n", 18);

  int space_count = 0;
  while (space_count < 3) {
    while (Serial2.available() == 0);
    uint8_t input = Serial2.read();
    if (input == ' '){
      space_count++;  
    }
  }

  Serial.println("Done");
  
  Serial2.readBytes(all_str, TIME_CHARS);
  while (Serial2.available()) Serial2.read();

  uint32_t hours = ((all_str[HOURS_MSD_OFFSET] - '0') * 10) + (all_str[HOURS_LSD_OFFSET] - '0');
  uint32_t minutes = ((all_str[MINUTES_MSD_OFFSET] - '0') * 10) + (all_str[MINUTES_LSD_OFFSET] - '0');
  uint32_t seconds = ((all_str[SECONDS_MSD_OFFSET] - '0') * 10) + (all_str[SECONDS_LSD_OFFSET] - '0');

  current_time_seconds = (hours * 3600) + (minutes * 60) + seconds;
  Serial.println(current_time_seconds);
}

#undef ECHO
#ifdef ECHO
void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("Begin!");
}

void loop() {
  if (Serial.available() > 0) {
    uint8_t val = Serial.read();
    Serial2.write(val);
  }
  if (Serial2.available() > 0) {
    uint8_t val = Serial2.read();
    Serial.write(val);
  }
}

#else

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  
  Serial.println("Setting time!");
  // Configure ESP8266 to configure NTP
  Serial2.print("AT+CIPSNTPCFG=1,2,\"se.pool.ntp.org\"\r\n");

  delay(500);
  while(Serial2.available() > 0) Serial2.read();
  
  Serial.println("Configuring timer");
  // 125 overruns = 1 second (clock @ 16Mhz)
  OCR1A = 250;
  TCCR1B = (1 << CS12);
  TIMSK1 = (1 << OCIE1A);

  Serial.println("Adding FastLED leds");
  FastLED.addLeds<NEOPIXEL, DISPLAY0_PIN>(clock.displays[0].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY1_PIN>(clock.displays[1].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY2_PIN>(clock.displays[2].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DISPLAY3_PIN>(clock.displays[3].leds, LEDS_PER_DISPLAY);
  FastLED.addLeds<NEOPIXEL, DOT_UP_PIN>(clock.dots, LEDS_PER_DOT);
  FastLED.addLeds<NEOPIXEL, DOT_DOWN_PIN>(clock.dots + LEDS_PER_DOT, LEDS_PER_DOT);
  FastLED.show();


  Serial.println("Initializing waves");
  init_waves(waves, wave_colors, COLOR_COUNT, LEDS_PER_SEGMENT * 12);

}

void loop() {
  if (current_time_seconds % 120 == 0) {
    get_time();
  }
  
}
#endif
