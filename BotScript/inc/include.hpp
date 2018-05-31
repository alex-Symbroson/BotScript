
#ifndef _BSINC_HPP_
#define _BSINC_HPP_

#include "headers.hpp"

bool isSymbol(char);
bool isWhitespace(char);
bool isOperator(char);
bool isOperator(string);

// characters interpreted as symbols
const string symbols(" \t\r\n/\\()\"':,.;<>~!@#$%^&*|+=[]{}`?-…");

// characters interpreted as operators (op and op=)
const string operators("<>!%^&*|+=");

// whitespace characters
const string whitespace(" \t\r\n");

// copied (and modified) from stackoverflow, "ausercomment"
double stod(string s, uint8_t radix);

string dtos(double d);

// replace all in string
void replace(string* str, string src, string ovr);

// delay in milliseonds
void delay(int time);

// replace some \ placeholders
string format(string& s);

// ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore);

#endif //_BSINC_CPP_
