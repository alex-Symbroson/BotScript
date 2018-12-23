
ISBOT = True

try:
    import raspibot
except ImportError:
    ISBOT = False

objs = {}
methods = {}

if ISBOT:
    def m_cleanup():
        if objs.has_key("Display"): objs.pop("Display").cleanup()
        raspibot.Buttons.GPIO.cleanup()
        return True

    def getDisplay():
        d = objs.get("Display")
        if d == None:
            d = raspibot.Display()
            objs["Display"] = d
            d.clear()
            d.init()
            d.cursor_goto_xy(0,0)
        return d

    def getButton(i):
        b = objs.get("Button" + str(i))
        if b == None:
            b = raspibot.Button(*[(13,26,23),(19,20,24),(16,21,25)][i-1])
            objs["Button"+str(i)] = b;
        return b;

    def m_write(s, x=None, y=None, *args):
        d = getDisplay()
        if y != None: d.cursor_goto_xy(int(x), int(y))
        d.write(s)

    methods = {
        "write": m_write,
        "cleanup": m_cleanup,
        # Serial (Attiny)
        "getEncoders": raspibot.AttinyProtocol.get_encoders,
        "resetEncoders": raspibot.AttinyProtocol.reset_encoders,
        "stopMotors": raspibot.AttinyProtocol.stop_motors,
        "setMotors": raspibot.AttinyProtocol.set_motors,
        "setBuzzer": raspibot.AttinyProtocol.set_buzzer,
        "stopBuzzer": raspibot.AttinyProtocol.stop_buzzer,
        # Buttons
        "setRedLED": lambda i, v: getButton(i).setRedLED(v),
        "setGreenLED": lambda i, v: getButton(i).setGreenLED(v),
        "waitForBtnPress": lambda i: getButton(i).waitForButtonPress,
        "waitForBtnRelease": lambda i: getButton(i).waitForButtonRelease,
        "waitForBtn": lambda i: getButton(i).waitForButton,
        "isBtnPressed": lambda i: getButton(i).isPressed
    }
else:
    methods = {
        "cleanup": lambda: True
    }


def callMethod(method, *args):
    foo = methods.get(method);
    if foo != None: return foo(*args);
