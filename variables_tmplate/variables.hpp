
#ifndef _VARIABLES_HPP_
#define _VARIABLES_HPP_

#define CUSTOM_BEGIN
#include "headers.hpp"
#include "macros.hpp"

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
#define TCNT 10

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

typedef unordered_map<string, function<void(PVar[])>> FuncMap;

void FreeVariables();
const char* typeName(uint8_t t);

// macros for getting var instance values
//-> prevents bigger code by defining functions
#define callP(v, n, ...) \
    ONCE(static PVar _args_##v[] = {__VA_ARGS__}; (*(v)->pop)[n](_args_##v);)
#define callT(v, n, ...) \
    ONCE(static PVar _args_##v[] = {__VA_ARGS__}; (*(v).op)[n](_args_##v);)
#define getType(var) (*(var)->ptype)
#define getIntP(var) ((var_int*)(var)->getPtr())
#define getInt(var) (*(var_int*)(var)->getPtr())
#define getFltP(var) ((var_flt*)(var)->getPtr())
#define getFlt(var) (*(var_flt*)(var)->getPtr())
#define getStrP(var) ((var_str*)(var)->getPtr())
#define getStr(var) (*(var_str*)(var)->getPtr())
#define getLstP(var) ((var_lst*)(var)->getPtr())
#define getLst(var) (*(var_lst*)(var)->getPtr())
#define getObjP(var) ((var_obj*)(var)->getPtr())
#define getObj(var) (*(var_obj*)(var)->getPtr())

class IVar {
  public:
    uint8_t* ptype;
    FuncMap* pop;

    static forward_list<PVar> collector; // garbage collector

    IVar();
    virtual ~IVar();

    virtual void* getPtr() {
        return NULL;
    };

    virtual string toStr() {
        return "null";
    };
};

template <typename T> class TVar : public IVar {
  public:
    uint8_t type;
    FuncMap* op;

    T value;

    TVar(T, uint8_t = 0);
    ~TVar();
    void* operator new(size_t size);
    void operator delete(void* p);

    PVar getVar();

    void* getPtr();
    string toStr();
};

template <typename T> void* TVar<T>::operator new(size_t size) {
    TVar<T>* p = (TVar<T>*)malloc(size);
    DEBUG("%p TVar<>::op new()", p);
    IVar::collector.push_front(p->getVar());
    return p;
}

template <typename T> void TVar<T>::operator delete(void* p) {
    DEBUG("%p TVar<>::op delete()", p);
    free(p);
}

template <typename T> void* TVar<T>::getPtr() {
    return (void*)&value;
}

template <typename T> PVar TVar<T>::getVar() {
    return dynamic_cast<PVar>(this);
}

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
    var_lst::iterator it, end = value.end();

    for (it = value.begin(); it != end; it++) {
        result += (*it)->toStr() + ",";
    }

    if (value.size())
        result[result.size() - 1] = ']';
    else
        result += "]";
    return result;
};

template <> inline string TVar<var_obj>::toStr() {
    string result = "]";
    var_obj::iterator it, end = value.end();

    // builds string reversed because every new unordered_map
    // element is inserted at the front
    for (it = value.begin(); it != end; it++) {
        result = ",\"" + it->first + "\":" + it->second->toStr() + result;
    }

    if (value.size())
        result[0] = '[';
    else
        result += "]";
    return result;
};

#endif //_VARIABLES_HPP_
