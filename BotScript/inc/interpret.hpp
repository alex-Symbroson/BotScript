
#ifndef _BSINTERPRET_HPP_
#define _BSINTERPRET_HPP_

#include "include.hpp"
#include "variables.hpp"

// operator type
typedef struct {
    const char* name;
    uint8_t priority;
    char dir;
} Operator;


extern const unordered_map<string, Operator> operators; // operators
extern const uint8_t CtrlType[CCNT - KCNT];             // keyword types
extern const uint8_t VAR_Type[CCNT];                    // base type of types

extern var_fnc* curScope;           // current executed scope
extern PVar funcResult;             // func return value
extern FuncMapOpr operations[TCNT]; // type operators

inline bool isOperator(string s) {
    return operators.find(s) != operators.end();
}

// execute single code line
PVar handleLine(var_lst& line);

// calls handleLine on each scope line
void handleScope(PVar scope);

// set curScope for variable access and calls handleScope
void handleFunc(PVar func, PVar args);

// converts scope string to term
var_lst toFunction(char*& c, char separator, char end, var_fnc& parent);

// find variable by name
PVar findVar(string name, var_fnc* scope);

// start recursive conversation from string to term
var_fnc toCode(string& code);

#include "builtins.hpp"

#endif //_BSINTERPRET_HPP_
