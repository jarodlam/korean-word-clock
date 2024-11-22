# Sets the external RTC time to current local computer time.
# Run this in the Pi Pico REPL when connected to Thonny.

from rv3028 import RV3028

PIN_SDA = 20
PIN_SCL = 21

rtc = RV3028(pin_sda=PIN_SDA, pin_scl=PIN_SCL)

rtc.set_from_systime()
