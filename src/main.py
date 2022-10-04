from machine import Pin, RTC
import time
from math import sin, cos, pi, floor, ceil

from buttoninput import ButtonInput
from neopixel import Neopixel
from rv3028 import RV3028
from schedule import SCHEDULE

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
    "일곱시": [(1,3), (1,4), (2,4)],
    "여덟시": [(2,0), (2,1), (2,4)],
    "아홉시": [(2,2), (2,3), (2,4)],
    "열시": [(0,0), (2,4)],
    "열한시": [(0,0), (0,1), (2,4)],
    "열두시": [(0,0), (1,0), (2,4)],
    "자정": [(3,0), (3,1)],
    "정오": [(3,1), (4,1)],
    "오분": [(4,3), (4,4)],
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
PIN_SW_DST = 26
PIN_SW_TIME = 13
PIN_SW_BRIGHTNESS = 14
PIN_SW_MODE = 15
FAST_FORWARD_MS = 1000
RTC_EEADDR_EFFECT = 0x00
RTC_EEADDR_BRIGHTNESS = 0x01


#########################
# CLASSES AND FUNCTIONS #
#########################

def callback_time_factory(rtc):
    def callback_time(value):
        if not value: return
        y, mo, d, wd, h, m, _, _ = rtc.read_datetime()
        
        # Round down to nearest 5 minutes and add 5 minutes
        m = m - (m % 5) + 5
        
        # Roll over to next hour
        if m >= 60:
            m = m % 60
            h = (h + 1) % 24
        
        rtc.set_datetime((y, mo, d, wd, h, m, 0, 0))
    return callback_time


def callback_brightness_factory(clockface, rtc):
    def callback_brightness(value):
        if not value: return
        brightness = clockface.cycle_brightness()
        rtc.eeprom_write(RTC_EEADDR_BRIGHTNESS, brightness)
    return callback_brightness


def callback_mode_factory(clockface, rtc):
    def callback_mode(value):
        if not value: return
        effect_idx = clockface.cycle_effect()
        rtc.eeprom_write(RTC_EEADDR_EFFECT, effect_idx)
    return callback_mode


def rgb_to_hsv(r, g, b):
    """
    Convert an RGB colour to HSV.
    From https://en.wikipedia.org/wiki/HSL_and_HSV#Color_conversion_formulae.
    """
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
    h = h * 60
    s = 0
    if v != 0:
        s = c / v
    return h, s, v


def hsv_to_rgb(h, s, v):
    """
    Convert an HSV colour to RGB.
    From https://en.wikipedia.org/wiki/HSL_and_HSV#Color_conversion_formulae
    """
    h = h / 60
    c = v * s
    x = c * (1 - abs((h % 2) - 1))
    if h < 1: rgb = (c, x, 0)
    elif h < 2: rgb = (x, c, 0)
    elif h < 3: rgb = (0, c, x)
    elif h < 4: rgb = (0, x, c)
    elif h < 5: rgb = (x, 0, c)
    else: rgb = (c, 0, x)
    m = v - c
    rgb = (rgb[0] + m) * 255, (rgb[1] + m) * 255, (rgb[2] + m) * 255
    return round(rgb[0]), round(rgb[1]), round(rgb[2])


def rgb_full_value(r, g, b):
    """
    Convert an RGB colour to HSV, set V to 100%, then convert back.
    From https://en.wikipedia.org/wiki/HSL_and_HSV#Color_conversion_formulae
    """
    # Convert to HSV
    h, s, v = rgb_to_hsv(r, g, b)
    
    # Set v to 100%
    v = 1
    s = min(s * 2, 1)
    
    # Convert back to RGB
    return hsv_to_rgb(h, s, v)


def get_effect_from_rtc(rtc):
    """
    Get the persistent stored effect index from the external RTC EEPROM.
    """
    return rtc.eeprom_read(0x00)[0]


def set_effect_to_rtc(rtc, effect_idx):
    """
    Set the persistent stored effect index to the external RTC EEPROM.
    """
    rtc.eeprom_write(0x00, effect_idx)
    

