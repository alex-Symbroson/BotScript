
#ifndef _BS_EXT_HPP_
#define _BS_EXT_HPP_
/*
	#include <time.h>	//clock
	#include <stdio.h>	//fopen, fclose, fgetc, printf, vprintf, sprintf, snprintf
	#include <stdlib.h>	//exit, free
	#include <stdint.h>	//uintN_t types
	#include <iostream>	//cout, string
	#include <string>	//+ string non - members
	#include <vector>	//alternative array based list container
	#include <cmath>	//pow10
*/
	#include <stdio.h>

		//macros for error mesages
	#define err_ict(a,b) printf("\033[31;1mincompatible types %s and %s\033[0;37m\n", a, b)
	#define err_iop(a,b) printf("\033[31;1minvalid operator %s for %s\033[0;37m\n", a, b)
	#define err_imu(a,b) printf("\033[31;1m%ss have no '%s' member\033[0;37m\n", a, b)
	#define error(a,...) printf("\033[31;1m" a "\033[0;37m\n", ##__VA_ARGS__)

#endif
