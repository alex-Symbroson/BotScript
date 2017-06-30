
#include <iostream> //cout, string, file operations
#include <string>   //stoi (string non member functions)
#include <sstream>  //for string to float conversion
#include <vector>   //alternative list container

#define uint   unsigned int       // [0, 4.294.967.295]
#define uint8  unsigned char      // [0, 255]
#define uint16 unsigned short int // [0, 65.535]

#define ufloat unsigned float

	//<stdio,h>
extern "C" FILE *fopen(const char* path, const char* modes);
extern "C" int fclose(FILE *f);
extern "C" int fgetc(FILE *f);

	//<err.h>
extern "C" void err(int status=0, const char* format="", ...);

	//<time.h>
extern "C" long int clock(void);

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
	vector<uint16> variables;

	Scope(string content) {
		this->type  = "scope";
		this->value = content;
	}
};

void replace(string *str, const string& src, const string& ovr) {
    long int start = 0;
    while((start = str->find(src, start)) != -1) {
        str->replace(start, src.length(), ovr);
        start += ovr.length(); // case 'ovr' is substring of 'src'
    }
}

/*
float pow10(int exp) {
	float ret(1);
	if(exp > 0) while(exp--) ret *= 10;
	else if(exp < 0) while(exp++) ret *= 0.1;
	return ret;
}*/

float stof(string s) {
	float f;
	return istringstream(s) >> f, f;

	/*
	float ret(0);
	bool neg(*s.begin()=='-');
	if(neg) replace(&s, ",", "");


	uint8 len(s->length());
	uint8 flt(s->rfind('.'));
	cout << flt << endl;
	uint8 i(len);

	//while(i--) ret += pow10(i)
	*/
}

void delay(uint time) {
	time = clock() + time*1000;
	while(clock() < time);
}

void format(string *s) {
	//replace some \ placeholders
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
	uint8 c;

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
