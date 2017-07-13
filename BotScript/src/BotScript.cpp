
//c++ -std=c++11 -O3 -o BotScript.out BotScript.cpp;./BotScript.out

#ifndef MAIN
	#define MAIN 1
	int main(int argc, char *argv[]);
#endif


#include "include.cpp" //includes basic libraries, functions and classes

	//list of builtin functions
const char* builtins[] = {"print", "input", "delay", "clock"};
const int num_builtins = 4;

	//list of defined variables
vector<variable> variables;

	//returns variable by name based on a list of indices of available variables
variable getVar(vector<uint16_t> vec, string name) {
	if(vec.size()) {
		vector<uint16_t>::iterator i(vec.begin()), end(vec.end());
		do
			if(variables[*i].name == name)
				return variables[*i];
		while(++i < end);
	}
	return variable("", "", "");
}

	//returns builtin index (0 = not found)
uint16_t builtin(string s) {
	uint8_t i(0);
	do
		if(*(builtins+i) == s)
			return i + 1;
	while(++i < num_builtins);
	return 0;
 }

	//handle builtin call by index
variable callBuiltin(uint8_t index, variable *var) {
	variable ret("", "", "");
	switch(index) {
		case 1: //print
			cout << var->value;
		break;

		case 2: //input
			cout << var->value;
			ret.type = "string";
			cin >> ret.value;
			format(&ret.value);
		break;

		case 3: //delay
			delay(strtof(var->value));
		break;

			//clock
		case 4:
			ret.type = "integer";
			char buffer[16];
			snprintf(buffer, sizeof(buffer), "%i", (int)(clock()/1));
			ret.value = buffer;
		break;
	}
	return ret;
}

	//scope interpreter
variable handleScope(Scope *scope) {
	variable temp("", "", "");
	if(scope->value == "") return temp;

	string word;
	string::iterator c(scope->value.begin()), end(scope->value.end());

	do {
		while(symbols.find(*c) > 21) word += *c++;

		if(*c == '"') {
			temp.type = "string";
			while(*++c != '"') temp.value += *c;
			format(&temp.value);
			c++;
		} else if(*c == '(') {
			Scope scp("");
			uint16_t scopecount(0);
			while(*++c != ')' || scopecount) {
				if(*c == '(') scopecount++;
				else if(*c == ')') scopecount--;
				scp.value += *c;
			}
			temp = handleScope(&scp);
		}

		if(word.length()) {
			//wenn word = scope & gefolgt von scope then call 1st scope
			uint16_t index(builtin(word));
			if(index) //builtin detected
				temp = callBuiltin(index, &temp);

			word = "";
		}
	} while(++c < end);
	return temp;
}


#if MAIN == 1

	//initialisation
int main(int argc, char *argv[]) {
	//argv[0] -> command which executed a.out (path)

		//cout settings
	cout.precision(15); //float digits
	cout << fixed; //prevent scientific notation (e+00)

		//create code scope of content from default or argument file path
	Scope code(readFile(argc > 1 ? argv[1] : "code.bsc"));

		//execute code
	handleScope(&code);

	printf("\n");
}
#endif