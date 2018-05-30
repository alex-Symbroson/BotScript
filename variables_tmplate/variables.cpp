
//$ g++ -std=c++11 -O3 -c Variables.cpp

#include "variables.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

static unordered_map<uint8_t, FuncMap> operations = {
    {T_INT,
     {{"add", [](PVar v[2]) { getInt(v[0]) += getInt(v[1]); }},
      {"sub", [](PVar v[2]) { getInt(v[0]) -= getInt(v[1]); }},
      {"mul", [](PVar v[2]) { getInt(v[0]) *= getInt(v[1]); }},
      {"div", [](PVar v[2]) { getInt(v[0]) /= getInt(v[1]); }},
      {"mod", [](PVar v[2]) { getInt(v[0]) %= getInt(v[1]); }}}},

    {T_FLT,
     {{"add", [](PVar v[2]) { getFlt(v[0]) += getFlt(v[1]); }},
      {"sub", [](PVar v[2]) { getFlt(v[0]) -= getFlt(v[1]); }},
      {"mul", [](PVar v[2]) { getFlt(v[0]) *= getFlt(v[1]); }},
      {"div", [](PVar v[2]) { getFlt(v[0]) /= getFlt(v[1]); }},
      {"mod",
       [](PVar v[2]) {
           var_flt *a = getFltP(v[0]), b = getFlt(v[1]);
           *a = (*a - b * floor(*a / b));
       }}}},

    {T_STR,
     {{"add",
       [](PVar v[2]) {
           printf("str.add\n");
           printf(" (%i,%i)\n", getType(v[0]), getType(v[1]));
           printf(" (%s,%s)\n", getStr(v[0]).c_str(), getStr(v[1]).c_str());

           getStr(v[0]) += getStr(v[1]);
           printf("~str.add\n");
       }}}},

    {T_LST, {}},

    {T_OBJ, {}},

    {T_PIN, {}},

    {T_FNC, {}}};

static int VAR_Type[] = {T_NIL, T_STR, T_INT, T_FLT, T_STR,
                         T_LST, T_OBJ, T_INT, T_LST, T_LST};

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
    DEBUG("%p IVar<>::IVar()", this);
}

IVar::~IVar() {
    DEBUG("%p ~IVar<%s>::IVar()", this, typeName(getType(this)));
}

#define TypeClassDef(tmpl, Type, TypeID)                                           \
                                                                                   \
    template <tmpl> TVar<Type>::TVar(Type v, uint8_t typeID) {                     \
        value = v;                                                                 \
        if (typeID) {                                                              \
            if (VAR_Type[typeID] != VAR_Type[TypeID])                              \
                error_exit(                                                        \
                    "cannot assign %s to %s", typeName(typeID), typeName(TypeID)); \
                                                                                   \
            type = typeID;                                                         \
        } else                                                                     \
            type = TypeID;                                                         \
                                                                                   \
        ptype = &type;                                                             \
        pop = op = &operations[TypeID];                                            \
        DEBUG(                                                                     \
            "%p TVar<%s>::TVar(%s)", this, typeName(this->type),                   \
            this->toStr().c_str());                                                \
    }                                                                              \
                                                                                   \
    template <tmpl> TVar<Type>::~TVar() {                                          \
        DEBUG(                                                                     \
            "%p ~TVar<%s>::TVar(%s)", this, typeName(this->type),                  \
            this->toStr().c_str());                                                \
                                                                                   \
        collector.remove(this);                                                    \
    }

TypeClassDef(typename T, T, T_NIL);
TypeClassDef(, var_int, T_INT);
TypeClassDef(, var_flt, T_FLT);
TypeClassDef(, var_str, T_STR);
TypeClassDef(, var_lst, T_LST);
TypeClassDef(, var_obj, T_OBJ);

void FreeVariables() {
    INFO("freeing garbage");
    while (!IVar::collector.empty()) delete IVar::collector.front();
}
