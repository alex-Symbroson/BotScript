
//$ g++ -std=c++11 -O3 -c Variables.cpp

#include "variables.hpp"

#define CUSTOM_BEGIN
#include "macros.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

IVar::IVar() {}
IVar::~IVar() {}

#define TypeClassDef(tmpl, Type, TypeID)                                    \
    template <tmpl> const uint8_t TVar<Type>::type = TypeID;                \
                                                                            \
    template <tmpl> TVar<Type>::TVar(Type v) {                              \
        printf("TVar<%i>::TVar(%s)\n", this->type, this->toStr().c_str());  \
        value = v;                                                          \
        ptype = &type;                                                      \
        pop   = op;                                                         \
    }                                                                       \
                                                                            \
    template <tmpl> TVar<Type>::~TVar() {                                   \
        printf("~TVar<%i>::TVar(%s)\n", this->type, this->toStr().c_str()); \
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
                       printf("(%i,)\n", getType(v[1]));
                       getStr(v[0]) += getStr(v[1]);
                       printf("~str.add\n");
                   }}});

TypeClassOp(lst, {});

TypeClassOp(obj, {});

#define ReleaseTypeClassOp(tBas)   \
    TVar<var_##tBas>::op->clear(); \
    delete TVar<var_##tBas>::op

void FreeVariables() {
    ReleaseTypeClassOp(int);
    ReleaseTypeClassOp(flt);
    ReleaseTypeClassOp(str);
    ReleaseTypeClassOp(lst);
    ReleaseTypeClassOp(obj);
    // ReleaseTypeClassOp(pin);
}