class ClockFace:
    
    def __init__(self, leds, nrow, ncol, effect_idx=0, brightness=255):
        self.leds = leds
        self.nrow = nrow
        self.ncol = ncol
        self.effect_idx = effect_idx
        self.leds.brightness(brightness)
        
        self.internal_rtc = RTC()
        self.effects = [self.effect_white, self.effect_rainbow]
        if self.effect_idx > len(self.effects): self.effect_idx = 0
        self.prev_time = 0
        
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
        """
        Apply effects then write to LEDs.
        """
        # Only update once per second
        if time.time() == self.prev_time: return
        self.prev_time = time.time()
        
        effect_function = self.effects[self.effect_idx]
        
        # Check schedule if any apply
        curr_dtt = self.internal_rtc.datetime()[0:7]  # remove subsecond
        for schedule_dtt, effect_func_str in SCHEDULE:
            # Match this schedule entry against the current time
            match = True
            for curr_val, sched_val in zip(curr_dtt, schedule_dtt):
                # None is a wildcard, always matches
                if sched_val is None: continue
                if curr_val != sched_val:
                    match = False
                    break
            if match:
                # Get the effect function object from the schedule string
                effect_function = eval(
                    'self.' + effect_func_str,
                    {'self': self}
                )
        
        # Run the effect function to get colours
        arr_rgbw = effect_function(self.arr)
        
        # Write RGBW values to the LED strip
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
            self.set_state("오분")
    
    def cycle_brightness(self):
        b = self.leds.brightness()
        b = b + 50
        if b > 255: b = 55
        self.leds.brightness(b)
        self.prev_time = 0  # Force update
        return b
    
    def cycle_effect(self):
        self.effect_idx = self.effect_idx + 1
        if self.effect_idx >= len(self.effects):
            self.effect_idx = 0
        self.prev_time = 0  # Force update
        return self.effect_idx

    def demo(self):
        """
        Loop through all times of day to test LED functionality.

        :param leds: `Neopixel` object.
        """
        for mins_since_midnight in range(1440):
            self.update_time(mins_since_midnight)
            self.show()
            time.sleep_ms(100)
    
    def effect_hour_min(self, arr, rgbw_hour, rgbw_min):
        """
        Apply one colour to hour LEDs, another colour to minute LEDs.
        """
        arr_rgbw = [[(0, 0, 0, 0) for _ in row] for row in arr]
        hm_pos = [
            ['h', 'h', 'h', 'h', 'h'], 
            ['h', 'h', 'h', 'h', 'h'], 
            ['h', 'h', 'h', 'h', 'h'],
            ['h', 'h', 'm', 'm', 'm'],
            ['m', 'm', 'm', 'm', 'm']
        ]
        
        for row, columns in enumerate(arr):
            for col, on in enumerate(columns):
                if not on: continue
                
                rgbw = (0, 0, 0, 255)
                if hm_pos[row][col] == 'h': rgbw = rgbw_hour
                elif hm_pos[row][col] == 'm': rgbw = rgbw_min
                
                arr_rgbw[row][col] = rgbw
        return arr_rgbw
    
    def effect_white(self, arr):
        arr_rgbw = [[(0, 0, 0, 0) for _ in row] for row in arr]
        for row, columns in enumerate(arr):
            for col, on in enumerate(columns):
                if not on: continue
                arr_rgbw[row][col] = (255, 255, 255, 255)
        return arr_rgbw
    
    def effect_rainbow(self, arr):
        arr_rgbw = [[(0, 0, 0, 0) for _ in row] for row in arr]
        
        period = 1_000
        t = time.time() % period
        
        hue_h = (t / period * 360) % 360
        hue_m = (120 + t / period * 360) % 360
        
        rgb_h = hsv_to_rgb(hue_h, 1, 1) + (0,)
        rgb_m = hsv_to_rgb(hue_m, 1, 1) + (0,)
        
        return self.effect_hour_min(arr, rgb_h, rgb_m)
    
    def effect_birthday(self, arr):
        """
        A rainbow cycling effect inspired by party balloons.
        """
        arr_rgbw = [[(0, 0, 0, 0) for _ in row] for row in arr]
        
        # Colours to cycle
        colours = [
             (255, 0, 0, 0),    # Red
             (0, 255, 255, 0),  # Cyan
             (128, 0, 255, 0),  # Purple
             (255, 255, 0, 0),  # Yellow
             (0, 0, 255, 0),    # Blue
             (255, 128, 0, 0),  # Orange
             (255, 0, 255, 0),  # Magenta
             (0, 255, 0, 0)     # Green
        ]
        
        # Cycle colours every 2 seconds
        t = (time.time() // 2) % len(colours)
        
        for row, columns in enumerate(arr):
            # Select a colour based on row number and t, colours ascending
            rgbw = colours[(row + t) % len(colours)]
            for col, on in enumerate(columns):
                if not on: continue
                arr_rgbw[row][col] = rgbw
        return arr_rgbw
    
    def effect_love(self, arr):
        """
        A colour effect that cycles between love-themed colours.
        """
        # Colours to cycle
        colours = [
            (255, 0, 0, 0),    # Red
            (255, 128, 0, 0),  # Orange
            (255, 0, 255, 0)   # Magenta
        ]
        
        # t is a float on interval [0, len(colours))
        period = 120
        t = (time.time() % period) / period * len(colours)
        
        # Colours for this cycle
        def colour_from_t(t):
            colour1 = colours[floor(t)]
            colour2 = colours[ceil(t) % len(colours)]
            alpha = t % 1
            return (
                colour1[0] + (colour2[0] - colour1[0]) * alpha,
                colour1[1] + (colour2[1] - colour1[1]) * alpha,
                colour1[2] + (colour2[2] - colour1[2]) * alpha,
                colour1[3] + (colour2[3] - colour1[3]) * alpha
            )
        rgbw_h = colour_from_t(t)
        rgbw_m = colour_from_t((t + 1) % len(colours))  # offset by 1
        
        return self.effect_hour_min(arr, rgbw_h, rgbw_m)
    
    def effect_christmas(self, arr):
        return self.effect_hour_min(arr, (255, 0, 0, 0), (0, 255, 0, 0))
    
    def effect_celebration(self, arr):
        h = (time.time() * 32) % 360
        return self.effect_hour_min(
            arr,
            hsv_to_rgb(h, 1, 1) + (0,),
            (255, 255, 255, 255)
        )

            
########
# MAIN #
########

if __name__ == "__main__":
    # Set up objects
    rtc = RV3028(pin_sda=PIN_SDA, pin_scl=PIN_SCL)
    clockface = ClockFace(
        Neopixel(NUM_LEDS, 0, PIN_LED, "GRBW"),
        NROW,
        NCOL,
        effect_idx=rtc.eeprom_read(RTC_EEADDR_EFFECT)[0],
        brightness=rtc.eeprom_read(RTC_EEADDR_BRIGHTNESS)[0]
    )
    sw_dst = ButtonInput(
        PIN_SW_DST
    )
    sw_time = ButtonInput(
        PIN_SW_TIME,
        callback_time_factory(rtc),
        FAST_FORWARD_MS,
        100
    )
    sw_brightness = ButtonInput(
        PIN_SW_BRIGHTNESS,
        callback_brightness_factory(clockface, rtc),
        FAST_FORWARD_MS,
        500
    )
    sw_mode = ButtonInput(
        PIN_SW_MODE,
        callback_mode_factory(clockface, rtc)
    )

    while True:
        # Uncomment to put in demo mode
        # clockface.demo()
        
        # Update buttons
        dst = sw_dst.update()
        sw_time.update()
        sw_brightness.update()
        sw_mode.update()
        
        # Update system time from external RTC
        rtc.read_to_systime(dst=dst)
        
        # Update display
        clockface.update_time()
        clockface.show()
        
        time.sleep_ms(10)
