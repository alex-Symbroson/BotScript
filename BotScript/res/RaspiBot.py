
ISBOT = True

try:
    import warnings
    import raspibot, RPi.GPIO as GPIO
    from serial import Serial, PARITY_EVEN
    from smbus import SMBus
except ImportError as e:
    ISBOT = False
    warnings.warn("raspibot module couldnt be loaded:\n" + e.msg, Warning)

objs = {}
methods = {}

if ISBOT:

    def getObj(name, get):
        o = objs.get(name)
        if o == None: o = objs[name] = get()
        return o

    def m_cleanup():
        for obj in "Button1,Button2,Button3,Display".split(","):
            obj = objs.get(obj)
            if obj: obj.cleanup()

        GPIO.cleanup()
        return True

    def getDisplay():
        init = objs.get("Display") == None
        d = getObj("Display", lambda: raspibot.Display())
        if init:
            d.clear()
            d.init()
            d.cursor_goto_xy(0, 0)
        return d

    def getButton(i):
        return getObj("Button" + str(i), lambda:
            raspibot.Button(*[(13,26,23), (19,20,24), (16,21,25)][i - 1]))

    def getAttiny():
        return getObj("Attiny", lambda:
            raspibot.AttinyProtocol(Serial('/dev/ttyAMA0', 4800, 8, PARITY_EVEN, 2)))

    def getADC():
        return getObj("SMBus", lambda: raspibot.ADC(SMBus(1)))

    def m_write(s, x = None, y = None, *args):
        d = getDisplay()
        if y != None: d.cursor_goto_xy(int(x), int(y))
        d.write(s)

    methods = {
        "write": m_write,
        "cleanup": m_cleanup,
        # Serial (Attiny)
        "getEncoders": lambda : getAttiny().get_encoders(),
        "resetEncoders": lambda : getAttiny().reset_encoders(),
        "stopMotors": lambda : getAttiny().stop_motors(),
        "setMotors": lambda l, r: getAttiny().set_motors(l, r), # left, right
        "setBuzzer": lambda f, d, v: getAttiny().set_buzzer(f, d, v), # frequency, duration, volume
        "stopBuzzer": lambda : getAttiny().stop_buzzer(),
        # ADC
        "getSharp": lambda i: getADC().read_channel(i - 1), # 1 - 3
        "getBattery": lambda i: getADC().read_channel(4),
        # Buttons
        "setRedLED": lambda i, v: getButton(i).setRedLED(int(v)),
        "setGreenLED": lambda i, v: getButton(i).setGreenLED(int(v)),
        "waitForBtnPress": lambda i: getButton(i).waitForButtonPress(),
        "waitForBtnRelease": lambda i: getButton(i).waitForButtonRelease(),
        "waitForBtn": lambda i: getButton(i).waitForButton(),
        "isBtnPressed": lambda i: getButton(i).isPressed(),
        "isBot": lambda : True
    }
else:
    methods = {
        "cleanup": lambda : True,
        "isBot": lambda : False
    }


def callMethod(method, *args):
    foo = methods.get(method)
    # print((method + " " * 15)[:15], args)
    assert foo != None
    return foo(*args)
