#include <Button.h>
#include <Sk6812LedMatrix.h>
#include <Rv3028Clock.h>
#include <KoreanClockFace.h>
#include <clockutils.h>
#include <stdint.h>

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
constexpr uint8_t PIN_LEDDATA = 6;
constexpr uint8_t PIN_SW1 = 16;
constexpr uint8_t PIN_SW2 = 14;
constexpr uint8_t PIN_SW3 = 15;
constexpr uint8_t PIN_SW4 = 10;
constexpr uint8_t MATRIX_LEDS_PER_CELL = 2;
constexpr uint8_t EEADDR_BRIGHTNESS = 0x00;

/*
 * Globals
 */
Sk6812LedMatrix matrix{ KOREAN_CLOCK_FACE_WIDTH, KOREAN_CLOCK_FACE_HEIGHT, MATRIX_LEDS_PER_CELL, PIN_LEDDATA };
Rv3028Clock rtc{};
KoreanClockFace clockFace{ matrix };
Button buttonTimePlus{ PIN_SW1 };
Button buttonTimeMinus{ PIN_SW3 };  // Labelled as "MODE"
Button buttonBrightness{ PIN_SW2 };

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

#ifdef SET_TIME
  DateTime newTime = DateTime{ SET_TIME };
  rtc.setDateTime(newTime);
  while (true) {}  // Disable device in set time mode
#endif

  matrix.begin();
  clockFace.setBrightness(rtc.eepromRead(EEADDR_BRIGHTNESS));
}

void loop() {
  DateTime dt = rtc.readDateTime();
  buttonTimePlus.update();
  buttonTimeMinus.update();
  buttonBrightness.update();

  if (buttonTimePlus.read()) {
  }
  if (buttonBrightness.read()) {
    uint8_t newBrightness = cycleBrightness(rtc.eepromRead(EEADDR_BRIGHTNESS));
    rtc.eepromWrite(EEADDR_BRIGHTNESS, newBrightness);
    clockFace.setBrightness(newBrightness);
  }

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
