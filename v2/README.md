# Korean word clock v2
A wall-mounted clock that reads out the current time in Korean.

Change log from v1:
- Uses an ATmega32U4 microcontroller instead of the RP2040. 
- PCB is drawn in KiCad instead of EAGLE.

## Bill of materials
* SparkFun Qwiic Pro Micro
    * [Core Electronics](https://core-electronics.com.au/sparkfun-qwiic-pro-micro-usb-c-atmega32u4.html)
* Pimoroni RV3028 Breakout
    * [Core Electronics](https://core-electronics.com.au/pimoroni-rv3028-real-time-clock-rtc-breakout.html)
* LED 3-pin JST-SM pigtail connectors
    * [Core Electronics](https://core-electronics.com.au/led-strip-pigtail-connector-3-pin.html)
* 12mm momentary push button
    * [Core Electronics](https://core-electronics.com.au/momentary-push-button-switch-12mm-square.html)
    * [Jaycar](https://www.jaycar.com.au/spst-pcb-mount-tactile-switch-round/p/SP0609)

## Translations
Korean | English
------ | -------
한시 | one o'clock
두시 | two o'clock
세시 | three o'clock
네시 | four o'clock
다섯시 | five o'clock
여섯시 | six o'clock
일곱시 | seven o'clock
여덟시 | eight o'clock
아홉시 | nine o'clock
열시 | ten o'clock
열한시 | eleven o'clock
열두시 | twelve o'clock
자정 | midnight
정오 | noon
오분 | five minutes
십분 | ten minutes
이십분 | twenty minutes
삼십분 | thirty minutes
사십분 | forty minutes
오십분 | fifty minutes

## Acknowledgements and references
* [HangulClock](https://suapapa.github.io/site-hangulclocks/) by Homin Lee
* [Adafruit NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel)
* [SparkFun KiCad Libraries](https://github.com/sparkfun/SparkFun-KiCad-Libraries)
