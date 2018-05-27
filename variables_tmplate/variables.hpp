
#ifndef _VARIABLES_HPP_
#define _VARIABLES_HPP_

#include "headers.hpp"

#define uint8_t unsigned char

// type id's
#define T_NIL 0
#define T_VAR 1
#define T_INT 2
#define T_FLT 3
#define T_STR 4
#define T_LST 5
#define T_OBJ 6
#define T_PIN 7
#define T_TRM 8
#define T_FNC 9

class IVar;
template <typename> class TVar;
typedef IVar* PVar;

// types

#define var_int long
#define var_flt double
#define var_str string
#define var_lst vector<PVar>
#define var_obj unordered_map<string, PVar>

typedef TVar<var_int> TInt;
typedef TVar<var_flt> TFlt;
typedef TVar<var_str> TStr;
typedef TVar<var_lst> TLst;
typedef TVar<var_obj> TObj;

// macros for getting var instance values
//-> prevents bigger code by defining functions
#define callP(v, n, ...) \
    ONCE(static PVar _args_##v[] = {__VA_ARGS__}; (*v->pop)[n](_args_##v);)
#define callT(v, n, ...) \
    ONCE(static PVar _args_##v[] = {__VA_ARGS__}; (*v.op)[n](_args_##v);)
#define getType(var) (*var->ptype)
#define getIntP(var) ((var_int*)var->getPtr())
#define getInt(var) (*(var_int*)var->getPtr())
#define getFltP(var) ((var_flt*)var->getPtr())
#define getFlt(var) (*(var_flt*)var->getPtr())
#define getStrP(var) ((var_str*)var->getPtr())
#define getStr(var) (*(var_str*)var->getPtr())
#define getLstP(var) ((var_lst*)var->getPtr())
#define getLst(var) (*(var_lst*)var->getPtr())
#define getObjP(var) ((var_obj*)var->getPtr())
#define getObj(var) (*(var_obj*)var->getPtr())

class IVar {
  public:
    const uint8_t* ptype;
    unordered_map<string, function<void(PVar[])>>* pop;

    IVar();
    ~IVar();

    virtual void* getPtr() {
        return NULL;
    };

    virtual string toStr() {
        return "null";
    };
};

template <typename T> class TVar : public IVar {
  public:
    static const uint8_t type;
    static unordered_map<string, function<void(PVar[])>>* op;

    T value;

    TVar(T);
    ~TVar();

    PVar getVar();

    void* getPtr();
    string toStr();
};

template <typename T> void* TVar<T>::getPtr() {
    return (void*)&value;
}

template <typename T> inline PVar TVar<T>::getVar() {
    printf("getVar T\n");
    return dynamic_cast<PVar>(this);
}
/*
template <> inline PVar TVar<var_str>::getVar() {
    printf("getVar str %s\n", this->value.c_str());
    return dynamic_cast<PVar>(this);
}*/

/*
TVar::toStr()
*/

template <> inline string TVar<var_int>::toStr() {
    return to_string(value);
};

template <> inline string TVar<var_flt>::toStr() {
    return to_string(value);
};

template <> inline string TVar<var_str>::toStr() {
    return "\"" + value + "\"";
};

template <> inline string TVar<var_lst>::toStr() {
    string result = "[";
    var_lst::iterator it, end = value.end() - 1;

    for (it = value.begin(); it != end; it++) {
        result += (*it)->toStr() + ",";
    }
    result[result.size() - 1] = ']';
    return result;
};

template <> inline string TVar<var_obj>::toStr() {
    string result = "[";
    var_obj::iterator it, end = value.end();

    for (it = value.begin(); it != end; it++) {
        result += it->first + ": " + it->second->toStr() + ",";
    }
    result[result.size() - 1] = ']';
    return result;
};

void FreeVariables();
#endif //_VARIABLES_HPP_
