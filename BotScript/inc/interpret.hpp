
#ifndef _BSINTERPRET_HPP_
#define _BSINTERPRET_HPP_

#include "include.hpp"
#include "variables.hpp"

PVar callBuiltin(var_bfn func, var_lst& args);

PVar handleScope(var_lst& scope);

// converts scope string to term
var_lst toFunction(string::iterator* c, string::iterator end);

// start recursive conversation from string to term
var_lst toCode(string* code);

#include "builtins.hpp"

#endif //_BSINTERPRET_HPP_
