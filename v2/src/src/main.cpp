#include <Button.h>
#include <Sk6812LedMatrix.h>
#include <Rv3028Clock.h>
#include <KoreanClockFace.h>

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
#define MATRIX_LEDS_PER_CELL 2

/*
 * Globals
 */
Sk6812LedMatrix matrix{ KOREAN_CLOCK_FACE_WIDTH, KOREAN_CLOCK_FACE_HEIGHT, MATRIX_LEDS_PER_CELL, PIN_LEDDATA };
Rv3028Clock rtc{};
KoreanClockFace clockFace{ matrix };

void setup() {
  Serial.begin(9600);
  Wire.begin();

  rtc.begin();
  matrix.begin();
  // clockFace.begin();
  // matrix.setBrightness(255);

#ifdef SET_TIME
  DateTime newTime = DateTime{ SET_TIME };
  rtc.setDateTime(newTime);
  while (true) {  // Disable device in set time mode
  }
#endif
}

void loop() {
  DateTime dt = rtc.readDateTime();

  Serial.print(dt.hour);
  Serial.print(":");
  Serial.print(dt.minute);
  Serial.print(":");
  Serial.print(dt.second);
  Serial.println();

  clockFace.update(dt);
  clockFace.show();

  delay(10);
}
