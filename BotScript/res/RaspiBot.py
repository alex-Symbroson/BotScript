ISBOT = True

try:
    from raspibot import Display
except ImportError:
    ISBOT = False

objs = {}

def m_cleanup():
    if objs.has_key("Display"): objs.pop("Display").cleanup()

if ISBOT:
    def getDisplay():
        d = objs.get("Display")
        if d == None:
            d = Display()
            objs["Display"] = d
            d.init()
            d.clear()
            d.cursor_goto_xy(0,0)
        return d


    def m_write(s, x=None, y=None, *args):
        d = getDisplay()
        if y != None: d.cursor_goto_xy(int(x), int(y))
        d.write(s)

else:
    def m_write(s, x=None, y=None, *args): pass

methods = {
    "write": m_write,
    "cleanup": m_cleanup
}

def callMethod(method, *args):
    methods[method](*args)
