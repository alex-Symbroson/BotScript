
#define CUSTOM_BEGIN
#include "variables.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

#define FUNCTION (PVar a, PVar b)->PVar
unordered_map<uint8_t, FuncMapOp> operations;

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
            TOSTR(this));*/                                         \
    }                                                               \
                                                                    \
    template <TMPL>                                                 \
    TVar<TYPE>::~TVar() {                                           \
        /*BEGIN(                                                    \
            "%p ~TVar<%s> %s", this, typeName(this->type),          \
            TOSTR(this));*/                                         \
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

// clang-format off

void initOperations() {
    BEGIN();
    operations = {
        {T_INT, {
            {"add", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TInt(getInt(a) + getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getInt(a) + getFlt(b)));
                    default   : err_iop("+", a, b);
                }
            }},
            {"sub", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TInt(getInt(a) - getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getInt(a) - getFlt(b)));
                    default   : err_iop("-", a, b);
                }
            }},
            {"mul", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TInt(getInt(a) * getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getInt(a) * getFlt(b)));
                    default   : err_iop("*", a, b);
                }
            }},
            {"div", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TInt(getInt(a) / getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getInt(a) / getFlt(b)));
                    default   : err_iop("/", a, b);
                }
            }},
            {"mod", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TInt(getInt(a) % getInt(b)));
                    case T_FLT: {
                        var_flt c = getInt(a), d = getFlt(b);
                        return NEWVAR(TFlt(c - d * floor(c / d)));
                    }
                    default   : err_iop("%", a, b);
                }
            }}
        }},

        {T_FLT,{
            {"add", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TFlt(getFlt(a) + getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getFlt(a) + getFlt(b)));
                    default   : err_iop("+", a, b);
                }
            }},
            {"sub", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TFlt(getFlt(a) - getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getFlt(a) - getFlt(b)));
                    default   : err_iop("-", a, b);
                }
            }},
            {"mul", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TFlt(getFlt(a) * getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getFlt(a) * getFlt(b)));
                    default   : err_iop("*", a, b);
                }
            }},
            {"div", [] FUNCTION {
                switch(getType(b)) {
                    case T_INT: return NEWVAR(TFlt(getFlt(a) / getInt(b)));
                    case T_FLT: return NEWVAR(TFlt(getFlt(a) / getFlt(b)));
                    default   : err_iop("/", a, b);
                }
            }},
            {"mod", [] FUNCTION {
                var_flt c = getFlt(a), d;
                switch(getType(b)) {
                    case T_INT: d = getInt(b);
                    case T_FLT: d = getFlt(b);
                    default   : err_iop("%", a, b);
                }
                return NEWVAR(TFlt(c - d * floor(c / d)));
            }}
        }},

        {T_STR,{
            {"add", [] FUNCTION {
                if(getType(b) == T_STR)
                    return NEWVAR(TStr(getStr(a) + getStr(b)));
                else err_iop("+", a, b);
            }}
        }},

        {T_BFN,{
            {"call", [] FUNCTION {
                if (getType(b) == T_ARGS)
                    return callBuiltin(getBfn(a), getLst(b));
                else err_iop("call", a, b);
                //return callBuiltin(getBfn(a), {b});
            }}}},

        {T_LST,{
            {"push", [] FUNCTION {
                getLst(a).push_back(b);
                return NEWVAR(TInt(getLst(a).size()));
            }},
            {"pop", [] FUNCTION {
                PVar ret = getLst(a).back();
                getLst(a).pop_back();
                return ret;
            }},
            {"at", [] FUNCTION {
                if(getType(b) == T_INT) {
                    var_int i = getInt(b);
                    uint32_t len = getLst(a).size();
                    if(i < 0) i += len;

                    if(i < 0 || i >= len)
                        err_rng(a, b);
                    else
                        return getLst(a)[i];
                } else
                    err_iop("at", a, b);
            }}}},

        {T_OBJ, {}},

        {T_PIN, {}},

        {T_FNC, {}}
    };
    END();
}

// clang-format on

PVar evalExpr(PVar& expr) {
    if (getType(expr) == T_TRM)
        return handleLine(getLst(expr));
    else if (getType(expr) == T_LST) {
        var_lst lst;
        for (PVar& v: getLst(expr))
            lst.push_back(evalExpr(v));
        return NEWVAR(TLst(lst));
    } else if (getType(expr) == T_LST) {
        var_obj obj;
        for (auto& v: getObj(expr))
            obj[v.first] = evalExpr(v.second);
        return NEWVAR(TObj(obj));
    } else
        return expr;
}

bool hasOperator(PVar& v, char o) {
    BEGIN();
    if (isOperator(o)) {
        string op;
        op += o;
        op = operators[op].name;
        END();
        return operations[getType(v)].find(op) != operations[getType(v)].end();
    }
    END();
    return false;
}

bool hasOperator(PVar& v, string op) {
    BEGIN();
    END();
    if (isOperator(op)) op = operators[op].name;
    return operations[getType(v)].find(op) != operations[getType(v)].end();
}

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
