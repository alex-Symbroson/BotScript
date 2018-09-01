
#ifndef _VARIABLES_HPP_
#define _VARIABLES_HPP_

#include "include.hpp"

#define uint8_t unsigned char

// type id's  (requires addition in variables.cpp: VAR_Type[])
#define T_NIL 0
// #define T_VAR 1
#define T_BIN 1  // boolean
#define T_INT 2  // integer
#define T_FLT 3  // float/double
#define T_STR 4  // string
#define T_LST 5  // list
#define T_OBJ 6  // object
#define T_PIN 7  // pin
#define T_TRM 8  // term
#define T_FNC 9  // function
#define T_OPR 10 // operator
#define T_ARG 11 // arguments
#define T_BFN 12 // builtin function
#define T_MFN 13 // member function
#define TCNT 14

// keywords
//! add in interpret.cpp/ctrl_types and CTRL_Type
#define K_NIL 14 // true
#define K_TRU 15 // true
#define K_FLS 16 // false
#define KCNT 17

// control statements
#define C_CIF 17 // if ()
#define C_EIF 18 // elif ()
#define C_ELS 19 // else ()
#define C_CDO 20 // do {}
#define C_WHL 21 // while ()
#define C_UNT 22 // until ()
#define CCNT 23

class IVar;
template <typename>
class TVar;
typedef IVar* PVar;

#define NEWVAR(v) (dynamic_cast<PVar>(new v))
#define V_NULL NEWVAR(TNil(0))
#define TOSTR(v) v->toStr().c_str()
#define EVALARGS(ARGS, DFLT)           \
    static const var_lst _dflt = DFLT; \
    setDefault(ARGS, _dflt)

// types

typedef char var_nil;
typedef long long var_int;
typedef long double var_flt;
typedef string var_str;
typedef vector<PVar> var_lst;
typedef unordered_map<string, PVar> var_obj;

typedef function<PVar(var_lst)> TFuncSmp;
typedef function<PVar(PVar, PVar)> TFuncOpr;
// typedef unordered_map<string, TFuncSmp> FuncMapSmp;
typedef unordered_map<string, TFuncOpr> FuncMapOpr;

typedef struct {
    const char* name;
    forward_list<var_obj> vars; // list for recursive calls
    var_lst func;               // content
} TFunc;

typedef struct {
    const char* name;
    TFuncSmp func;
} TBltFunc;

typedef TBltFunc* var_bfn;

typedef TVar<var_nil> TNil;
typedef TVar<var_int> TInt;
typedef TVar<var_flt> TFlt;
typedef TVar<var_str> TStr;
typedef TVar<var_lst> TLst;
typedef TVar<var_obj> TObj;
typedef TVar<var_bfn> TBfn;

void initOperations();
PVar evalExpr(PVar& expr);
void setDefault(var_lst& args, const var_lst& dflt);
bool hasOperator(PVar& v, string op);
void cleanupCollector();
void FreeVariables();
const char* typeName(uint8_t t);

extern FuncMapOpr operations[TCNT];
extern PVar handleLine(var_lst& line);
extern uint8_t VAR_Type[CCNT];
extern list<PVar> collector; // garbage collector
extern uint8_t KWType[CCNT - KCNT];

// macros for calling type-specific functions
#define callP(a, o, b) (operations[*(a)->ptype]).at(o)((a), (b))
#define callT(a, o, b) (operations[(a).type]).at(o)((a), (b))

// macros for type related values
#define getType(var) (*(var)->ptype)
#define setType(var, t) (*(var)->ptype = t)
#define baseType(type) VAR_Type[type]
#define getBaseType(var) VAR_Type[getType(var)]
#define getTypeName(var) typeName(getType(var))
#define baseTypeName(type) typeName(baseType(type))
#define getBaseTypeName(var) typeName(getBaseType(var))

// macros for getting var instance values
#define getIntP(var) ((var_int*)(var)->pval)
#define getInt(var) (*(var_int*)(var)->pval)
#define getFltP(var) ((var_flt*)(var)->pval)
#define getFlt(var) (*(var_flt*)(var)->pval)
#define getStrP(var) ((var_str*)(var)->pval)
#define getStr(var) (*(var_str*)(var)->pval)
#define getLstP(var) ((var_lst*)(var)->pval)
#define getLst(var) (*(var_lst*)(var)->pval)
#define getObjP(var) ((var_obj*)(var)->pval)
#define getObj(var) (*(var_obj*)(var)->pval)
#define getBfnP(var) ((var_bfn*)(var)->pval)
#define getBfn(var) (*(var_bfn*)(var)->pval)

class IVar {
  public:
    uint32_t refcnt = 0;
    uint8_t* ptype  = NULL;
    void* pval      = NULL;
    bool isConst    = false;
    list<PVar>::iterator it;

    IVar();
    virtual ~IVar();

    virtual string toStr(bool escape = false) {
        return "null";
    }
};

template <typename T>
class TVar : public IVar {
  public:
    uint8_t type;

    T value;

    TVar(T, uint8_t = 0, bool isConst = true);
    ~TVar();

    string toStr(bool escape = false);
};

/*
    TVar::toStr()
*/

template <>
inline string TVar<var_nil>::toStr(bool) {
    return "null";
}

template <>
inline string TVar<var_int>::toStr(bool) {
    if (type == T_PIN) return "Pin(" + to_string(value) + ")";
    if (type == T_BIN) return value ? "true" : "false";
    if (type >= TCNT) return typeName(type);
    return to_string(value);
}

template <>
inline string TVar<var_flt>::toStr(bool) {
    return dtos2(value);
}

template <>
inline string TVar<var_str>::toStr(bool escapeStr) {
    if (escapeStr)
        return "\"" + escape(value) + "\"";
    else
        return value;
}

template <>
inline string TVar<var_lst>::toStr(bool) {
    string result;
    char lstEnd;
    switch (type >= KCNT ? KWType[type - KCNT] : type) {
        case T_LST:
            result = "[";
            lstEnd = ']';
            break;

        case T_FNC:
            result = "{";
            lstEnd = '}';
            break;

        case T_TRM:
            result = "(";
            lstEnd = ')';
            break;

        case T_ARG:
            result = "<";
            lstEnd = '>';
            break;

        default:
            error_exit("unknown list type id %i - report", type);
            // result = "|"; lstEnd = '|';
    }

    for (PVar& v: value) result += v->toStr(true) + ",";

    if (result.size() > 1)
        result[result.size() - 1] = lstEnd;
    else
        result += lstEnd;

    if (type >= TCNT) result = typeName(type) + (' ' + result);

    return result;
}

template <>
inline string TVar<var_obj>::toStr(bool) {
    string result = "]";

    // builds string reversed because every new unordered_map
    // element is inserted at the front
    for (auto& v: value)
        result = ",\"" + v.first + "\":" + v.second->toStr(true) + result;

    if (!value.empty())
        result[0] = '[';
    else
        result = result + "[";
    return result;
}

template <>
inline string TVar<var_bfn>::toStr(bool) {
    string result = value->name;
    /*
    if (value->argc) {
        result += "(";
        for (uint16_t i = 1; i <= value->argc; i++)
            result += "p" + to_string(i) + ",";
        result[result.size() - 1] = ')';
    } else
        result += "()";
    result += " { [builtin] }"
    */
    return result;
}

#endif //_VARIABLES_HPP_
