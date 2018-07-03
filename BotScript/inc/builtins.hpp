
#ifndef _BUILTINS_HPP_
#define _BUILTINS_HPP_

#include "include.hpp"
#include "variables.hpp"

#define getBltin(NAME) (&builtins[NAME])
extern unordered_map<string, TBltFnc> builtins;

void initBuiltins();
bool isBuiltin(const char*);

#endif //_BUILTINS_CPP_
