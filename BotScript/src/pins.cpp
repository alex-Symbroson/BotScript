
//$ g++ -std=c++11 -O3 -c pins.cpp

#include "pins.hpp"

#define CUSTOM_BEGIN
#include "macros.hpp"

#include <wiringPi.h>
#include <softPwm.h>

#define BEGIN(...) BEGIN_1("Pins",__VA_ARGS__)
#define END(...) BEGIN_1("Pins",__VA_ARGS__)

/* softPwm.h
	extern int  softPwmCreate (int pin, int value, int range);
	extern void softPwmWrite  (int pin, int value);
	extern void softPwmStop   (int pin);
*/

void Pins::setup() {
	BEGIN();
	if(wiringPiSetup() == -1) error("setup wiringPi failed !");
	END();
}
