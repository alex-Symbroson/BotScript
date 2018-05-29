
#ifndef _BUILTINS_HPP_
#define _BUILTINS_HPP_

#include "variables.hpp"

#define uint16_t unsigned short int

namespace Builtins {
    PVar b_print(PVar param);

    PVar b_input(PVar param);

    PVar b_delay(PVar param);

    PVar b_clock(PVar param);

    // returns builtin index (0 = not found)
    bool exists(string s);

    void add(string name, vector<const char*> params[], PVar content);

    PVar get(string name);

    PVar call(string name, PVar arg);

    void create();
} // namespace Builtins

#endif //_BUILTINS_CPP_
