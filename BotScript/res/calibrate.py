
from RaspiBot import Methods, sleep, objs

class Btn:

    def __init__(self, red, green, nextid = None):
        print(nextid)
        self.red   = red
        self.green = green
        self.next  = nextid

    def __str__(self): return "Btn(%i,%i,%i)"%(self.red,self.green,self.next)

class State:

    def __init__(self, title, param, btns, func):
        self.title = title
        self.param = param
        self.func  = func
        self.btns  = btns

    def run(self):
        Methods.clearLCD()
        Methods.writeLCD(self.title, 0, 0)
        Methods.writeLCD(self.param, 0, 1)

        for i in range(len(self.btns)):
            Methods.setRedLED(i + 1, self.btns[i].red)
            Methods.setGreenLED(i + 1, self.btns[i].green)

        return self.func(self)

class StateMachine:
    state = {}
    states = 1

    getStates = range

    def setState(self, id, *StateArgs):
        self.state[id] = State(*StateArgs)
        self.states += 1
        return self.states

    def run(self, id):
        while id != None: id = self.state[id].run()


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

def cal_sharps(state):
    i = 0
    sharp = 1
    
    while True:
        
        if Methods.isBtnPressed(1):
            sharp = 1
            Methods.writeLCD("left ", 5, 0)
            Methods.waitForBtnRelease(1)
            
        if Methods.isBtnPressed(2):
            sharp = 2
            Methods.writeLCD("right", 5, 0)
            Methods.waitForBtnRelease(2)
            
        if Methods.isBtnPressed(3):  
            Methods.waitForBtnRelease(3)
            return state.btns[2].next

        if i % 8 == 0:
            Methods.writeLCD("%i" % Methods.getSharp(sharp, "raw"), 12, 0)

        sleep(0.1)
        i += 1

def cal_radenc(state):
    time = 1
    
    while True:
        
        if Methods.isBtnPressed(1):
            time = (time + 1) % 10
            Methods.writeLCD("%i" % time, 5, 1)
            Methods.waitForBtnRelease(1)
            
        if Methods.isBtnPressed(2):
            Methods.writeLCD("l: ---- r: ---- ", 0, 0)
            Methods.waitForBtnRelease(2)
            
            Methods.resetEncoders()
            Methods.setMotors(50, 50)
            sleep(time)
            
            Methods.stopMotors()
            Methods.writeLCD("l:%5i r:%5i" % tuple(Methods.getEncoders("raw")), 0, 0)

        if Methods.isBtnPressed(3):  
            Methods.waitForBtnRelease(3)
            return state.btns[2].next

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
    [Btn(100, 100), Btn(0, 100), Btn(100, 0, S_START)], cal_radenc
)

Calibrate.setState(S_EXIT, "", "",
    [Btn(0, 0), Btn(0, 0), Btn(0, 0)], lambda _: Methods.cleanup()
)

Calibrate.run(S_START)
