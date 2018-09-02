
def m_hello(*args):
    print("Hello World!")

def m_write(s, *args):
    print(s)

methods = {
    "hello": m_hello,
    "write": m_write
}

def callMethod(method, *args):
    methods[method](*args)
