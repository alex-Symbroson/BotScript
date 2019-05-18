
#ifndef _BUILTINS_HPP_
#define _BUILTINS_HPP_

#include "include.hpp"
#include "variables.hpp"
#include "interpret.hpp"

#define getBltin(NAME) (&builtins[NAME])
extern unordered_map<string, TBltFunc> builtins;

bool initBuiltins();
bool isBuiltin(const char*);

#endif //_BUILTINS_CPP_
