# coding: utf-8
"""Provides a class for controlling the TI ADS105."""
from time import sleep
import RPi.GPIO as GPIO

# ACHTUNG: Das SMBus-Protokoll geht bei der Übertragung von Werten mit
# mehr als einem Byte Länge (16-bit-Word) davon aus, dass das LSB zuerst
# übertragen wird. Der ADS1015 sendet jedoch das MSB zuerst, daher ist
# eine Konvertierung notwendig


def _swap_bytes_16bit(value):
    return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8)


def _set_bits(config, mask, bits):
    # clear the bits before setting them
    config &= ~mask
    config |= (bits & mask)
    return config


class ADS1015:
    """
    Implements the I2C protocol for the TI ADS1015 chip.

    Provides methods for configuring and reading A/D conversion values via an
    I2C bus from the Texas Instruments ADS1015 chip.
    """

    alrt=22

    _CHANNELS = [0, 1, 2, 3]

    __CONFIG_REGISTER = 0b01
    __CONVERSION_REGISTER = 0b00

    # bits [15] start the conversion
    __CONFIG_CONVERSION_START = 1 << 15
    # bits [14:12] are for configuring the mux
    __CONFIG_MUX_BITS = 0b111 << 12
    # Bits zur Konfiguration des Muxers zum Messen von Spannungen relativ
    # zu Masse, für Eingänge A1 bis A4
    __CONFIG_MUX_ABSOLUTE = [
        0b100 << 12,
        0b101 << 12,
        0b110 << 12,
        0b111 << 12]

    # bits [11:9] configure the programmable gain amplifier
    __CONFIG_PGA_BITS = 0b111 << 9
    __CONFIG_PGA_4_VOLT = 0b001 << 9

    __CONFIG_MODE_BITS = 1 << 8
    __CONFIG_MODE_CONTINUOUS = 0 << 8
    __CONFIG_MODE_SINGLE     = 1 << 8

    def __init__(self, bus, bus_address=0x49):
        """Create a new object for communicating on a given bus and address."""
        self.__bus = bus
        self.__bus_address = bus_address

        config = self.read_config_register()

        config=_set_bits(config, self.__CONFIG_MUX_BITS, self.__CONFIG_MUX_ABSOLUTE[0])
        config=_set_bits(config, self.__CONFIG_PGA_BITS, self.__CONFIG_PGA_4_VOLT)
        config=_set_bits(config, self.__CONFIG_MODE_BITS, self.__CONFIG_MODE_SINGLE)

        self.write_config_register(config)

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.alrt, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    def bus(self):
        """Get the bus that is currently used for communication."""
        return self.__bus

    def bus_address(self):
        """Get the bus address of the chip used for communication."""
        return self.__bus_address

    def read_config_register(self):
        """Retrieve the two-byte configuration register from the ADC."""
        config = self.__bus.read_word_data(
            self.__bus_address, self.__CONFIG_REGISTER)
        return _swap_bytes_16bit(config)

    def write_config_register(self, configuration):
        """
        Set the 2-byte ADC configuration register.

        :type configuration: 16-bit word
        """
        self.__bus.write_word_data(
            self.__bus_address,
            self.__CONFIG_REGISTER,
            _swap_bytes_16bit(configuration))

    def read_conversion_value(self):
        """Read the contents of the conversion register on the ADC."""
        sensor_value = self.__bus.read_word_data(
            self.__bus_address, self.__CONVERSION_REGISTER)
        sensor_value = _swap_bytes_16bit(sensor_value)
        # die niederwertigsten 4 Bit sind immer auf 0 gesetzt, daher können
        # diese ignoriert werden (siehe Datenblatt, Registerbeschreibung
        # Tabelle 8 auf Seite 15)
        return sensor_value >> 4

    def set_mux_absolute(self, channel):
        """
        Configure the ADC to read absolute values on the given channel.

        Absolute voltage values are measured with reference to the ground
        potential.
        """
        if channel not in self._CHANNELS:
            return
        else:
            config = self.read_config_register()

            config=_set_bits(config,
                      self.__CONFIG_MUX_BITS,
                      self.__CONFIG_MUX_ABSOLUTE[channel])
            config=_set_bits(config, self.__CONFIG_CONVERSION_START, self.__CONFIG_CONVERSION_START)

            self.write_config_register(config)

    def read_channel(self, channel):
        """
        Read one value from the given ADC channel.

        :param channel: the index of the channel to be read - can be one of
        [0, 1, 2, 3]

        :returns the converted channel value as an integer
        """
        if channel not in self._CHANNELS:
            return
        else:
            self.set_mux_absolute(channel)
            # lässt dem ADC Zeit, eine neue Messung mit der neuen
            # Konfiguration stattfinden zu lassen
            while not GPIO.input(self.alrt):
                a=1
            return self.read_conversion_value()
