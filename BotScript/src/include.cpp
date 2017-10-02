
//$ g++ -std=c++11 -O3 -c include.cpp

#include "include.hpp"
#include "error.hpp"
#include "builtins.hpp"

#include <math.h>        //pow

	//copied (and modified) from stackoverflow, "ausercomment"
double stod(string s, uint8_t radix) {
	debug("stod %s", s.c_str());
	double n = 0;
	uint8_t x = s.size(), y = 0;
	while(x)
		if(!(s[--x]^'.')) n /= pow(radix, s.size()-1-x), y += s.size()-x;
		else n += ((s[x] - (s[x] <= '9'?'0':'7')) * pow(radix, s.size()-1-x-y));
	return n;
}

string dtos(double d) {
	debug("dtos %f", d);
	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%.15f", d);
	return string(buffer);
}

	//replace all in string
void replace(string* str, string src, string ovr) {
	debug("replace");
	int start = 0;
	while((start = str->find(src, start)) + 1) {
		str->replace(start, src.length(), ovr);
		start += ovr.length(); //case 'ovr' is substring of 'src'
	}
}

#ifndef delay //can be imported with wiringpi.h
	//delay in milliseonds
void delay(int time) {
	time = clock() + time*1000;
	while(clock() < time);
}
#endif

	//replace some \ placeholders
void format(string *s) {
	debug("format");
	replace(s, "\\n", "\n");
	replace(s, "\\t", "\t");
	replace(s, "\\033", "\033");
	replace(s, "\\\\", "\\"); //must be last!!
}

	//converts scope string to term
var_lst toFunction(string::iterator* c) {
	debug("toFunction");
	var_lst block = {};
	while(**c != ')' && **c != '}' && **c != 255) {
		switch(**c) {
			case '"': {
				debug("toFunction case '\"'");
				string word = "";
				while(*++*c != '"') word += **c;
				block.push_back(new variable(&word, T_STR));
			}
			break;
			case '(': case '{': {
				debug("toFunction case '(' or '{'");
				var_lst scope;
				uint8_t type = **c == '('? T_TRM : T_FNC;
				++*c;
				scope = toFunction(c);
				block.push_back(new variable(&scope, type));
			}
			break;
			default:
				debug("toFunction case default");
				string word = "";
				if(symbols.find(**c) + 1)
					do word += **c; while(symbols.find(*++*c) + 1);
				else
					do word += **c; while(!(symbols.find(*++*c) + 1));
				if(Builtins::find(word)) block.push_back(new variable(&word, T_FNC));
				else block.push_back(new variable(&word, T_VAR));
			continue;
		}
		++*c;
	}
	return block;
}

	//start recursive conversation from string to term
var_lst toCode(string* code) {
	string::iterator c = code->begin();
	return toFunction(&c);
}

	//ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore) {
	debug("reading file %s", path);
		//file buffer
	FILE *f = fopen(path, "r");
	debug("file: %i", f);

	if(!f) error("file \"%s\" does not exist!", path);

		//character from file
	int c;
	string content = "";

	while( (c = fgetc(f)) != EOF ) {
		if(ignore) {
				//whitespace
			if(whitespace.find(c) < 3) {
				if(c == '\n' && *string::reverse_iterator(content.end()) != ';')
					content += ';';
				continue;
			}

				//comments
			if(c == '/') {
				c = fgetc(f);
					//line
				if(c == '/')
					do c = fgetc(f);
					while(c != EOF && c != '\n');
					//block
				else if(c == '*') {
					bool brk = false;
					while((c = fgetc(f)) != EOF) {
						if(brk && c == '/') break;
						brk = (c == '*');
					}
				}
				continue;
			}
		}
		content += c;
	}
	content += c;
	debug("got \"%s\n\"", content.c_str());
	return content;
}
