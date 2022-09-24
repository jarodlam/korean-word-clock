from machine import Pin, RTC
import time
from math import sin, cos, pi
from neopixel import Neopixel
from rv3028 import RV3028

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
    [(41, 40), (43, 42), (45, 44), (47, 46), (49, 48)],
    [(38, 39), (36, 37), (34, 35), (32, 33), (30, 31)],
    [(21, 20), (23, 22), (25, 24), (27, 26), (29, 28)],
    [(18, 19), (16, 17), (14, 15), (12, 13), (10, 11)],
    [(1, 0), (3, 2), (5, 4), (7, 6), (9, 8)]
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
NROW = 5
NCOL = 5
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
RTC_I2C_ADDR = 0x52


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

def rgb_full_value(r, g, b):
    """
    Convert an RGB colour to HSV, set V to 100%, then convert back.
    From https://en.wikipedia.org/wiki/HSL_and_HSV#Color_conversion_formulae.
    """
    # Convert to HSV
    xmax = max(r, g, b)
    xmin = min(r, g, b)
    c = xmax - xmin
    v = xmax
    if c == 0:
        h = 0
    elif v == r:
        h = 0 + (g - b) / c
    elif v == g:
        h = 2 + (b - r) / c
    elif v == b:
        h = 4 + (r - g) / c
    s = 0
    if v != 0:
        s = c / v
    
    # Set v to 100%
    v = 1
    s = min(s * 2, 1)
    
    # Convert back to RGB
    c = v * s
    x = c * (1 - abs((h % 2) - 1))
    if h < 1: rgb = (c, x, 0)
    elif h < 2: rgb = (x, c, 0)
    elif h < 3: rgb = (0, c, x)
    elif h < 4: rgb = (0, x, c)
    elif h < 5: rgb = (x, 0, c)
    elif h < 6: rgb = (c, 0, x)
    m = v - c
    rgb = (rgb[0] + m) * 255, (rgb[1] + m) * 255, (rgb[2] + m) * 255
    return round(rgb[0]), round(rgb[1]), round(rgb[2])
    

class ClockFace:
    
    def __init__(self, leds, nrow, ncol):
        self.leds = leds
        self.nrow = nrow
        self.ncol = ncol
        self.effect = self.effect_white
        self.internal_rtc = RTC()
        self.clear()
    
    def set_state(self, state):
        """
        Turn on LEDs for a state.

        :param state: State to set from `STATES` dict.
        """
        positions = STATES[state]
        for pos in positions:
            self.arr[pos[0]][pos[1]] = True
                
    def show(self):
        arr_rgbw = self.effect(self.arr)
        
        for row, columns in enumerate(arr_rgbw):
            for col, rgbw in enumerate(columns):
                indices = LED_POSITIONS[row][col]
                for i in indices:
                    self.leds.set_pixel(i, rgbw)
        self.leds.show()
        
    def clear(self):
        self.arr = [
            [False for _ in range(self.ncol)]
            for _ in range(self.nrow)
        ]
        self.leds.clear()

    def update_time(self, mins_since_midnight=None):
        """
        Update the time on the display based on current time.
        Still need to call `leds.show()` after.

        :param leds: `Neopixel` object.
        :param mins_since_midnight: Current time in minutes since midnight.
        """
        if mins_since_midnight is None:
            y, mo, d, wd, h, m, _, _ = self.internal_rtc.datetime()
            mins_since_midnight = h * 60 + m
        
        hour = mins_since_midnight // 60
        minute = mins_since_midnight % 60
        minute_10 = minute // 10
        minute_1 = minute % 10

        self.clear()

        # Midnight and noon
        if hour == 0 and minute < 5:
            self.set_state("자정")
            return
        if hour == 12 and minute < 5:
            self.set_state("정오")
            return

        # Hour
        hour = hour % 12  # Convert to 12-hour time
        self.set_state(HOUR_STATES[hour])

        # Minute
        if minute < 5:
            # On the hour, no minutes
            return
        if minute >= 15 and minute < 20:
            # Special arrangement at 15 past
            self.set_state("십오분")
            return
        if minute_10 >= 1:
            self.set_state(MINUTE_STATES[minute_10])
        if minute_1 >= 5:
            self.set_state("오")

    def demo(self):
        """
        Loop through all times of day to test LED functionality.

        :param leds: `Neopixel` object.
        """
        for mins_since_midnight in range(1440):
            self.update_time(mins_since_midnight)
            self.show()
            time.sleep_ms(100)
    
    def effect_white(self, arr):
        arr_rgbw = [[(0, 0, 0, 0) for _ in row] for row in arr]
        for row, columns in enumerate(arr):
            for col, on in enumerate(columns):
                if not on: continue
                arr_rgbw[row][col] = (0, 0, 0, 255)
        return arr_rgbw
    
    def effect_rainbow(self, arr):
        arr_rgbw = [[(0, 0, 0, 0) for _ in row] for row in arr]
        nrow = len(arr)
        ncol = len(arr[0])
        
        #period = 11483  # randomly selected prime number
        period = 127
        t = time.time() % period
        wave_r = 0.5 * sin((2 * pi / period) * (t - (0 * period / 3))) + 0.5
        wave_g = 0.5 * sin((2 * pi / period) * (t - (1 * period / 3))) + 0.5
        wave_b = 0.5 * sin((2 * pi / period) * (t - (2 * period / 3))) + 0.5
        
        # period_rot = 8209  # randomly selected prime number
        period_rot = 31
        t_rot = time.time() % period_rot
        theta = (2 * pi) * (t_rot / period_rot)
        
        for row, columns in enumerate(arr):
            for col, on in enumerate(columns):
                if not on: continue
                x = col / ncol * cos(theta) - row / nrow * sin(theta) + 1
                y = col / ncol * sin(theta) + row / nrow * cos(theta) + 1
                rgb = rgb_full_value(
                    wave_r * x + (1-wave_r) * y,
                    wave_g * x + (1-wave_g) * y,
                    wave_b * x + (1-wave_b) * y
                )
                rgbw = (rgb[0], rgb[1], rgb[2], 0)
                arr_rgbw[row][col] = rgbw
        return arr_rgbw


########
# MAIN #
########

if __name__ == "__main__":
    leds = Neopixel(NUM_LEDS, 0, PIN_LED, "GRBW")
    clockface = ClockFace(leds, NROW, NCOL)
    rtc = RV3028(pin_sda=PIN_SDA, pin_scl=PIN_SCL)
    sw_power = ButtonInput(PIN_SW_POWER, callback_power)
    sw_time = ButtonInput(PIN_SW_TIME)
    sw_brightness = ButtonInput(PIN_SW_BRIGHTNESS)
    sw_mode = ButtonInput(PIN_SW_MODE)

    while True:
        clockface.demo()
        
        
        # Power
        if not sw_power.update():
            leds.clear()
            leds.show()
            time.sleep_ms(10)
            continue
        
        # Update buttons
        sw_time.update()
        sw_brightness.update()
        sw_mode.update()
        
        if sw_time.time_pressed() > FAST_FORWARD_MS:
            print("Time")
        
        if sw_brightness.time_pressed() > FAST_FORWARD_MS:
            print("Brightness")
        
        if sw_mode.time_pressed() > FAST_FORWARD_MS:
            print("Mode")
        
        # Read system time
        rtc.read_to_systime()
        
        # Update display
        clockface.update_time()
        clockface.show()
        
        time.sleep_ms(10)
