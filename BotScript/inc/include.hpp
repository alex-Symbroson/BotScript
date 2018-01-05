
#ifndef _BSINC_HPP_
#define _BSINC_HPP_

#include "headers.hpp"
#include "variables.hpp"

using namespace std;

	//characters interpreted as symbols
const string symbols(" \n\t!\"%'()* + , - / :;<=>?[]");

	//whitespace characters
const string whitespace(" \t\n");

	//copied (and modified) from stackoverflow, "ausercomment"
double stod(string s, uint8_t radix);

string dtos(double d);

	//replace all in string
void replace(string* str, string src, string ovr);

	//delay in milliseonds
void delay(int time);

	//replace some \ placeholders
void format(string *s);

	//converts scope string to term
var_lst toFunction(string::iterator* c, string::iterator end);

	//start recursive conversation from string to term
var_lst toCode(string* code);

	//ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore);

#endif //_BSINC_CPP_
