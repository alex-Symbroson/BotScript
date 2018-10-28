
#ifndef _VARIABLES_HPP_
#define _VARIABLES_HPP_

#include "include.hpp"


// predefinitions
class IVar;
template <typename>
class TVar;
typedef IVar* PVar;


// type id's  (! add VAR_Type[], typeName())
#define T_NIL 0  // null
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

// keywords  (! add VAR_Type[], keywords{}, typeName())
#define K_NIL 14 // true
#define K_TRU 15 // true
#define K_FLS 16 // false
#define K_BRK 17 // break
#define K_RET 18 // return [(value)]
#define KCNT 19

// control statements (! add VAR_Type[], KWType[], keywords{}, typeName())
#define C_CIF 19 // if ()
#define C_EIF 20 // elif ()
#define C_ELS 21 // else ()
#define C_CDO 22 // do {}
#define C_WHL 23 // while ()
#define C_UNT 24 // until ()
#define CCNT 25


// new variables
#define newNil() NEWVAR(TChr(0, T_NIL, false))
#define newBin(v) NEWVAR(TChr(v, T_BIN, false))
#define newInt(v) NEWVAR(TInt(v, T_INT, false))
#define newPin(v) NEWVAR(TInt(v, T_PIN, false))
#define newFlt(v) NEWVAR(TFlt(v, T_FLT, false))
#define newStr(v) NEWVAR(TStr(v, T_STR, false))
#define newLst(v) NEWVAR(TLst(v, T_LST, false))
#define newTrm(v) NEWVAR(TLst(v, T_TRM, false))
#define newFnc(v) NEWVAR(TLst(v, T_FNC, false))
#define newObj(v) NEWVAR(TObj(v, T_OBJ, false))
#define newOpr(v) NEWVAR(TStr(v, T_OPR, false))
#define newArg(v) NEWVAR(TLst(v, T_ARG, false))
#define newBfn(v) NEWVAR(TBfn(v, T_BFN, false))

// new constants
#define newNilC() NEWVAR(TChr(0, T_NIL, true))
#define newBinC(v) NEWVAR(TChr(v, T_BIN, true))
#define newIntC(v) NEWVAR(TInt(v, T_INT, true))
#define newPinC(v) NEWVAR(TInt(v, T_PIN, true))
#define newFltC(v) NEWVAR(TFlt(v, T_FLT, true))
#define newStrC(v) NEWVAR(TStr(v, T_STR, true))
#define newLstC(v) NEWVAR(TLst(v, T_LST, true))
#define newTrmC(v) NEWVAR(TLst(v, T_TRM, true))
#define newFncC(v) NEWVAR(TLst(v, T_FNC, true))
#define newObjC(v) NEWVAR(TObj(v, T_OBJ, true))
#define newOprC(v) NEWVAR(TStr(v, T_OPR, true))
#define newArgC(v) NEWVAR(TLst(v, T_ARG, true))
#define newBfnC(v) NEWVAR(TBfn(v, T_BFN, true))


// get raw var value instance
#define getChrRaw(var) (*(var_chr*)(var)->pval)
#define getIntRaw(var) (*(var_int*)(var)->pval)
#define getFltRaw(var) (*(var_flt*)(var)->pval)
#define getStrRaw(var) (*(var_str*)(var)->pval)
#define getLstRaw(var) (*(var_lst*)(var)->pval)
#define getObjRaw(var) (*(var_obj*)(var)->pval)
#define getBfnRaw(var) (*(var_bfn*)(var)->pval)
#define getOprRaw(var) getStrRaw(var)
#define getTrmRaw(var) getLstRaw(var)
#define getFncRaw(var) getLstRaw(var)

// get var value instance with type assertion
#define getNil(var) getChrRaw(assertT(var, T_NIL))
#define getBin(var) getChrRaw(assertT(var, T_BIN))
#define getInt(var) getIntRaw(assertT(var, T_INT))
#define getPin(var) getIntRaw(assertT(var, T_PIN))
#define getFlt(var) getFltRaw(assertT(var, T_FLT))
#define getStr(var) getStrRaw(assertT(var, T_STR))
#define getLst(var) getLstRaw(assertT(var, T_LST))
#define getTrm(var) getLstRaw(assertT(var, T_TRM))
#define getFnc(var) getLstRaw(assertT(var, T_FNC))
#define getObj(var) getObjRaw(assertT(var, T_OBJ))
#define getOpr(var) getStrRaw(assertT(var, T_OPR))
#define getArg(var) getLstRaw(assertT(var, T_ARG))
#define getBfn(var) getBfnRaw(assertT(var, T_BFN))


