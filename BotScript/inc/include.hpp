
#ifndef _BSINC_HPP_
#define _BSINC_HPP_

#include "builtins.hpp"
#include "headers.hpp"
#include "variables.hpp"

using namespace std;

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
void format(string* s);

// converts scope string to term
var_lst toFunction(string::iterator* c, string::iterator end);

// start recursive conversation from string to term
var_lst toCode(string* code);

// ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore);

#endif //_BSINC_CPP_
