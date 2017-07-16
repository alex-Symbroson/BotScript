
//c++ -std=c++11 -O3 -lwiringPi -o pins.out pins.cpp;./pins.out

#include <wiringPi.h> //-
#include "extern.h"

#ifndef _PINS_CPP_
#define _PINS_CPP_

#ifndef MAIN
	#define MAIN 5
	int main();
#endif

extern "C" {
	//<softPwm.h>
	extern int  softPwmCreate (int pin, int value, int range);
	extern void softPwmWrite  (int pin, int value);
	extern void softPwmStop   (int pin);
}

void setup() {
	if(wiringPiSetup() == - 1) error("setup wiringPi failed !");
}


#if MAIN == 5

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

#endif //MAIN == 5

#endif //_PINS_CPP_