// macros for type related values
#define getType(var) ((var)->type)
#define baseType(type) VAR_Type[type]
#define setType(var, t) ((var)->type = t)
#define getBaseType(var) VAR_Type[getType(var)]
#define getTypeName(var) typeName(getType(var))
#define baseTypeName(type) typeName(baseType(type))
#define getBaseTypeName(var) typeName(getBaseType(var))


// other variable related macros
#define NEWVAR(v) (dynamic_cast<PVar>(new v))
#define REPVAR(var, v) (decRef(var), incRef(var = v))
#define TOSTR(val, ...) toStr(val, ##__VA_ARGS__).c_str()
#define CALLOPR(a, o, b) (operations[a->type]).at(o)((a), (b))
#define FILLARGS(ARGS, DFLT)     \
    static var_lst _dflt = DFLT; \
    fillArgs(ARGS, _dflt)

/*
inline void REPVAR(PVar a, PVar b) {
    if (var->refcnt == 1) {
        if (getType(a) == getType(b)) a.value = b.value;
    }
}*/

// typedefs

typedef char var_chr;
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

typedef TVar<var_chr> TChr;
typedef TVar<var_int> TInt;
typedef TVar<var_flt> TFlt;
typedef TVar<var_str> TStr;
typedef TVar<var_lst> TLst;
typedef TVar<var_obj> TObj;
typedef TVar<var_bfn> TBfn;


// functions
bool initOperations();
void cleanupCollector();
void FreeVariables();
bool hasOperator(PVar& v, string op);
void fillArgs(var_lst& args, var_lst& dflt);
PVar evalExpr(PVar& expr, bool copy);
PVar copyVar(PVar& v);
const char* typeName(uint8_t t);

string toStr(PVar& v, uint8_t type = T_NIL);
string toStr(var_chr& v, uint8_t type = T_NIL);
string toStr(var_int& v, uint8_t type = T_INT);
string toStr(var_flt& v, uint8_t type = T_FLT);
string toStr(var_str& v, uint8_t type = T_STR);
string toStr(var_lst& v, uint8_t type = T_LST);
string toStr(var_obj& v, uint8_t type = T_OBJ);
string toStr(var_bfn& v, uint8_t type = T_BFN);

// extern functions and variables
extern PVar handleLine(var_lst& line); // execute code line
extern FuncMapOpr operations[TCNT];    // type operators
extern uint8_t KWType[CCNT - KCNT];    // keyword types
extern uint8_t VAR_Type[CCNT];         // base type of types
extern list<PVar> collector;           // garbage collector
extern uint8_t status;                 // current program status
extern PVar funcResult;                // latest returned value


// Variable interface class
class IVar {
  public:
    uint32_t refcnt = 0;    // reference counter
    uint8_t type    = 0;    // pointer to value type
    bool isConst    = true; // is constant bool
    void* pval      = NULL; // pointer to value
    // PVar ret        = NULL;  // return value when used (ie. functions)
    list<PVar>::iterator it; // iterator from collector

    IVar();
    virtual ~IVar();

    virtual string toString(bool escape = false) {
        return "null";
    }
};


// Variable class
template <typename T>
class TVar : public IVar {
  public:
    T value;

    TVar(T v_value, uint8_t v_type, bool v_isConst = false);
    ~TVar();

    string toString(bool escape = false);
};


template <typename T>
inline string TVar<T>::toString(bool) {
    return toStr(value, type);
}

template <>
inline string TVar<var_str>::toString(bool escapeStr) {
    if (escapeStr && type == T_STR)
        return "\"" + escape(value) + "\"";
    else
        return value;
}


// inline functions

inline PVar incRef(PVar v) {
    // INFO("incRef %p %s %i", v, getTypeName(v), v->refcnt + 1);
    ++v->refcnt;
    return v;
}

inline PVar decRef(PVar v) {
    // INFO("decRef %p %s %i", v, getTypeName(v), v->refcnt - 1);
    if (v->refcnt <= 0) error("refcnt=%i", v->refcnt - 1), getchar();
    if (!--v->refcnt) delete v;
    return v;
}

#define assertT(var, type) _assertT(var, type, __func__, __FILE__, __LINE__)

inline PVar _assertT(
    PVar v, uint8_t type, const char* func, const char* file, uint line) {
    if (getType(v) != type)
        error_exit(
            "error %s%s line %4i %s:\n"
            "incompatible types: expected %s got %s",
            file, "                    " + strlen(file), line, func,
            getTypeName(v), typeName(type));
    return v;
}

#endif //_VARIABLES_HPP_
