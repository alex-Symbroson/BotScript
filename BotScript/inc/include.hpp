
#ifndef _BSINC_HPP_
#define _BSINC_HPP_

#include "headers.hpp"

inline bool isAlphaNum(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

inline bool isSymbol(char c) {
    return !isAlphaNum(c);
}

inline bool isWhitespace(char c) {
    return c <= ' ';
}

inline bool isOperator(char c) {
    // BEGIN("%c", c);
    // characters interpreted as operators
    static const string s_operators("<>!%^&*|+=.-/");
    // END();
    return s_operators.find(c) + 1;
}

// replace all in string
string replace(string str, string src, string ovr);

// delay in milliseonds
void delay(long double time);

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
string readFile(const char* path, bool ignore);

#endif //_BSINC_CPP_
