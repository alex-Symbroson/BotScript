
#ifndef _BSINTERPRET_HPP_
#define _BSINTERPRET_HPP_

#include "include.hpp"
#include "variables.hpp"

typedef struct {
    const char* name;
    uint8_t priority;
    char dir;
} Operator;

extern unordered_map<string, Operator> operators;

PVar callBuiltin(var_bfn func, var_lst args);

PVar handleLine(var_lst& line);

void handleScope(var_lst& scope);

// converts scope string to term
var_lst toFunction(string::iterator& c, char separator, char end);

// start recursive conversation from string to term
var_lst toCode(string& code);

#include "builtins.hpp"

#endif //_BSINTERPRET_HPP_
