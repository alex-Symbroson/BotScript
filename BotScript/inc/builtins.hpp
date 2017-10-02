
#ifndef _BUILTINS_HPP_
#define _BUILTINS_HPP_

#include "variables.hpp"

#define uint16_t unsigned short int

namespace Builtins {
	var print(var param);
/*
	var input(var param);

	var delay(var param);

	var clock(var param);
*/

			//returns builtin index (0 = not found)
	uint16_t find(string s);

	var add(const char* name, const char* params[], var& content);

	void create();
}

#endif //_BUILTINS_CPP_

/*
#if MAIN == _BUILTINS_CPP_

int main() {
	var_obj* print = builtins::add("input", {"text"}, builtins::print);
	input["content"]("hallo\n");
}
#endif //MAIN == _BUILTINS_CPP_
*/
