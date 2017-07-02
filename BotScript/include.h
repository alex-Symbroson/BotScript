
#include <iostream>  //cout, string, FILE, freadc, fopen, fclose
#include <cmath>     //pow10
#include <vector>    //alternative list container
#include <stdint.h>  //uintn_t types

	//<stdio.h>
extern "C" {
	FILE *fopen(const char* path, const char* modes);
	int fclose(FILE *f);
	int fgetc(FILE *f);
	int printf(const char* f, ...);

	//<err.h>
	void err(int status = 0, const char* format = "", ...);

	//<time.h>
	long int clock(void);
}

using namespace std;

	//characters interpreted as symbols
const string symbols(" \n\t!\"%'()*+,-/:;<=>?[]");

	//whitespace characters
const string whitespace(" \t\n");


class variable {
public:
	string type, name, value;

	variable(string name, string value, string type) {
		this->type  = type;
		this->name  = name;
		this->value = value;
	}
};

class Scope {
public:
	string type, value;
	vector<uint16_t> variables;

	Scope(string content) {
		this->type  = "scope";
		this->value = content;
	}
};

	//replace string
void replace(string *str, const string& src, const string& ovr) {
	long int start = 0;
	while((start = str->find(src, start)) + 1) {
		str->replace(start, src.length(), ovr);
		start += ovr.length(); // case 'ovr' is substring of 'src'
    }
}

	//convert string to double precision float (16 digits)
double strtof(string s) {
	double ret(0);
	if(*(s.end()-1) == '.') s += "0";
	string::reverse_iterator c(s.begin()), end(s.end());
	uint32_t f(s.find('.') + 1);
	if(!f) f = s.length();
	else f--;

	while(--c >= end) {
		if(*c == '.' || *c == '-') --c;
		ret += pow10(--f) * (*c - 48);
	}
	return *s.begin()=='-'?-ret:ret;
}

	//delay in milliseond
void delay(uint32_t time) {
	time = clock() + time*1000;
	while(clock() < time);
}

	//replace some \ placeholders
void format(string *s) {
	replace(s, "\\n", "\n");
	replace(s, "\\t", "\t");
	replace(s, "\\\\", "\\"); //must be last!!
}

string readFile(const char* path) {
		//file buffer
	FILE *f = fopen(path, "r");

		//file may not exist
	if(!f) err(0, "error reading \"%s\"", path);

		//charakter from file
	uint8_t c;

		//whole content
	string content;

	bool isString, wasSymbol;


	while( (c = fgetc(f)) != 255 ) { // 255: eof
		if(c == '"') isString = !isString;

			//ignore whitespace
		if(!isString && wasSymbol)
			while(whitespace.find(c) < 3) c = fgetc(f);

			//ignore comments
		if(c == '/') {
			switch(c = fgetc(f)) {
				case '/': //comment line
					while((c = fgetc(f)) != 255)
						if(c == '\n') break;
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

		wasSymbol = (symbols.find(c) < 22);
		content += c;
	}
	fclose(f);

		//file may be dir (or empty)
	if(content == "") err(0, "error reading empty \"%s\"", path);
	cout << content << "\n\n";
	return content;
}
