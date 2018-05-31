
#ifndef _BUILTINS_HPP_
#define _BUILTINS_HPP_

#include "variables.hpp"

#define getBltin(NAME) (&builtins[NAME])
extern unordered_map<string, TBltFnc> builtins;

bool builtin_exists(const char*);

#endif //_BUILTINS_CPP_
