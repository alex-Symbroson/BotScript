
#ifndef _BS_EXT_H_
#define _BS_EXT_H_

	#include <time.h>    //clock
	#include <stdio.h>   //fopen, fclose, fgetc, printf, vprintf, sprintf, snprintf
	#include <stdarg.h>  //va_start
	#include <stdlib.h>  //exit, free
	#include <stdint.h>  //uintN_t types
	#include <iostream>  //cout, string
	#include <string>    //+string non-members
	#include <vector>    //alternative array based list container
	#include <cmath>     //pow10

	using namespace std;

	#ifndef delay
			//delay in milliseonds
		void delay(uint32_t time) {
			time = clock() + time*1000;
			while(clock() < time);
		}
	#endif

		//convert string to double precision float (16 digits)
	double strtof(string s) {
		double ret(0);

			//"0." -> "0.0"
		if(*(s.end()-1) == '.') s += "0";

			//c: one digit before first digit, end = last digit
		string::reverse_iterator c(s.begin()), end(s.end());

			//find float position, +1 because then 0 -> int
		uint32_t f = s.find('.') + 1;

		if(!f) f = s.length(); //integer float position is last digit
		else f--;              //subtract previous added 1

			//start calculating from first digit
		while(--c >= end) {
			if(*c == '.' || *c == '-') --c; //jump over non-digits
			ret += pow10(--f) * (*c - 48);  //add digit*10^pos to result
		}

			//if first digit is '-' return negative, else positive result
		return *s.begin()=='-'?-ret:ret;
	}


	namespace Error {

		void error(const char* format, ...) {
			va_list args;           //argument list ...
			va_start(args,0);       //init args
			printf("\033[31;1m");   //style: bold red
			vprintf(format,args);   //print formatted error msg
			printf("\033[0;37m\n"); //style: normal white
			exit(0);
		}

		//incompatible type
		void ict(const char* a, const char* b) {
			error("incompatible types %s and %s", a, b);
		}

		//invalid operator
		void iop(const char* a, const char* b) {
			error("invalid operator %s for %s", b, a);
		}
	};

#endif
