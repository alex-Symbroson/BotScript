
# BotScript v1.00
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
- execute ```make [-j4] [ISPI=true] [ISBOT=true] [ISDBG=true]```
    - ```ISPI ```: includes pin access features
    - ```ISBOT```: includes python and therefore RaspiBot functions
    - ```ISDBG```: adds -g flag and nvwa memory leak detector

**External Dependencies:**
- wiringPi.h ( -> https://github.com/WiringPi/WiringPi )
