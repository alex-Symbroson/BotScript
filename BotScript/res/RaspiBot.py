#!/usr/bin/python2.7

# initialized correctly
ISBOT = True
# object containing components
objs = {}

def warn(msg, type):
    warnings.warn(msg, type)


try:
    import warnings, raspibot, RPi.GPIO as GPIO
    from serial import Serial, PARITY_EVEN
    from smbus import SMBus
    from time import sleep

    # create components
    objs = {
        "Button1": raspibot.Button(13, 26, 23),
        "Button2": raspibot.Button(19, 20, 24),
        "Button3": raspibot.Button(16, 21, 25),
        "Display": raspibot.Display(),
        "ADC":   raspibot.ADC(SMBus(1)),
        "Attiny":  raspibot.AttinyProtocol(Serial('/dev/ttyAMA0', 4800, 8, PARITY_EVEN, 2))
    }

    # initialize components
    obj = objs["Display"]
    # obj.clear()
    obj.init()
    obj.cursor_goto_xy(0, 0)

except ImportError as e:
    ISBOT = False
    warn("\n\033[1;33mraspibot module couldnt be loaded:\033[0;33m\n" + str(e) + "\033[0;37m", Warning)


class BaseMethods:
    def cleanup(_):
        for key in objs:
            if hasattr(objs[key], "cleanup"): objs[key].cleanup()

    def isBot(_): return ISBOT

    # remove values with greatest difference and calc arithmetic mean
    def arith(_, values, remove=0):
        arm = sum(values) / len(values)
        # calc difference med <-> values
        diff = [abs(val - arm) for val in values]

        # remove elements with greatest difference
        for _ in range(remove):
            i = diff.index(max(diff))
            values.pop(i)
            diff.pop(i)
        return sum(values) / len(values)

    def getSensor(_, foo):
            return self.arith([foo() for _ in range(7)][1:], 2)


if ISBOT:

    class _Methods(BaseMethods):

    # Display
        # writeLCD(test, x, y) = ("text", [0 - 15], [0 - 1])
        def writeLCD(_, s, x = None, y = None):
            print("write %i %i %s"%(x,y,s))
            if x != None and y != None: objs["Display"].cursor_goto_xy(int(x), int(y))
            objs["Display"].write(s)

        clearLCD = objs["Display"].clear # ()

    # Serial (Attiny)
        # setMotors(left right) = ([0 - 100], [0 - 100])
        setMotors = objs["Attiny"].set_motors
        # setBuzzer(frequency, duration , volume   )
        #          ([0 - 2^16], [0 - 2^16], [0 - 100])
        setBuzzer = objs["Attiny"].set_buzzer
        resetEncoders = objs["Attiny"].reset_encoders # ()
        stopMotors = objs["Attiny"].stop_motors # ()
        stopBuzzer = objs["Attiny"].stop_buzzer # ()

        # getEncoders -> [left, right] = ([0 - 100], [0 - 100])
        def getEncoders(_, opt = None):
            vals = objs["Attiny"].get_encoders()
            if opt == "raw": return vals
            return vals/22.5

    # ADC
        def getSharp(self, i, opt = None): # i = ([1 - 2])
            # get arith from 4 best of 8 inputs
            d = self.getSensor(lambda : objs["ADC"].read_channel(3-i))
            if opt == "raw": return d

            # convert data into cm
            d /= 100
            #d = (-0.648*d + 7.543)*d*d - 33.0*d + 62.2
            #d = (-0.155*d + 3.300)*d*d - 24.5*d + 75.5
            d = (-0.0865*d + 2.000)*d*d - 17.0*d + 61.6
            return int(d * 1000) / 1000

        def getBattery(_): getSharp(0, "raw")

    # Buttons
        # (i, v) = ([1 - 3], [0 - 100])
        def setRedLED(_, i, v): objs["Button%i" % i].setRedLED(int(v))
        def setGreenLED(_, i, v): objs["Button%i" % i].setGreenLED(int(v))
        def waitForBtnPress(_, i): objs["Button%i" % i].waitForButtonPress()
        def waitForBtnRelease(_, i): objs["Button%i" % i].waitForButtonRelease()
        def waitForBtn(_, i): objs["Button%i" % i].waitForButton()
        def isBtnPressed(_, i): return objs["Button%i" % i].isPressed()
else:
    class _Methods(BaseMethods): pass

Methods = _Methods()

def callMethod(method, *args):
    # print((method + 15*" ")[:15], args)
    if hasattr(Methods, method):
        return getattr(Methods, method)(*args)
    else:
        error = AssertionError("\033[0;31mmethod '%s' not found\033[0;37m" % method)
        raise error
