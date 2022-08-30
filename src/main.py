from machine import Pin, I2C
import time
from neopixel import Neopixel

# For ISR exception handling
import micropython
micropython.alloc_emergency_exception_buf(100)

#############
# CONSTANTS #
#############

# 열 한 다 세 네
# 두 여 섯 일 곱
# 여 덟 아 홉 시
# 자 정 이 삼 십
# 사 오 십 오 분
# Format is (row, column)
STATES = {
    "한시": [(0,1), (2,4)],
    "두시": [(1,0), (2,4)],
    "세시": [(0,3), (2,4)],
    "네시": [(0,4), (2,4)],
    "다섯시": [(0,2), (1,2), (2,4)],
    "여섯시": [(1,1), (1,2), (2,4)],
    "일곱시": [(1,3), (1,3), (2,4)],
    "여덟시": [(2,0), (2,1), (2,4)],
    "아홉시": [(2,2), (2,3), (2,4)],
    "열시": [(0,0), (2,4)],
    "열한시": [(0,0), (0,1), (2,4)],
    "열두시": [(0,0), (1,0), (2,4)],
    "자정": [(3,0), (3,1)],
    "정오": [(3,1), (4,1)],
    "오": [(4,3)],
    "십분": [(3,4), (4,4)],
    "십오분": [(4,2), (4,3), (4,4)],
    "이십분": [(3,2), (4,2), (4,4)],
    "삼십분": [(3,3), (3,4), (4,4)],
    "사십분": [(4,0), (4,2), (4,4)],
    "오십분": [(4,1), (4,2), (4,4)]
}
LED_POSITIONS = [
    [(49, 48), (47, 46), (45, 44), (43, 42), (41, 40)],
    [(30, 31), (32, 33), (34, 35), (36, 37), (38, 39)],
    [(29, 28), (27, 26), (25, 24), (23, 22), (21, 20)],
    [(10, 11), (12, 13), (14, 15), (16, 17), (18, 19)],
    [( 9,  8), ( 7,  6), ( 5,  4), ( 3,  2), ( 1,  0)]
]
HOUR_STATES = {
    0: "열두시",
    1: "한시",
    2: "두시",
    3: "세시",
    4: "네시",
    5: "다섯시",
    6: "여섯시",
    7: "일곱시",
    8: "여덟시",
    9: "아홉시",
    10: "열시",
    11: "열한시"
}
MINUTE_STATES = {
    1: "십분",
    2: "이십분",
    3: "삼십분",
    4: "사십분",
    5: "오십분"
}
NUM_LEDS = 50
PIN_LED = 28
PIN_SDA = 20
PIN_SCL = 21
PIN_RTC_INT = 22
PIN_SW_POWER = 26
PIN_SW_TIME = 13
PIN_SW_BRIGHTNESS = 14
PIN_SW_MODE = 15
DEBOUNCE_MS = 50
FAST_FORWARD_MS = 1000


#########################
# CLASSES AND FUNCTIONS #
#########################

"""
Simple polling button handler.
"""
class ButtonInput:
    
    def __init__(self, pin_no, callback=None):
        self.pin = Pin(pin_no, Pin.IN)
        self.callback = callback
        self.previous_ticks = time.ticks_ms()
        self.toggled_ticks = time.ticks_ms()
        self.previous_value = self.pin.value()
        
    def update(self):
        ticks = time.ticks_ms()
        value = self.pin.value()
        
        # No change
        if value == self.previous_value:
            self.previous_ticks = ticks
            return value
        
        # Debouncing, wait before read
        if (ticks - self.previous_ticks) < DEBOUNCE_MS:
            return self.previous_value
        
        # Value changed
        self.previous_ticks = ticks
        self.toggle_ticks = ticks
        self.previous_value = value
        if self.callback is not None:
            self.callback(value)
        return value
        
    def time_pressed(self):
        if not self.previous_value:
            return 0
        return time.ticks_ms() - self.toggle_ticks


def callback_power(value):
    text = "on" if value else "off"
    print("Power {}".format(text))

"""
Turn on LEDs for a state.

:param leds: `Neopixel` object.
:param state: State to set from `STATES` dict.
"""
def set_state(leds, state):
    positions = STATES[state]
    for pos in positions:
        indices = LED_POSITIONS[pos[0]][pos[1]]
        for i in indices:
            leds.set_pixel(i, (0, 0, 0, 255))


"""
Update the time on the display based on current time.
Still need to call `leds.show()` after.

:param leds: `Neopixel` object.
:param mins_since_midnight: Current time in minutes since midnight.
"""
def update_time(leds, mins_since_midnight):
    hour = mins_since_midnight // 60
    minute = mins_since_midnight % 60
    minute_10 = minute // 10
    minute_1 = minute % 10

    leds.clear()

    # Midnight and noon
    if hour == 0 and minute < 5:
        set_state(leds, "자정")
        return
    if hour == 12 and minute < 5:
        set_state(leds, "정오")
        return

    # Hour
    hour = hour % 12  # Convert to 12-hour time
    set_state(leds, HOUR_STATES[hour])

    # Minute
    if minute < 5:
        # On the hour, no minutes
        return
    if minute >= 15 and minute < 20:
        # Special arrangement at 15 past
        set_state(leds, "십오분")
        return
    if minute_10 >= 1:
        set_state(leds, MINUTE_STATES[minute_10])
    if minute_1 >= 5:
        set_state(leds, "오")


"""
Loop through all times of day to test LED functionality.

:param leds: `Neopixel` object.
"""
def demo(leds):
    for mins_since_midnight in range(1440):
        print(mins_since_midnight)
        update_time(leds, mins_since_midnight)
        leds.show()
        time.sleep_ms(10)


########
# MAIN #
########

if __name__ == "__main__":
    leds = Neopixel(NUM_LEDS, 0, PIN_LED, "GRBW")
    sw_power = ButtonInput(PIN_SW_POWER, callback_power)
    sw_time = ButtonInput(PIN_SW_TIME)
    sw_brightness = ButtonInput(PIN_SW_BRIGHTNESS)
    sw_mode = ButtonInput(PIN_SW_MODE)
    i2c = I2C(0, sda=Pin(PIN_SDA), scl=Pin(PIN_SCL))
    # rtc =
    
    i2c.writeto_mem(41, 0x00, bytearray(0xA0 | 41, 0x01 | 0x02 | 0x10))
    
    time.sleep_ms(1000)
    
    bytes_returned = i2c.readfrom_mem(41, 0x10, 1)
    print(bytes_returned)

    while True:
        #demo(leds)
        
        # Power
        if not sw_power.update():
            leds.clear()
            leds.show()
            time.sleep_ms(10)
            continue
        
        # Update values
        sw_time.update()
        sw_brightness.update()
        sw_mode.update()
        
        if sw_time.time_pressed() > FAST_FORWARD_MS:
            print("Time")
        
        if sw_brightness.time_pressed() > FAST_FORWARD_MS:
            print("Brightness")
        
        if sw_mode.time_pressed() > FAST_FORWARD_MS:
            print("Mode")
        
        time.sleep_ms(10)
