
//$ g++ -std=c++11 -O3 BotScript.cpp -lwiringPi -pthread  include.o variables.o pins.o threads.o builtins.o

#include "include.hpp" //includes basic libraries, functions and classes
#include "error.hpp"

#include <iostream>

using namespace std;

	//list of defined variables
vector<variable> variables;

	//scope interpreter
void* handleScope(var_lst *scope) {

}

	//initialisation
int main(int argc, char *argv[]) {
	//argv[0] -> command which executed a.out (path)

		//cout settings
	cout.precision(15); //float digits
	cout << fixed; //prevent scientific notation (e + 00)

	const char* path = argc > 1? argv[1] : "code.bsc";
	string code = readFile("code.bsc", true);
	if(code == "") error("file \"%s\" is empty", path);

		//create code scope of content from default or argument file path
	var_lst main = toCode(&code);

	cout << Variables::stringify(new variable(&main, T_TRM)) << endl;

		//execute code
	handleScope(&main);

	Variables::free();
	printf("\n");
}
