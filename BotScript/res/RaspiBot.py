#!/usr/bin/python2.7

# initialized correctly
ISBOT = True
# object containing components
objs = {}

def warn(msg, type):
    warnings.warn(msg, type)

def m_cleanup():
    for obj in objs.values:
        if hasattr(obj, "cleanup"): obj.cleanup()
    return True

# remove values with greatest difference and calc arithmetic mean
def arith(foo, vlen, remove):
    values = [foo() for i in range(vlen)]
    arm = sum(values) / vlen
    # calc difference med <-> values
    diff = [abs(val - arm) for val in values]
    # remove elements with greatest difference
    for i in range(remove): values.pop(diff.index(max(diff)))
    return sum(values) / vlen


try:
    import warnings, raspibot, RPi.GPIO as GPIO
    from serial import Serial, PARITY_EVEN
    from smbus import SMBus

    # create components
    objs = {
        "GPIO": GPIO,
        "Button1": raspibot.button(13, 26, 23),
        "Button2": raspibot.button(19, 20, 24),
        "Button3": raspibot.button(16, 21, 25),
        "Display": raspibot.Display(),
        "SMBus":   raspibot.ADC(SMBus(1)),
        "Attiny":  raspibot.AttinyProtocol(Serial('/dev/ttyAMA0', 4800, 8, PARITY_EVEN, 2))
    }

    # initialize components
    obj = objs["Display"]
    obj.clear()
    obj.init()
    obj.cursor_goto_xy(0, 0)

except ImportError as e:
    ISBOT = False
    warn("\033[0;33mraspibot module couldnt be loaded:\n" + str(e) + "\033[0;37m", Warning)


if ISBOT:

    def m_write(s, x = None, y = None, *args):
        if y != None: objs["Display"].cursor_goto_xy(int(x), int(y))
        objs["Display"].write(s)

    methods = {
        "cleanup": m_cleanup,
        "write": m_write,
        # Serial (Attiny)
        "setMotors":    lambda l, r: objs["Attiny"].set_motors(l, r), # left, right
        "setBuzzer":    lambda f, d, v: objs["Attiny"].set_buzzer(f, d, v), # frequency, duration, volume
        "getEncoders":  lambda : objs["Attiny"].get_encoders(),
        "resetEncoders":lambda : objs["Attiny"].reset_encoders(),
        "stopMotors":   lambda : objs["Attiny"].stop_motors(),
        "stopBuzzer":   lambda : objs["Attiny"].stop_buzzer(),
        # ADC
        "getSharp":     arith(lambda i: objs["ADC"].read_channel(i), 8, 4), # 1 - 2
        "getBattery":   lambda i: objs["ADC"].read_channel(4),
        # Buttons
        "setRedLED":    lambda  i, v: objs["Button%i" % i].setRedLED(int(v)),
        "setGreenLED":  lambda  i, v: objs["Button%i" % i].setGreenLED(int(v)),
        "waitForBtnPress":  lambda i: objs["Button%i" % i].waitForButtonPress(),
        "waitForBtnRelease":lambda i: objs["Button%i" % i].waitForButtonRelease(),
        "waitForBtn":       lambda i: objs["Button%i" % i].waitForButton(),
        "isBtnPressed":     lambda i: objs["Button%i" % i].isPressed(),
        "isBot":            lambda  : True
    }
else:
    methods = {
        "cleanup": lambda : True,
        "isBot": lambda : False
    }


def callMethod(method, *args):
    foo = methods.get(method)
    # print((method + " " * 15)[:15], args)
    if foo == None:
        error = AssertionError("\033[0;31mmethod '%s' not found\033[0;37m" % method)
        raise error
    return foo(*args)
