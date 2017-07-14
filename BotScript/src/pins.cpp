
//c++ -std=c++11 -O3 -lwiringPi -o pins.out pins.cpp;./pins.out

#ifndef MAIN
	#define MAIN 3
	int main();
#endif


#include<wiringPi.h>//- 
#include "extern.h"

extern "C" {
	//<softPwm.h>
	extern intsoftPwmCreate (int pin, int value, int range);
	extern void softPwmWrite(int pin, int value);
	extern void softPwmStop (int pin);
}

void setup() {
	if(wiringPiSetup() == - 1) error("setup wiringPi failed !");
}


#if MAIN == 3

int main() {
	setup();

	int ledRed = 0;
	softPwmCreate(ledRed,0, 255);

	for(int i = 0;i<3;i++) {
		softPwmWrite(ledRed, 255);
		delay(1000);
		softPwmWrite(ledRed, 0);
		delay(1000);
	}

	softPwmStop(ledRed);

	return 0;
}

#endif
