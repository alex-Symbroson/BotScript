
//$ g++ -std=c++11 -O3 -c builtins.cpp

#include "builtins.hpp"
#include "include.hpp"
#include "error.hpp"

#include <time.h>
#include <stdlib.h>
#include <iostream>

#define cstr const char*

namespace Builtins {
		//list of builtin functions
	unordered_map<string, var> map;

	var b_print(var param) {
		debug("Builtins::print(%s)", Variables::sstringify(param));
		printf("%s", Variables::sstringify(param, false));
		return V_NULL;
	}

	var b_input(var param) {
		debug("Builtins::input(%s)", Variables::sstringify(param));
		printf("%s", Variables::sstringify(param, false));
		string inp;
		cin >> inp;
		return Variables::create(&inp, T_STR);
	}

	var b_delay(var param) {
		debug("Builtins::delay(%s)", Variables::sstringify(param));
		delay(getInt(param));
		return V_NULL;
	}

	var b_clock(var param) {
		debug("Builtins::clock()");
		uint32_t time = (int)(clock() / 1000);
		return Variables::create(&time, T_INT);
	}


			//returns builtin index (0 = not found)
	bool exists(string s) {
		debug("Builtins::exists(%s)", s.c_str());
		return map.find(s) != map.end();
	}

	void add(string name, vector<cstr> params, var content) {
		debug("Builtins::add(%s, cstr[], var)", name.c_str());

		uint i = params.size();
		var_lst vps(i);
		while(i--) vps.at(i) = Variables::create(params[i], T_STR);
		var_obj obj = {
			{"name", Variables::create(name, T_STR)},
			{"args", Variables::create(vps, T_TRM)},
			{"content", content}
		};

		map[name] = Variables::create(&obj, T_FNC, true);
	}

	var get(string name) {
		debug("Builtins::get(\"%s\")", name.c_str());
		return map[name];
	}

	var call(string name, var arg) {
		debug("Builtins::call(\"%s\", %s)", name.c_str(), Variables::sstringify(arg));
		var_obj fnc = getObj(map[name]);
		return ((var(*)(var))fnc["content"])(arg);
	}

	void create() {
		debug("Builtins::create()");
		vector<cstr> args;
		var a;

		args = {"string"};
		add("print", args, (var)b_print);

		info("testing print:");
		a = Variables::create("hallo\ndu\n", T_STR);
		call("print", a);
		info("success!");

		args = {};
		add("clock", args, (var)b_clock);

		info("testing clock:");
		a = Variables::create(nullptr, T_NIL);
		a = call("clock", a);
		debug(" -> %s", Variables::sstringify(a));
		info("success!");

		args = {"message"};
		add("input", args, (var)b_input);

		info("testing input:");
		a = Variables::create("type smth: ", T_STR);
		a = call("input", a);
		debug(" -> \"%s\"", Variables::sstringify(a));
		info("success!");

		args = {"milliseconds"};
		add("delay", args, (var)b_delay);

		info("testing delay 2 seconds:");
		a = Variables::create((var_int)2000, T_INT);
		int start, end;
		start = clock();
		call("delay", a);
		end = clock();
		info("  needed %.3f ms", (end-start)/1000.);
		info("success!");
	}
}
