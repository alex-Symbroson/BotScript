
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
	if(index == 1) //print
		cout << "in progress"; //TODO param->value;

	else if(index == 2) {//input
		cout << "in progress"; //TODO param->value;
		string inp;
		cin >> inp;
		format(&inp);
		return new variable(inp);
	}
	else if(index == 3) //delay
		delay(*param->getInt());

	else if(index == 4) {//clock
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%i", (int)(clock() / 1000));
		return new variable((int)(strtof(string(buffer))));
	}
	return new variable((int*)0);
}

	//scope interpreter
void* handleScope(var_lst *scope) {

}


#if MAIN == 1

	//initialisation
int main(int argc, char *argv[]) {
	//argv[0] -> command which executed a.out (path)

		//cout settings
	cout.precision(15); //float digits
	cout << fixed; //prevent scientific notation (e + 00)

		//create code scope of content from default or argument file path
	var_lst code = readFile(argc > 1? argv[1] : "code.bsc");

		//execute code
	handleScope( &code);

	printf("\n");
}
#endif
