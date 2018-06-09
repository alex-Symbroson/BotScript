
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

// replace all in string
string replace(string str, string src, string ovr);

// delay in milliseonds
void delay(int time);

// replace some \ placeholders
string format(string s);

string unformat(string s);

// ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore);

#endif //_BSINC_CPP_
