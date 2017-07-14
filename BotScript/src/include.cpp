
//c++ -std=c++11 -O3 -o include.out include.cpp;./include.cpp

#ifndef MAIN
	#define MAIN 2
	int main();
#endif


#include "extern.h"
#include "variables.cpp"


#ifndef _BSINC_CPP_
#define _BSINC_CPP_

	//characters interpreted as symbols
const string symbols(" \n\t!\"%'()*+,-/:;<=>?[]");

	//whitespace characters
const string whitespace(" \t\n");


	//replace string
void replace(string *str, const string& src, const string& ovr) {
	long int start = 0;
	while((start = str->find(src, start)) + 1) {
		str->replace(start, src.length(), ovr);
		start += ovr.length(); // case 'ovr' is substring of 'src'
    }
}

	//replace some \ placeholders
void format(string *s) {
	replace(s, "\\n", "\n");
	replace(s, "\\t", "\t");
	replace(s, "\\033", "\033");
	replace(s, "\\\\", "\\"); //must be last!!
}

var_lst readFile(const char* path) {
		//file buffer
	FILE *f = fopen(path, "r");

		//file may not exist
	if(!f) Error::error("error reading \"%s\"", path);

		//character from file
	uint8_t c;
	string content;

	while( (c = fgetc(f)) != 255 ) { // 255: eof

	}
}

#endif


#if MAIN == 2

int main() {
	readfile("code.bsc");
	return 0;
};

#endif
