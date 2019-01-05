
from RaspiBot import Methods

class Btn:

    def __init__(self, red, green, next = None):
        self.red   = red
        self.green = green
        self.next  = next

class State:

    def __init__(self, title, param, btns, func):
        self.title = title
        self.param = param
        self.func  = func
        self.btns  = btns

    def run(self):
        Methods.clearLCD()
        Methods.writeLCD(state.title, 0, 0)
        Methods.writeLCD(state.param, 0, 1)

        for i in range(len(state.btns)):
            methods.setRedLED(i, state.btns[i].red)
            methods.setGreenLED(i, state.btns[i].green)

        self.func(self)

class StateMachine:
    state = {}
    states = 1

    getStates = range

    def setState(self, id, *args):
        self.state[id] = State(*args)
        self.states += 1
        return self.states

    def run(self, id):
        while id: id = self.state[id].run()


def select(state):

    while True:
        if Methods.isButtonPressed(1): return btns[0].next
        if Methods.isButtonPressed(2): return btns[1].next
        if Methods.isButtonPressed(3): return btns[2].next
        sleep(0.1)

def cal_sharps():
    i = 0
    sharp = 1
    while True:
        if Methods.isButtonPressed(1):
            sharp = 1
            Methods.writeLCD("left ", 5, 0)
        if Methods.isButtonPressed(2):
            sharp = 2
            Methods.writeLCD("right", 5, 0)
        if Methods.isButtonPressed(3): return btns[2].next

        if i % 10 == 0:
            Methods.writeLCD("%i" % Methods.getSharp(1, "raw"), 12, 0)

        sleep(0.1)
        i += 1

def cal_radenc():
    time = 1
    while True:
        if Methods.isButtonPressed(1):
            time = (time + 1) % 10
            Methods.writeLCD("%i" % time, 5, 1)
        if Methods.isButtonPressed(2):
            Methods.writeLCD("l: ---- r: ---- ", 0, 0)
            Methods.resetEncoders()
            Methods.setMotors(50, 50)
            sleep(time)
            Methods.stopMotors()
            Methods.writeLCD("l:%5i r:%5i" % Methods.getEncoders(1, "raw"), 0, 0)

        if Methods.isButtonPressed(3): return btns[2].next

        sleep(0.1)

Calibrate = StateMachine()

# S:status C:calibrate
(S_EXIT, S_START, S_C_IRS, S_C_RNC
) = Calibrate.getStates(4)

Calibrate.setState(S_START, "calibration tool", "sharp radenc   x",
    [Btn(0, 100, S_C_IRS), Btn(0, 100, S_C_RNC), Btn(100, 0, S_EXIT)], select
)

Calibrate.setState(S_C_IRS, "      sharp",     "left right     x",
    [Btn(0, 100), Btn(0, 100), Btn(100, 0, S_START)], cal_sharps
)

Calibrate.setState(S_C_RNC, "l:      r:      ",  "time 1s start x",
    [Btn(100, 100), Btn(0, 100), Btn(100, 0)], cal_radenc
)

Calibrate.setState(S_EXIT, "", "",
    [Btn(0, 0), Btn(0, 0), Btn(0, 0)], lambda : False
)

Calibrate.run(S_START)
