
#ifndef _BS_EXT_H_
#define _BS_EXT_H_

	#include <time.h>	//clock
	#include <stdio.h>	//fopen, fclose, fgetc, printf, vprintf, sprintf, snprintf
	#include <stdarg.h>	//va_start
	#include <stdlib.h>	//exit, free
	#include <stdint.h>	//uintN_t types
	#include <iostream>	//cout, string
	#include <string>	//+ string non - members
	#include <vector>	//alternative array based list container
	#include <cmath>	//pow10

	using namespace std;

	namespace Error {

		void error(const char* format, ...) {
			va_list args; //argument list ...
			va_start(args, 0); //init args
			printf("\033[31;1m"); //style: bold red
			vprintf(format, args); //print formatted error msg
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

		//invalid member use
		void imu(const char* a, const char* b) {
			error("%ss have no '%s' member", a, b);
		}

	};
#endif
