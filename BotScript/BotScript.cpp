
#include "include.cpp" //includes basic libraries, functions and classes

	//list of builtin functions
const char* builtins[] = {"print", "input", "delay"};
const int num_builtins = 3;

	//list of defined variables
vector<variable> variables;

	//returns variable by name based on a list of indices of available variables
variable getVar(vector<uint16> vec, string name) {
	if(vec.size()) {
		vector<uint16>::iterator i(vec.begin()), end(vec.end());
		do if(variables[*i].name == name)
				return variables[*i];
		while(++i < end);
	}
	return variable("", "", "");
}

	//returns builtin index (0 = not found)
uint16 builtin(string s) {
	uint16 i(0);
	do if(*(builtins+i) == s) return i + 1;
	while(++i < num_builtins);
	return 0;
}

	//handle builtin call by index
variable callBuiltin(uint8 index, variable *var) {
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
			uint16 scopecount(0);
			while(*++c != ')' || scopecount) {
				if(*c == '(') scopecount++;
				else if(*c == ')') scopecount--;
				scp.value += *c;
			}
			temp = handleScope(&scp);
		}

		if(word.length()) {
			//wenn word = scope & gefolgt von scope then call 1st scope
			uint16 index(builtin(word));
			if(index) //builtin detected
				temp = callBuiltin(index, &temp);

			word = "";
		}
	} while(++c < end);
	return temp;
}

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
}
