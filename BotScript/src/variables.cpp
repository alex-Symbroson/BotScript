
#define CUSTOM_BEGIN
#include "variables.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

#define FUNCTION (var_lst v)->PVar
unordered_map<uint8_t, FuncMap> operations;

void initOperations() {
    operations = {{T_INT,
                   {{"add",
                     [] FUNCTION {
                         getInt(v[0]) += getInt(v[1]);
                         return v[0];
                     }},
                    {"sub",
                     [] FUNCTION {
                         getInt(v[0]) -= getInt(v[1]);
                         return v[0];
                     }},
                    {"mul",
                     [] FUNCTION {
                         getInt(v[0]) *= getInt(v[1]);
                         return v[0];
                     }},
                    {"div",
                     [] FUNCTION {
                         getInt(v[0]) /= getInt(v[1]);
                         return v[0];
                     }},
                    {"mod",
                     [] FUNCTION {
                         getInt(v[0]) %= getInt(v[1]);
                         return v[0];
                     }}}},

                  {T_FLT,
                   {{"add",
                     [] FUNCTION {
                         getFlt(v[0]) += getFlt(v[1]);
                         return v[0];
                     }},
                    {"sub",
                     [] FUNCTION {
                         getFlt(v[0]) -= getFlt(v[1]);
                         return v[0];
                     }},
                    {"mul",
                     [] FUNCTION {
                         getFlt(v[0]) *= getFlt(v[1]);
                         return v[0];
                     }},
                    {"div",
                     [] FUNCTION {
                         getFlt(v[0]) /= getFlt(v[1]);
                         return v[0];
                     }},
                    {"mod",
                     [] FUNCTION {
                         var_flt *a = getFltP(v[0]), b = getFlt(v[1]);
                         *a = (*a - b * floor(*a / b));
                         return v[0];
                     }}}},

                  {T_STR,
                   {{"add",
                     [] FUNCTION {
                         getStr(v[0]) += getStr(v[1]);
                         return v[0];
                     }}}},

                  {T_BFN,
                   {{"call",
                     [] FUNCTION {
                         DEBUG(
                             "called %s%s", v[0]->toStr().c_str(),
                             v[1]->toStr().c_str());

                         if (getType(v[1]) == T_ARGS)
                             return callBuiltin(getBfn(v[0]), getLst(v[1]));
                         else
                             return callBuiltin(getBfn(v[0]), {v[1]});
                     }}}},

                  {T_LST, {}},

                  {T_OBJ, {}},

                  {T_PIN, {}},

                  {T_FNC, {}}};
}

uint8_t VAR_Type[] = {T_INT, T_STR, T_INT, T_FLT, T_STR, T_LST, T_OBJ,
                      T_INT, T_LST, T_LST, T_STR, T_LST, T_BFN};

// returns the name of the type of a Variable
const char* typeName(uint8_t t) {
    BEGIN();
    END();
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
        case T_OPR: return "operator";
        case T_ARGS: return "arguments";
        case T_BFN: return "builtin_function";
        default: return "undefined";
    }
}

forward_list<PVar> IVar::collector = {};

IVar::IVar() {
    // BEGIN();
    // END("-> %p", this);
}

IVar::~IVar() {
    // BEGIN("%p ~IVar<%s>", this, typeName(getType(this)));
    // END();
}

#define TypeClassDef(TMPL, TYPE, TYPEID)                            \
                                                                    \
    template <TMPL>                                                 \
    TVar<TYPE>::TVar(TYPE v, uint8_t typeID) {                      \
        /* BEGIN(); */                                              \
                                                                    \
        value = v;                                                  \
        if (typeID) {                                               \
            if (VAR_Type[typeID] != VAR_Type[TYPEID]) {             \
                END();                                              \
                error_exit(                                         \
                    "cannot assign %s of type %s to %s of type %s", \
                    typeName(typeID), typeName(VAR_Type[typeID]),   \
                    typeName(TYPEID), typeName(VAR_Type[TYPEID]));  \
            }                                                       \
            type = typeID;                                          \
        } else                                                      \
            type = TYPEID;                                          \
                                                                    \
        ptype = &type;                                              \
        pop = op = &operations[TYPEID];                             \
                                                                    \
        /*END("-> %p : TVar<%s> %s", this, typeName(this->type),    \
            this->toStr().c_str());*/                               \
    }                                                               \
                                                                    \
    template <TMPL>                                                 \
    TVar<TYPE>::~TVar() {                                           \
        /*BEGIN(                                                    \
            "%p ~TVar<%s> %s", this, typeName(this->type),          \
            this->toStr().c_str());*/                               \
                                                                    \
        collector.remove(this);                                     \
        /*END();*/                                                  \
    }

TypeClassDef(typename T, T, T_NIL);
TypeClassDef(, var_nil, T_NIL);
TypeClassDef(, var_int, T_INT);
TypeClassDef(, var_flt, T_FLT);
TypeClassDef(, var_str, T_STR);
TypeClassDef(, var_lst, T_LST);
TypeClassDef(, var_obj, T_OBJ);
TypeClassDef(, var_bfn, T_BFN);

void FreeVariables() {
    BEGIN();
    uint32_t n = 0;
    while (!IVar::collector.empty()) {
        delete IVar::collector.front();
        n++;
    }
    INFO("freed %i variables", n);
    /*
    printf(
        "sizes:\n"
        "nil:%lu\nint:%lu\nflt:%lu\nstr:%lu\nlst:%lu\nobj:%lu\nbfn:%lu\n"
        "IVar:%lu\n",
        sizeof(TNil), sizeof(TInt), sizeof(TFlt), sizeof(TStr), sizeof(TLst),
        sizeof(TObj), sizeof(TBfn) - sizeof(TBltFnc*) + sizeof(TBltFnc),
        sizeof(IVar));
        */
    END();
}
