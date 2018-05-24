
//$ g++ -std=c++11 -O3 BotScript.cpp -lwiringPi -pthread  include.o variables.o pins.o threads.o builtins.o

#include "include.hpp" //includes special functions
#include "builtins.hpp"
#include "macros.hpp"

using namespace std;

	//list of defined variables
vector<Variable> variables;

	// scope interpreter
var handleScope(var_lst* scope) {
	BEGIN("var_lst*scope");

	END();
	return V_NULL;
}

	//initialisation
int main(int argc, char* argv[]) {
	BEGIN("int argc=%i,char**argv",argc);
	PRINT_STATUS();
	//argv[0] -> command which executed a.out (path)

	INFO("init builtins");
	Builtins::create();

	const char* path = argc > 1? argv[1] : "res/code.bsc";
	string code = readFile(path, true);
	INFO("file input: \"%s\"", code.c_str());
	if(code == "") error("file \"%s\" is empty", path);

#if DEBUG == 1
	printf("press enter to continue\n");
	wait_enter();
#endif

		//create code scope of content from default or argument file path
	INFO("formatting code");
	var_lst main = toCode(&code);

	printf("main: %s\n", Variables::sstringify(main));

		//execute code
	INFO("executing code");
	printf("returned: %s\n", Variables::sstringify(handleScope(&main)));

		//free all allocated variables
	INFO("freeing...");
	Variables::free();
	printf("\n");

	INFO("end.");
	END();
	return 0;
}
