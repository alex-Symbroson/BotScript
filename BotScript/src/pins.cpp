
//$ g++ -std=c++11 -O3 -c pins.cpp

#include "pins.hpp"
#include "macros.hpp"

#include <wiringPi.h>
#include <softPwm.h>

/* softPwm.h
	extern int  softPwmCreate (int pin, int value, int range);
	extern void softPwmWrite  (int pin, int value);
	extern void softPwmStop   (int pin);
*/

void Pins::setup() {
	debug("Pins::setup()");
	if(wiringPiSetup() == -1) error("setup wiringPi failed !");
}
