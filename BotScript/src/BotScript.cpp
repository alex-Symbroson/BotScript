
//$ g++ -std=c++11 -O3 BotScript.cpp -lwiringPi -pthread  include.o variables.o pins.o threads.o builtins.o

#include "include.hpp" //includes special functions
#include "error.hpp"

using namespace std;

	//list of defined variables
vector<variable> variables;

	//scope interpreter
var handleScope(var_lst *scope) {

	return V_NULL;
}

	//initialisation
int main(int argc, char *argv[]) {
	//argv[0] -> command which executed a.out (path)

	const char* path = argc > 1? argv[1] : "res/code.bsc";
	string code = readFile(path, true);
	if(code == "") error("file \"%s\" is empty", path);

		//create code scope of content from default or argument file path
	var_lst main = toCode(&code);

	printf("%s\n", Variables::stringify(new variable(&main, T_TRM)).c_str());

		//execute code
	handleScope(&main);

	Variables::free();
	printf("\n");
}
