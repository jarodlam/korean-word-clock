#include <Adafruit_NeoPixel.h>

#include "Button.h"
#include "Sk6812LedMatrix.h"
#include "Rv3028CLock.h"
#include "KoreanClockFace.h"

/*
 * Uncomment to set the time.
 * After setting time, comment out and re-upload.
 *
 * Format is:
 *   second, minute, hour (24h), weekday (0=Sunday), day, month, year
 */
// #define SET_TIME 00, 40, 10, 0, 15, 02, 2025

/*
 * Config
 */
#define PIN_LEDDATA 6
#define PIN_SW1 16
#define PIN_SW2 14
#define PIN_SW3 15
#define PIN_SW4 10

#define MATRIX_WIDTH 5
#define MATRIX_HEIGHT 5
#define MATRIX_LEDS_PER_CELL 2
// #define MATRIX_COUNT (MATRIX_HEIGHT * MATRIX_WIDTH * 2)

/*
 * Globals
 */
// Adafruit_NeoPixel leds{MATRIX_COUNT, PIN_LEDDATA, NEO_GRBW + NEO_KHZ800};
Sk6812LedMatrix matrix{MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_LEDS_PER_CELL,
                 PIN_LEDDATA};
uint8_t buffer[MATRIX_WIDTH][MATRIX_HEIGHT][4];
RV3028 rtc{};
// KoreanController controller{};

void setup() {
  Serial.begin(9600);
  Wire.begin();

  rtc.begin();
  matrix.begin();
  matrix.setBrightness(255);

#ifdef SET_TIME
  DateTime newTime = DateTime{SET_TIME};
  rtc.setDateTime(newTime);
  while (true) {
  } // Disable device in set time mode
#endif
}

void loop() {
  DateTime time = rtc.readDateTime();

  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      matrix.clear();
      matrix.setCell(x, y, 255, 255, 255, 255);
      matrix.show();
      delay(100);
    }
  }
}
