
//$ g++ -std=c++11 -O3 -c builtins.cpp

#include "builtins.hpp"
#include "include.hpp"

#define CUSTOM_BEGIN
#include "macros.hpp"

#include <stdlib.h>
#include <time.h>

#define cstr const char*

#define BEGIN(...) BEGIN_1("Builtins", __VA_ARGS__)
#define END(...) END_1("Builtins", __VA_ARGS__)

namespace Builtins {
    // list of builtin functions
    unordered_map<string, var> map;

    var b_print(var param) {
        BEGIN("var param=\"%s\"", Variables::sstringify(param));
        printf("%s", Variables::sstringify(param, false));
        END();
        return V_NULL;
    }

    var b_input(var param) {
        BEGIN("var param=\"%s\"", Variables::sstringify(param));
        printf("%s", Variables::sstringify(param, false));
        string inp;
        cin >> inp;
        END();
        return Variables::create(&inp, T_STR);
    }

    var b_delay(var param) {
        BEGIN("var param=\"%s\"", Variables::sstringify(param));
        delay(getInt(param));
        END();
        return V_NULL;
    }

    var b_clock(var param) {
        BEGIN("");
        uint32_t time = (int)(clock() / 1000);
        END();
        return Variables::create(&time, T_INT);
    }

    // returns builtin index (0 = not found)
    bool exists(string s) {
        BEGIN("string s=%s", s.c_str());
        END();
        return map.find(s) != map.end();
    }

    void add(string name, vector<cstr> params, var content) {
        BEGIN("string name=%s,vector<cstr>params,var content)", name.c_str());

        uint i = params.size();
        var_lst vps(i);
        while (i--) vps.at(i) = Variables::create(params[i], T_STR);
        var_obj obj = {{"name", Variables::create(name, T_STR)},
                       {"args", Variables::create(vps, T_TRM)},
                       {"content", content}};

        map[name] = Variables::create(&obj, T_FNC, true);
        END();
    }

    var get(string name) {
        BEGIN("string name=\"%s\"", name.c_str());
        END();
        return map[name];
    }

    var call(string name, var arg) {
        BEGIN(
            "string name=\"%s\",var arg=%s)", name.c_str(),
            Variables::sstringify(arg));
        var_obj fnc = getObj(map[name]);
        END();
        return ((var(*)(var))fnc["content"])(arg);
    }

    void create() {
        BEGIN("");
        vector<cstr> noparams = {};

        add("print", {"string"}, (var)b_print);
        add("clock", noparams, (var)b_clock);
        add("delay", {"milliseconds"}, (var)b_delay);
        add("input", {"message"}, (var)b_input);

#if TEST_BUILTINS == 1
        var a;

        DEBUG("testing print:");
        a = Variables::create("hallo\ndu\n", T_STR);
        call("print", a);
        DEBUG("success!");

        DEBUG("testing clock:");
        a = Variables::create(nullptr, T_NIL);
        a = call("clock", a);
        DEBUG(" -> %s", Variables::sstringify(a));
        DEBUG("success!");

        DEBUG("testing input:");
        a = Variables::create("type smth: ", T_STR);
        a = call("input", a);
        DEBUG(" -> %s", Variables::sstringify(a));
        DEBUG("success!");

        DEBUG("testing delay 2 seconds:");
        a = Variables::create((var_int)2000, T_INT);
        int start, end;
        start = clock();
        call("delay", a);
        end = clock();
        DEBUG("  needed %.3f ms", (end - start) / 1000.);
        DEBUG("success!");
#endif
        END();
    }
} // namespace Builtins
