
from RaspiBot import Methods, sleep

# represents btn color and action on press in a state
class Btn:

    def __init__(self, red, green, nextid = None):
        self.red   = red
        self.green = green
        self.next  = nextid

# represents one menu state with message, button and own action
class State:

    def __init__(self, title, param, btns, func):
        self.title = title
        self.param = param
        self.func  = func
        self.btns  = btns

    def run(self):
        # write messages
        Methods.clearLCD()
        Methods.writeLCD(self.title, 0, 0)
        Methods.writeLCD(self.param, 0, 1)

        # set button colors
        for i in range(len(self.btns)):
            Methods.setRedLED(i + 1, self.btns[i].red)
            Methods.setGreenLED(i + 1, self.btns[i].green)

        # run action
        return self.func(self)

# represents whole menu
class StateMachine:
    state = {}
    states = 1

    # returns ids to create new states
    def getStates(num): return range(states, states + num)

    # define state of specific id
    def setState(self, id, *StateArgs):
        self.state[id] = State(*StateArgs)
        self.states += 1
        return self.states

    # run machine
    def run(self, id):
        while id != None: id = self.state[id].run()


# navigate through menu
def select(state):

    while True:

        if Methods.isBtnPressed(1):
            Methods.waitForBtnRelease(1)
            return state.btns[0].next

        if Methods.isBtnPressed(2):
            Methods.waitForBtnRelease(2)
            return state.btns[1].next

        if Methods.isBtnPressed(3):
            Methods.waitForBtnRelease(3)
            return state.btns[2].next

        sleep(0.1)

# measure sharp values
def cal_sharps(state):
    i = 0
    sharp = 1

    while True:

        # measure left sharp
        if Methods.isBtnPressed(1):
            sharp = 1
            Methods.writeLCD("left ", 5, 0)
            Methods.waitForBtnRelease(1)

        # measure right sharp
        if Methods.isBtnPressed(2):
            sharp = 2
            Methods.writeLCD("right", 5, 0)
            Methods.waitForBtnRelease(2)

        # exit
        if Methods.isBtnPressed(3):
            Methods.waitForBtnRelease(3)
            return state.btns[2].next

        if i % 8 == 0:
            Methods.writeLCD("%i" % Methods.getSharp(sharp, "raw"), 12, 0)

        sleep(0.1)
        i += 1

# measure radencoders after driven for specific time
def cal_radenc(state):
    time = 1

    while True:

        # increase time
        if Methods.isBtnPressed(1):
            time = (time + 1) % 10
            Methods.writeLCD("%i" % time, 5, 1)
            Methods.waitForBtnRelease(1)

        # start driving
        if Methods.isBtnPressed(2):
            # reset display
            Methods.writeLCD("l: ---- r: ---- ", 0, 0)
            Methods.waitForBtnRelease(2)

            # drive
            Methods.resetEncoders()
            Methods.setMotors(50, 50)
            sleep(time)

            # get encoder values
            Methods.stopMotors()
            Methods.writeLCD("l:%5i r:%5i" % tuple(Methods.getEncoders("raw")), 0, 0)

        # exit
        if Methods.isBtnPressed(3):
            Methods.waitForBtnRelease(3)
            return state.btns[2].next

        sleep(0.1)

# create new state machine
Calibrate = StateMachine()

# S:status C:calibrate
(S_EXIT, S_START, S_C_IRS, S_C_RNC
) = Calibrate.getStates(4)

# start menu
Calibrate.setState(S_START, "calibration tool", "sharp radenc   x",
    [Btn(0, 100, S_C_IRS), Btn(0, 100, S_C_RNC), Btn(100, 0, S_EXIT)], select
)

# calibrate sharps
Calibrate.setState(S_C_IRS, "      sharp",     "left right     x",
    [Btn(0, 100), Btn(0, 100), Btn(100, 0, S_START)], cal_sharps
)

# calibrate encoders
Calibrate.setState(S_C_RNC, "l:      r:      ",  "time 1s start x",
    [Btn(100, 100), Btn(0, 100), Btn(100, 0, S_START)], cal_radenc
)

# exit menu
Calibrate.setState(S_EXIT, "", "",
    [Btn(0, 0), Btn(0, 0), Btn(0, 0)], lambda _: Methods.cleanup()
)

# run machine at start
Calibrate.run(S_START)
