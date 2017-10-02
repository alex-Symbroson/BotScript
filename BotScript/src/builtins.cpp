
//$ g++ -std=c++11 -O3 -c builtins.cpp

#include "builtins.hpp"
#include "include.hpp"
#include "error.hpp"

//#include <time.h>
//#include <stdlib.h>

namespace Builtins {
			//list of builtin functions
		const char* list[] = {"print", "input", "delay", "clock"};
		#define count 4

	var print(var param) {
		printf("in progress\n"); //TODO param->value;
		return new variable(nullptr, T_NIL);
	}
	/*
	var input(var param) {
		cout << "in progress"; //TODO param->value;
		string inp;
		cin >> inp;
		format(&inp);
		return new variable(&inp, T_STR);
	}

	var delay(var param) {
		delay(getInt(param));
		return new variable(nullptr, T_NIL);
	}

	var clock(var param) {
		uint32_t time = (int)(clock() / 1000);
		return new variable(&time, T_INT);
	}
	*/

			//returns builtin index (0 = not found)
	uint16_t find(string s) {
		uint16_t i(0);
		do
			if(*(list + i) == s)
				return i + 1;
		while(++i < count);
		return 0;
	}

	var add(const char* name, const char* params[], var& content) {
		var_obj obj = {
			{"name", Variables::create(name, T_STR)},
			{"params", Variables::create(params, T_LST)},
			{"content", Variables::create(content, T_FNC)}
		};
		return Variables::create( &obj, T_FNC, true);
	}

	void create() {

	}
}
