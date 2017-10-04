
//$ g++ -std=c++11 -O3 -c include.cpp

#include "include.hpp"
#include "builtins.hpp"
#include "macros.hpp"

#include <math.h>        //pow

	//copied (and modified) from stackoverflow, "ausercomment"
double stod(string s, uint8_t radix) {
	debug("stod(%s,%i)", s.c_str(), radix);
	double n = 0;
	uint8_t x = s.size(), y = 0;
	while(x)
		if(!(s[--x]^'.')) n /= pow(radix, s.size()-1-x), y += s.size()-x;
		else n += ((s[x] - (s[x] <= '9'?'0':'7')) * pow(radix, s.size()-1-x-y));
	return n;
}

	//replace all in string
void replace(string* str, string src, string ovr) {
	debug("replace(string*, string, string)");
	int start = 0;
	while((start = str->find(src, start)) + 1) {
		str->replace(start, src.length(), ovr);
		start += ovr.length(); //case 'ovr' is substring of 'src'
	}
}

	//delay in milliseonds
void delay(int time) {
	debug("delay(%i)", time);
	time = clock() + time*1000;
	while(clock() < time);
}

	//replace some \ placeholders
void format(string* s) {
	debug("formamt(string*)");
	debug("format");
	replace(s, "\\n", "\n");
	replace(s, "\\t", "\t");
	replace(s, "\\033", "\033");
	replace(s, "\\\\", "\\"); //must be last!!
}

	//converts scope string to term
var_lst toFunction(string::iterator* c, string::iterator end) {
	debug("toFunction(string::iterator)");
	var_lst block(0);
	while(**c != ')' && **c != '}' && *c < end) {
		switch(**c) {
			case '"': {
				info("tof \"");
				string word = "";
				while(*++*c != '"' && *c < end) word += **c;
				info("read %s", word.c_str());
				block.push_back(new Variable(&word, T_STR));
			}
			break;
			case '(': case '{': {
				info("tof %c", **c);
				var_lst scope;
				uint8_t type = **c == '('? T_TRM : T_FNC;
				++*c;
				scope = toFunction(c, end);
				info("got %s", Variables::sstringify(scope));
				block.push_back(new Variable(&scope, type));
			}
			break;
			default:
				info("tof def");
				string word = "";
				if(symbols.find(**c) + 1)
					do word += **c; while(symbols.find(*++*c) + 1 && *c < end);
				else
					do word += **c; while(!(symbols.find(*++*c) + 1) && *c < end);
				info("read %s", word.c_str());
				if(Builtins::exists(word)) block.push_back(Builtins::get(word));
				else block.push_back(new Variable(&word, T_VAR));
			continue;
		}
		++*c;
	}
	info("returning %s", Variables::sstringify(block));
	return block;
}

	//start recursive conversation from string to term
var_lst toCode(string* code) {
	debug("toCode(string*)");
	string::iterator c = code->begin();
	return toFunction(&c, code->end());
}

	//ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore) {
	debug("reading file %s", path);
		//file buffer
	FILE *f = fopen(path, "r");

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
	return content;
}
