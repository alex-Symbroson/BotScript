
//c++ -std=c++11 -O3 -o include.out include.cpp;./include.cpp

#ifndef MAIN
	#define MAIN 2
	int main();
#endif


#include "extern.h"
#include <vector>    //alternative array based list container


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

string readFile(const char* path) {
		//file buffer
	FILE *f = fopen(path, "r");

		//file may not exist
	if(!f) error("error reading \"%s\"", path);

		//character from file
	uint8_t c;
	string content;

	while( (c = fgetc(f)) != 255 ) { // 255: eof
		if(c == '"') {
			content += c;
			while((c = fgetc(f)), c != '"') content += c;
		}

			//ignore whitespace
		if(whitespace.find(c) < 3) {
			if(c == '\n') c = ';';
			continue;
		}

			//ignore comments
		if(c == '/') {
			switch(c = fgetc(f)) {
				case '/': //comment line
					while((c = fgetc(f)), c != 255 && c != '\n');
				break;

				case '*': //comment block
					bool brk;
					while((c = fgetc(f)) != 255) {
						if(brk && c == '/') break;
						brk = (c == '*');
					}
				break;
			}
			continue;
		}

		content += c;
	}
	fclose(f);

		//file may be dir (or empty)
	if(content == "") error("error reading empty \"%s\"", path);
	cout << content << "\n\n";
	return content;
}

#endif


#if MAIN == 2

int main() {
	readfile("code.bsc");
	return 0;
};

#endif
