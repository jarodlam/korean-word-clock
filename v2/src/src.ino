#include "Button.h"
#include "RV3028.h"

/*
 * Uncomment to set the time.
 * After setting time, comment out and re-upload.
 * 
 * Format is:
 *   second, minute, hour (24h), weekday (0=Sunday), day, month, year
 */
// #define SET_TIME 00, 31, 18, 0, 12, 01, 2025

/*
 * Config
 */


/*
 * Globals
 */
RV3028 rtc{};

void setup() {
  Serial.begin(9600);
  Wire.begin();

  rtc.begin();

  #ifdef SET_TIME
  DateTime newTime = DateTime{SET_TIME};
  rtc.setDateTime(newTime);
  while (true) {}  // Disable clock in set time mode
  #endif
}

void loop() {
  DateTime time = rtc.readDateTime();
  Serial.print(time.year);
  Serial.print(" ");
  Serial.print(time.month);
  Serial.print(" ");
  Serial.print(time.day);
  Serial.print(" ");
  Serial.print(time.weekday);
  Serial.print(" ");
  Serial.print(time.hour);
  Serial.print(" ");
  Serial.print(time.minute);
  Serial.print(" ");
  Serial.print(time.second);
  Serial.print(" ");
  Serial.println();
  delay(1000);
}
