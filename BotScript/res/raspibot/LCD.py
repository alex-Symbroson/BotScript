# -*- coding: utf-8 -*-

# LCD control for the RaspiBot v2
# basically, an implementation of the HD44780 controller protocol,
# made-to-measure for our 4-bit interface to a 16x2 character LCD

import RPi.GPIO as GPIO
from time import sleep


def _bits(byte):
    return [(0 if (byte & (1 << i)) == 0 else 1) for i in reversed(range(8))]


class Display:
    """Implements the communication protocol for the RaspiBot v2's LCD."""

    def __init__(self):
        """Create a display object with default GPIO pin numbers."""
        # pin numbers, defaults to BCM GPIO numbering
        self.enable = 27
        self.rw = 18
        self.register_select = 17

        self.d4 = 8
        self.d5 = 7
        self.d6 = 5
        self.d7 = 6

        # define data pins in descending order, so the bits of a nibble can be
        # written in descending order as well, i.e. 0b0011 as
        # GPIO.output(self.data, [0, 0, 1, 1])
        self.data = [self.d7, self.d6, self.d5, self.d4]

        self.init()

    def init(self):
        """Initialize the GPIO pins and ensure 4-bit communication mode."""
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(
            [self.enable, self.rw, self.register_select],
            GPIO.OUT,
            initial=GPIO.LOW)
        GPIO.setup(self.data, GPIO.OUT, initial=GPIO.LOW)

        # initialize to 4-bit mode regardless of initial state
        self._write_nibble([0, 0, 1, 1])
        self._write_nibble([0, 0, 1, 1])
        self._write_nibble([0, 0, 1, 1])
        self._write_nibble([0, 0, 1, 0])

        # initialize entry mode
        self._write_byte([0, 0, 0, 0, 0, 1, 1, 0])

        # initialize display control
        self._write_byte([0, 0, 0, 0, 1, 1, 1, 1])

        # initialize function mode
        self._write_byte([0, 0, 1, 0, 1, 0, 0, 0])

        self.clear()
        sleep(0.001)

    def wait_for_controller(self):
        GPIO.output(self.rw, 1)
        GPIO.output(self.register_select, 0)
        GPIO.setup(self.d7, GPIO.IN)
        while not GPIO.input(self.d7):
            a=1
            #make it nicer
        GPIO.setup(self.d7, GPIO.OUT)

    def _write_nibble(self, nibble):
        """Send four bits to the display controller."""
        GPIO.output(self.rw, 0)
        GPIO.output(self.data, nibble)
        GPIO.output(self.enable, 1)
        GPIO.output(self.enable, 0)

    def _write_byte(self, bits):
        """Send a whole byte to the display controller."""
        self._write_nibble(bits[0:4])
        self._write_nibble(bits[4:8])
        self.wait_for_controller()

    def _select_data_register(self):
        GPIO.output(self.register_select, 1)

    def _select_instruction_register(self):
        GPIO.output(self.register_select, 0)

    def clear(self):
        """
        Clear display and reset cursor.
        Clear the LCD of any content and reset the cursor to the top left
        character of the display.
        """
        GPIO.output(self.register_select, 0)
        self._write_byte([0, 0, 0, 0, 0, 0, 0, 1])

    def print_codepoint(self, bits):
        """Print a single character at the cursor position."""
        self._select_data_register()
        self._write_byte(bits)

    def cursor_goto_xy(self, x ,y ):
        self._select_instruction_register()
        # WARNING: only works as expected for line == 1 or line == 0
        y = 0 if y == 0 else 1
        self._write_byte([1, y, x&32, x&16, x&8, x&4, x&2, x&1])

    def load_custom_character(self, picture, number):
        if number>=0 and number<=7:
            self._write_byte([0, 1, number&4, number&2, number&1, 0, 0, 0])
            if len(picture)==8:
                for c in picture:
                    self._select_data_register()
                    self._write_byte(c)

    def cursor_off(self):
        GPIO.output(self.register_select, 0)
        self._write_byte([0, 0, 0, 0, 1, 1, 0, 0])

    def cursor_on(self):
        GPIO.output(self.register_select, 0)
        self._write_byte([0, 0, 0, 0, 1, 1, 1, 1])

    def write(self, string):
        """Print a string at the cursor position."""
        # ASCII encoding is probably the closest to the HDS44780's actual
        # encoding
        for c in string.encode('ascii'):
            self.print_codepoint(_bits(ord(c)))

    # TODO: figure out a better way to cleanup GPIO instead of letting the
    # user call this manually...
    def cleanup(self):
        """Free any used GPIO pins."""
        GPIO.cleanup(self.data + [self.enable, self.rw, self.register_select])
