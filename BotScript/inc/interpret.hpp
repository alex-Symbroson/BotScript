
#ifndef _BSINTERPRET_HPP_
#define _BSINTERPRET_HPP_

#include "include.hpp"
#include "variables.hpp"


typedef struct {
    const char* name;
    uint8_t priority;
    char dir;
} Operator;


extern const unordered_map<string, Operator> operators; // operators
extern const uint8_t CtrlType[CCNT - KCNT];             // keyword types
extern const uint8_t VAR_Type[CCNT];                    // base type of types

extern var_fnc* curScope;           // current sxscureted scope
extern PVar funcResult;             // func return value
extern FuncMapOpr operations[TCNT]; // type operators

//#define isOperator(s) (operators.find(s) != operators.end())
inline bool isOperator(string s) {
    return operators.find(s) != operators.end();
}

PVar handleLine(var_lst& line);

void handleScope(var_fnc& scope);

// converts scope string to term
var_lst toFunction(char*& c, char separator, char end, var_fnc& parent);

// find variable by name
PVar findVar(string name, var_fnc* scope);

// start recursive conversation from string to term
var_fnc toCode(string& code);

#include "builtins.hpp"

#endif //_BSINTERPRET_HPP_
