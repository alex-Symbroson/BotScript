
# BotScript
Script language with optimized PIN access features

**Origin 3 - Stack:**
    - based on origin preset
    - creates a stack of commands to interpret the preformatted code

**planned features:**
    -> [Syntax.md](https://github.com/alex-Symbroson/BotScript/blob/stack/BotScript/Syntax.md)

**robot sensors**
- Encoder l/r
- Motor l/r
- IR front l/r
- Buttons (with rgy leds)
- Camera
- Display
- LEDs (rgyw)
- Buzzer
- (Ground sensors)

**Build:**
- switch directory to BotScript root directory
- default command is ```make [-j4]```
- optionally use ```make ISPI=true``` or ```make ISPI=false```
    depending on wether you are on a Raspberry pi or not
    (default is defined in [ispi](https://github.com/alex-Symbroson/BotScript/blob/stack/BotScript/ispi))

**External Dependencies:**
- wiringPi.h ( -> https://github.com/WiringPi/WiringPi )
