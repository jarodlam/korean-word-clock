from machine import Pin
import time

DEBOUNCE_MS = 50

class ButtonInput:
    
    def __init__(
            self,
            pin_no,
            callback=None,
            ff_threshold_ms=None,
            ff_interval_ms=200
        ):
        """
        Simple polling button controller.
        
        :param int pin_no: GPIO pin the button is connected to.
        :param function callback: Callback function to run on button press.
        :param int ff_threshold_ms: How long to wait before fast-forwarding.
        :param int ff_interval_ms: How often to trigger fast-forwarding.
        """
        self.pin = Pin(pin_no, Pin.IN)
        self.callback = callback
        self.ff_threshold_ms = ff_threshold_ms
        self.ff_interval_ms = ff_interval_ms
        
        # Value from the most recent button update
        self.previous_value = self.pin.value()
        
        # Time when previous_value was read
        self.previous_ticks = time.ticks_ms()
        
        # Time when previous_value last changed
        self.toggle_ticks = time.ticks_ms()
        
    def handle_fast_forward(self, value):
        # No callback configured and/or no fast forward configured
        if self.callback is None or self.ff_threshold_ms is None:
            return
        
        # Button not pressed
        if not value:
            self.previous_ff = -1
            return
        
        time_pressed_ff = self.time_pressed() - self.ff_threshold_ms
        
        # Time threshold not reached
        if time_pressed_ff < 0:
            return
        
        time_round = time_pressed_ff - (time_pressed_ff % self.ff_interval_ms)
        
        # Already triggered this fast-forward interval
        if time_round == self.previous_ff:
            return
        
        # Trigger the callback
        self.callback(value)
        self.previous_ff = time_round
        
    def update(self):
        ticks = time.ticks_ms()
        value = self.pin.value()
        
        # No change
        if value == self.previous_value:
            self.previous_ticks = ticks
            self.handle_fast_forward(value)
            return value
        
        # Debouncing, wait before read
        if (ticks - self.previous_ticks) < DEBOUNCE_MS:
            return self.previous_value
        
        # Value changed
        self.previous_ticks = ticks
        self.toggle_ticks = ticks
        self.previous_value = value
        
        # Trigger callback on change
        if self.callback is not None:
            self.callback(value)
        
        return value
        
    def time_pressed(self):
        if not self.previous_value:
            return 0
        return time.ticks_ms() - self.toggle_ticks