
#ifndef _PINS_HPP_
#    define _PINS_HPP_

namespace Pins {
    void setup();
}

#endif //_PINS_CPP_

/*
#if MAIN == _PINS_CPP_

int main() {
    setup();

    int ledRed = 0;
    softPwmCreate(ledRed, 0, 255);

    for(int i = 0; i<3; i++) {
        softPwmWrite(ledRed, 255);
        delay(1000);
        softPwmWrite(ledRed, 0);
        delay(1000);
    }

    softPwmStop(ledRed);

    return 0;
}

#endif //MAIN == _PINS_CPP_
*/
