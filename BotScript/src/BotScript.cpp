
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
	debug("started");
	//argv[0] -> command which executed a.out (path)

	const char* path = argc > 1? argv[1] : "res/code.bsc";
	debug("reading...");
	string code = readFile(path, true);
	if(code == "") error("file \"%s\" is empty", path);

		//create code scope of content from default or argument file path
	debug("converting...\n");
	var_lst main = toCode(&code);

	debug("print stringified");
	printf("%s\n", Variables::stringify(new variable(&main, T_TRM)).c_str());

		//execute code
	debug("execute");
	handleScope(&main);

	debug("free space");
	Variables::free();
	printf("\n");
	debug("exit");
}
