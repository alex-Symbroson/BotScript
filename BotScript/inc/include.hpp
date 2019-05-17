
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

// delay in milliseonds
void delay_ms(long double time);

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
