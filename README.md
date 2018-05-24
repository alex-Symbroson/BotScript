
# BotScript
Script language with optimized PIN access features

**Origin 3 - Stack:**
    - based on origin preset
    - creates a stack of commands to interpret the preformatted code

**planned features:**
    -> Syntax.txt

**Build:**
- switch directory to BotScript root directory
- default command is ```make [-j4]```
- optionally use ```make ISPI=true``` or ```make ISPI=false```
    depending on wether you are on a Raspberry pi or not
    (default is defined in file ispi)

**Dependencies:**
- c:
    - math.h
    - time.h
    - stdio.h
    - softPwm.h  (part of wiringPi)
    - pthread.h
    - wiringPi.h    ( -> https://github.com/WiringPi/WiringPi )
- c++:
    - string
    - vector
    - iostream
    - forward_list
    - unordered_map
