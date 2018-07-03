
#ifndef _BSINC_HPP_
#define _BSINC_HPP_

#include "headers.hpp"

bool isSymbol(char c);
bool isWhitespace(char c);
bool isOperator(char c);
bool isOperator(string s);

// replace all in string
string replace(string str, string src, string ovr);

// delay in milliseonds
void delay(double time);

// convert number string with radix (RxN) to double
double stod2(const char* s);
double stod2(string& s);

// replace some escape sequences
string unescape(string s);

// undo replacement of escape sequences
string escape(string s);

// ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore);

#endif //_BSINC_CPP_
