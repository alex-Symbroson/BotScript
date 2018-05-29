
//$ g++ -std=c++11 -O3 -c Variables.cpp

#include "variables.hpp"

#define CUSTOM_BEGIN
#include "macros.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

// returns the name of the type of a Variable
const char* typeName(uint8_t t) {
    switch (t) {
        case T_NIL: return "null";
        case T_VAR: return "var";
        case T_INT: return "integer";
        case T_FLT: return "float";
        case T_STR: return "string";
        case T_LST: return "list";
        case T_OBJ: return "object";
        case T_PIN: return "pin";
        case T_TRM: return "term";
        case T_FNC: return "function";
        default: return "undefined";
    }
}

forward_list<PVar> IVar::collector = {};

IVar::IVar() {
    printf("%p IVar<>::IVar()\n", this);
}

IVar::~IVar() {
    printf("%p ~IVar<%s>::IVar()\n", this, typeName(getType(this)));
}

#define TypeClassDef(tmpl, Type, TypeID)                            \
    template <tmpl> const uint8_t TVar<Type>::type = TypeID;        \
                                                                    \
    template <tmpl> TVar<Type>::TVar(Type v) {                      \
        value = v;                                                  \
        ptype = &type;                                              \
        pop   = op;                                                 \
        printf(                                                     \
            "%p TVar<%s>::TVar(%s)\n", this, typeName(this->type),  \
            this->toStr().c_str());                                 \
    }                                                               \
                                                                    \
    template <tmpl> TVar<Type>::~TVar() {                           \
        printf(                                                     \
            "%p ~TVar<%s>::TVar(%s)\n", this, typeName(this->type), \
            this->toStr().c_str());                                 \
                                                                    \
        collector.remove(this);                                     \
    }

TypeClassDef(typename T, T, T_NIL);
TypeClassDef(, var_int, T_INT);
TypeClassDef(, var_flt, T_FLT);
TypeClassDef(, var_str, T_STR);
TypeClassDef(, var_lst, T_LST);
TypeClassDef(, var_obj, T_OBJ);

#define TypeClassOp(tBas, ...)                                            \
    template <>                                                           \
    unordered_map<string, function<void(PVar[])>>* TVar<var_##tBas>::op = \
        new unordered_map<string, function<void(PVar[])>>(__VA_ARGS__)

TypeClassOp(
    int, {{"add", [](PVar v[2]) { getInt(v[0]) += getInt(v[1]); }},
          {"sub", [](PVar v[2]) { getInt(v[0]) -= getInt(v[1]); }},
          {"mul", [](PVar v[2]) { getInt(v[0]) *= getInt(v[1]); }},
          {"div", [](PVar v[2]) { getInt(v[0]) /= getInt(v[1]); }},
          {"mod", [](PVar v[2]) { getInt(v[0]) %= getInt(v[1]); }}});

TypeClassOp(
    flt, {{"add", [](PVar v[2]) { getFlt(v[0]) += getFlt(v[1]); }},
          {"sub", [](PVar v[2]) { getFlt(v[0]) -= getFlt(v[1]); }},
          {"mul", [](PVar v[2]) { getFlt(v[0]) *= getFlt(v[1]); }},
          {"div", [](PVar v[2]) { getFlt(v[0]) /= getFlt(v[1]); }},
          {"mod", [](PVar v[2]) {
               var_flt *a = getFltP(v[0]), b = getFlt(v[1]);
               *a = (*a - b * floor(*a / b));
           }}});

TypeClassOp(str, {{"add", [](PVar v[2]) {
                       printf("str.add\n");
                       printf(" (%i,%i)\n", getType(v[0]), getType(v[1]));
                       printf(
                           " (%s,%s)\n", getStr(v[0]).c_str(), getStr(v[1]).c_str());

                       getStr(v[0]) += getStr(v[1]);
                       printf("~str.add\n");
                   }}});

TypeClassOp(lst, {});

TypeClassOp(obj, {});

#define ReleaseTypeClassOp(tBas)   \
    TVar<var_##tBas>::op->clear(); \
    delete TVar<var_##tBas>::op

void FreeVariables() {
    printf("freeing variable operations\n");
    ReleaseTypeClassOp(int);
    ReleaseTypeClassOp(flt);
    ReleaseTypeClassOp(str);
    ReleaseTypeClassOp(lst);
    ReleaseTypeClassOp(obj);
    // ReleaseTypeClassOp(pin);

    printf("freeing garbage\n");
    while (!IVar::collector.empty()) delete IVar::collector.front();
}
