
#ifndef _BS_EXT_HPP_
#define _BS_EXT_HPP_

	#include <stdio.h>

	#define DEBUG 0
	#define TEST_BUILTINS 0

		//macros for error mesages
	#define err_ict(a,b) printf("\033[1;31mincompatible types %s and %s\033[0;37m\n", a, b)
	#define err_iop(a,b) printf("\033[1;31minvalid operator %s for %s\033[0;37m\n", a, b)
	#define err_imu(a,b) printf("\033[1;31m%ss have no '%s' member\033[0;37m\n", a, b)
	#define error(a,...) printf("\033[1;31m" a "\033[0;37m\n", ##__VA_ARGS__)

	#define wait_enter() stdin = freopen(NULL, "r", stdin); getchar();

		//yellow coloured debug macro
	#if DEBUG
	#define info(s, ...) printf("\033[0;36m" s "\033[0;37m\n", ##__VA_ARGS__)
	#define debug(s, ...) printf("\033[2;33m" s "\033[0;37m\n", ##__VA_ARGS__)
	#else
	#define debug(s, ...)
	#define info(s, ...)
	#endif
#endif
