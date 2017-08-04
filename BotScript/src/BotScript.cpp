
//c++ -std=c++11 -O3 -o BotScript.out BotScript.cpp;./BotScript.out

#ifndef _BOTSCRIPT_CPP_
#define _BOTSCRIPT_CPP_ 21931

#ifndef MAIN
	#define MAIN _BOTSCRIPT_CPP_
	int main(int argc, char *argv[]);
#endif

#include "include.cpp" //includes basic libraries, functions and classes

	//list of builtin functions
const char* builtins[] = {"print", "input", "delay", "clock"};
const int num_builtins = 4;

	//list of defined variables
vector<variable> variables;

	//returns builtin index (0 = not found)
uint16_t builtin(string s) {
	uint8_t i(0);
	do
		if(*(builtins + i) == s)
			return i + 1;
	while(++i < num_builtins);
	return 0;
}

	//handle builtin call by index
var callBuiltin(uint8_t index, var param) {
	switch(index) {
		case 1: //print
			cout << "in progress"; //TODO param->value;
		break;
		case 2: { //input
			cout << "in progress"; //TODO param->value;
			string inp;
			cin >> inp;
			format(&inp);
			return new variable(&inp, STR);
		}
		case 3: //delay
			delay(*param->getInt());
		break;
		case 4: { //clock
			uint32_t time = (int)(clock() / 1000);
			return new variable(&time, INT);
		}
	}
	return new variable(nullptr, NIL);
}

	//scope interpreter
void* handleScope(var_lst *scope) {

}


#if MAIN == _BOTSCRIPT_CPP_

	//initialisation
int main(int argc, char *argv[]) {
	//argv[0] -> command which executed a.out (path)

		//cout settings
	cout.precision(15); //float digits
	cout << fixed; //prevent scientific notation (e + 00)

	const char* path = argc > 1? argv[1] : "code.bsc";
	string code = readFile("code.bsc", true);
	if(code == "") Error::error("file \"%s\" is empty", path);

		//create code scope of content from default or argument file path
	var_lst main = toCode(&code);

	cout << Variables::stringify(new variable(&main, LST)) << endl;

		//execute code
	handleScope(&main);

	Variables::free();
	printf("\n");
}

#endif //MAIN == _BOTSCRIPT_CPP_

#endif //_BOTSCRIPT_CPP_
