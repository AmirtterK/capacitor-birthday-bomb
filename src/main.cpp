/*
  Happy Birthday OLED (from happy_birth_day_oled_V2) - Arduino UNO + OLED only

  Uses the frame data from the zip (Capacitor.h, you_thought.h).
  The full animations are ~320 KB and an Uno has 32 KB of flash, so only an
  evenly-spaced subset of frames is kept. Everything else (button, buzzer,
  LEDs, NeoPixel) is removed - it plays automatically and loops: countdown -> goodbye capacitor -> you thought -> repeat.

  Wiring: OLED GND->GND, VCC->5V, SCL->A5, SDA->A4
  Libraries: Adafruit GFX, Adafruit SH110X
  Put this file in the same folder as Capacitor.h and you_thought.h.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "Capacitor.h"
#include "you_thought.h"

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDR   0x3C   // try 0x3D if blank

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Frame subsets (only these are linked into flash)
const uint8_t* const capFrames[] PROGMEM = {
  frameCap0, frameCap11, frameCap22, frameCap33,
  frameCap44, frameCap54, frameCap65, frameCap76,
  frameCap87
};
const uint8_t num_capFrames = 9;
const uint8_t* const ytFrames[] PROGMEM = {
  frameYT0, frameYT16, frameYT33, frameYT50,
  frameYT66, frameYT82
};
const uint8_t num_ytFrames = 6;


// Total time each scene runs (ms) - frames are spread over this time
#define COUNTDOWN_MS  5000UL
#define GOODBYE_MS    5000UL
#define YOUTHOUGHT_MS 3000UL

void playFrames(const uint8_t* const* table, byte count, unsigned long totalMs) {
  unsigned long start = millis();
  unsigned long elapsed;
  while ((elapsed = millis() - start) < totalMs) {
    byte idx = (elapsed * count) / totalMs;
    if (idx >= count) idx = count - 1;
    const uint8_t* ptr = (const uint8_t*)pgm_read_ptr(&table[idx]);
    display.clearDisplay();
    display.drawBitmap(0, 0, ptr, 128, 64, SH110X_WHITE);
    display.display();
  }
}

void countdownScene() {
  unsigned long start = millis();
  unsigned long elapsed;
  while ((elapsed = millis() - start) < COUNTDOWN_MS) {
    unsigned long remaining = COUNTDOWN_MS - elapsed;
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(32, 4);
    display.print(F("WARNING!!"));
    display.setTextSize(2);
    display.setCursor(10, 26);
    display.print(remaining / 1000);
    display.print('.');
    byte hundredths = (remaining % 1000) / 10;
    if (hundredths < 10) display.print('0');
    display.print(hundredths);
    int barWidth = map(remaining, 0, COUNTDOWN_MS, 0, 120);
    display.drawRect(4, 52, 120, 8, SH110X_WHITE);
    display.fillRect(4, 52, barWidth, 8, SH110X_WHITE);
    display.display();
    delay(50);
  }
}

void setup() {
  display.begin(SCREEN_ADDR, true);
  display.clearDisplay();
  display.display();
  delay(500);
}

void loop() {
  countdownScene();
  playFrames(capFrames, num_capFrames, GOODBYE_MS);
  playFrames(ytFrames, num_ytFrames, YOUTHOUGHT_MS);
}
