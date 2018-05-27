
#ifndef _BUILTINS_HPP_
#    define _BUILTINS_HPP_

#    include "variables.hpp"

#    define uint16_t unsigned short int

namespace Builtins {
    var b_print(var param);

    var b_input(var param);

    var b_delay(var param);

    var b_clock(var param);

    // returns builtin index (0 = not found)
    bool exists(string s);

    void add(string name, vector<const char*> params[], var content);

    var get(string name);

    var call(string name, var arg);

    void create();
} // namespace Builtins

#endif //_BUILTINS_CPP_

/*
#if MAIN == _BUILTINS_CPP_

int main() {
    var_obj* print = builtins::add("input", {"text"}, builtins::print);
    input["content"]("hallo\n");
}
#endif //MAIN == _BUILTINS_CPP_
*/
