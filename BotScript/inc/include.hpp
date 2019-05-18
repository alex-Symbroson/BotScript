
#ifndef _BSINC_HPP_
#define _BSINC_HPP_

#include "headers.hpp"

#define isAlpha(c) (isalpha(c) || c == '_')
#define isAlnum(c) (isalnum(c) || c == '_')
#define isPunct(c) (ispunct(c) && c != '_')

inline bool isOperator(char c) {
    return strchr("=.+-|<>&!*/%^~", c);
}

// replace all in string
string replace(string str, string src, string ovr);


// set start time
void rtime_init();

// elapsed time since epoch
clock_t rtime(uint f_s, uint f_ns);
#define rtime_s() rtime(1, 1000000000)
#define rtime_ms() rtime(1000, 1000000)
#define rtime_us() rtime(1000000, 1000)
#define rtime_ns() rtime(1000000000, 1)

// elapsed time since start
clock_t rclock(uint f_s, uint f_ns);
#define rclock_s() rclock(1, 1000000000)
#define rclock_ms() rclock(1000, 1000000)
#define rclock_us() rclock(1000000, 1000)
#define rclock_ns() rclock(1000000000, 1)

// system delay in milliseonds
void rsys_delay_ms(long double time);


// convert number string with radix (RxN) to double
long double stod2(const char* s);
inline long double stod2(string& s) {
    return stod2(s.c_str());
}

// convert number with radix (RxN) to string
char* dtos2(long double num, uint8_t r = 10);

// replace some escape sequences
string unescape(string s);

// undo replacement of escape sequences
string escape(string s);

// ignore == true -> ignore useless whitespace
string readFile(FILE* f, bool ignore);

#endif //_BSINC_CPP_
