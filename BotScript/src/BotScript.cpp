
//$ g++ -std=c++11 -O3 BotScript.cpp -lwiringPi -pthread  include.o variables.o pins.o threads.o builtins.o

#include "include.hpp" //includes special functions
#include "builtins.hpp"
#include "macros.hpp"

using namespace std;

	//list of defined variables
vector<Variable> variables;

	//scope interpreter
var handleScope(var_lst* scope) {
	debug("handleScope(var_lst*)");
	return V_NULL;
}

	//initialisation
int main(int argc, char* argv[]) {
	info("start");
	//argv[0] -> command which executed a.out (path)

	info("init builtins");
	Builtins::create();

	const char* path = argc > 1? argv[1] : "res/code.bsc";
	string code = readFile(path, true);
	info("file input: \"%s\"", code.c_str());
	if(code == "") error("file \"%s\" is empty", path);

#if DEBUG == 1
	printf("press enter to continue\n");
	wait_enter();
#endif

		//create code scope of content from default or argument file path
	info("formatting code");
	var_lst main = toCode(&code);

	printf("main: %s\n", Variables::sstringify(main));

		//execute code
	handleScope(&main);

	info("freeing");
	Variables::free();
	printf("\n");
	info("end.");
}
