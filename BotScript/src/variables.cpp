
#define CUSTOM_BEGIN
#include "variables.hpp"

#include "interpret.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

#define FUNCTION (PVar a, PVar b)->PVar
FuncMapOpr operations[TCNT];

uint8_t VAR_Type[CCNT] = {T_INT, T_INT, T_INT, T_FLT, T_STR, T_LST,
                          T_OBJ, T_INT, T_LST, T_LST, T_STR, T_LST,
                          T_BFN, T_STR, T_NIL, T_INT, T_INT, T_LST,
                          T_LST, T_LST, T_LST, T_LST, T_LST};

list<PVar> collector = {};

// returns the name of the type of a Variable
const char* typeName(uint8_t t) {
    // BEGIN();
    // END();
    switch (t) {
        case T_NIL: return "null";
        // case T_VAR: return "var";
        case T_BIN: return "boolean";
        case T_INT: return "integer";
        case T_FLT: return "float";
        case T_STR: return "string";
        case T_LST: return "list";
        case T_OBJ: return "object";
        case T_PIN: return "pin";
        case T_TRM: return "term";
        case T_FNC: return "function";
        case T_OPR: return "operator";
        case T_ARG: return "arguments";
        case T_BFN: return "builtin_function";
        case T_MFN: return "member_function";

        case K_NIL: return "null";
        case K_TRU: return "true";
        case K_FLS: return "false";

        case C_CIF: return "if";
        case C_EIF: return "elif";
        case C_ELS: return "else";
        case C_CDO: return "do";
        case C_WHL: return "while";
        case C_UNT: return "until";

        default:
            error_exit("unknown type id %i - report", t);
            return "undefined";
    }
}

IVar::IVar() {
    // BEGIN();
    collector.push_front(this);
    this->it = collector.begin();
    // END("-> %p", this);
}

IVar::~IVar() {
    // BEGIN("%p ~IVar<%s>", this, typeName(getType(this)));
    collector.erase(this->it);
    // END();
}

#define TypeClassDef(TMPL, TYPE, TYPEID)                                      \
                                                                              \
    template <TMPL>                                                           \
    TVar<TYPE>::TVar(TYPE v, uint8_t typeID, bool Const) {                    \
        /* BEGIN(); */                                                        \
                                                                              \
        if (typeID) {                                                         \
            if (baseType(typeID) != baseType(TYPEID)) {                       \
                error_exit(                                                   \
                    "cannot assign %s of type %s to %s of type %s",           \
                    typeName(typeID), baseTypeName(typeID), typeName(TYPEID), \
                    baseTypeName(TYPEID));                                    \
            }                                                                 \
            type = typeID;                                                    \
        } else                                                                \
            type = TYPEID;                                                    \
                                                                              \
        value   = v;                                                          \
        isConst = Const;                                                      \
        pval    = &value;                                                     \
        ptype   = &type;                                                      \
                                                                              \
        /*END("-> %p : TVar<%s> %s", this, typeName(this->type),              \
            TOSTR(this));*/                                                   \
    }                                                                         \
                                                                              \
    template <TMPL>                                                           \
    TVar<TYPE>::~TVar() {                                                     \
        /*BEGIN(                                                              \
            "%p ~TVar<%s> %s", this, typeName(this->type),                    \
            TOSTR(this));*/                                                   \
                                                                              \
        /*END();*/                                                            \
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
    operations[T_INT] = {
        {"equal", [] FUNCTION {
            switch(getType(b)) {
                case T_INT: return NEWVAR(TInt(getInt(a) == getInt(b), T_BIN));
                case T_FLT: return NEWVAR(TInt(getInt(a) == getFlt(b), T_BIN));
                default   : return NEWVAR(TInt(0, T_BIN));
            }
        }},
        {"nequal", [] FUNCTION {
            switch(getType(b)) {
                case T_INT: return NEWVAR(TInt(getInt(a) != getInt(b), T_BIN));
                case T_FLT: return NEWVAR(TInt(getInt(a) != getFlt(b), T_BIN));
                default   : return NEWVAR(TInt(1, T_BIN));
            }
        }},
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
        }},
        {"toStr", [] FUNCTION {
            var_lst args = getLst(b);

            if(!args.empty()) {
                EVALARGS(args, {});
                if(getType(args[0]) == T_INT)
                    return NEWVAR(TStr(dtos2(getInt(a), getInt(args[0]))));
                else
                    err_iat(a, args[0], "toString", T_INT);
            } else
                return NEWVAR(TStr(dtos2(getInt(a), 10)));
        }}
    },

    operations[T_FLT] = {
        {"equal", [] FUNCTION {
            switch(getType(b)) {
                case T_INT: return NEWVAR(TInt(getFlt(a) == getInt(b), T_BIN));
                case T_FLT: return NEWVAR(TInt(getFlt(a) == getFlt(b), T_BIN));
                default   : return NEWVAR(TInt(0, T_BIN));
            }
        }},
        {"nequal", [] FUNCTION {
            switch(getType(b)) {
                case T_INT: return NEWVAR(TInt(getFlt(a) != getInt(b), T_BIN));
                case T_FLT: return NEWVAR(TInt(getFlt(a) != getFlt(b), T_BIN));
                default   : return NEWVAR(TInt(1, T_BIN));
            }
        }},
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
        }},
        {"toStr", [] FUNCTION {
            var_lst args = getLst(b);

            if(!args.empty()) {
                EVALARGS(args, {});
                if(getType(args[0]) == T_INT)
                    return NEWVAR(TStr(dtos2(getFlt(a), getInt(args[0]))));
                else
                    err_iat(a, args[0], "toString", T_INT);
            } else
                return NEWVAR(TStr(dtos2(getFlt(a), 10)));
        }}
    };

    operations[T_STR] = {
        {"equal", [] FUNCTION {
            if(getType(b) == T_STR)
                return NEWVAR(TInt(getStr(a) == getStr(b), T_BIN));
            else
                return NEWVAR(TInt(0, T_BIN));
        }},
        {"nequal", [] FUNCTION {
            if(getType(b) == T_STR)
                return NEWVAR(TInt(getStr(a) != getStr(b), T_BIN));
            else
                return NEWVAR(TInt(1, T_BIN));
        }},
        {"add", [] FUNCTION {
            if(getType(b) == T_STR)
                return NEWVAR(TStr(getStr(a) + getStr(b)));
            else err_iop("+", a, b);
        }},
        {"toInt", [] FUNCTION {
            string s = getStr(a);
            int32_t pos = s.find('.');
            if(pos > -1) s[pos] = 0;
            return NEWVAR(TInt(stod2(s)));
        }},
        {"toFlt", [] FUNCTION {
            return NEWVAR(TFlt(stod2(getStr(a))));
        }}
    };

    operations[T_BFN] = {
        {"call", [] FUNCTION {
            BEGIN("%s call %s", TOSTR(a), TOSTR(b));

            if (getType(b) == T_ARG) {
                PVar res = getBfn(a)->func(getLst(b));
                END("%s", TOSTR(res));
                return res;
            } else
                err_iop("call", a, b);
        }}
    };

    operations[T_LST] = {
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
        }},
        {"join", [] FUNCTION {
            var_lst args = getLst(b);

            if(!args.empty()) {
                EVALARGS(args, {});
                string result = "", sep = "";

                if(getType(args[0]) == T_STR) sep = getStr(args[0]);
                else err_iat(a, args[0], "join", T_STR);

                for (PVar& v: getLst(a)) result += v->toStr() + sep;

                return NEWVAR(TStr(result));
            } else
                err_iac(a, args, "join", 1);
        }}
    };

    END();
}

// clang-format on

PVar evalExpr(PVar& expr) {
    switch (getType(expr)) {
        case T_TRM: return handleLine(getLst(expr));
        case T_LST: {
            var_lst lst;
            for (PVar& v: getLst(expr)) lst.push_back(evalExpr(v));
            return NEWVAR(TLst(lst));
        }
        case T_OBJ: {
            var_obj obj;
            for (auto& v: getObj(expr)) obj[v.first] = evalExpr(v.second);
            return NEWVAR(TObj(obj));
        }
        default: return expr;
    }
}

void setDefault(var_lst& args, const var_lst& dflt) {
    int16_t i = 0, len = args.size();
    var_lst fargs(len);

    // evaluate arguments
    for (; i < len; i++) fargs[i] = evalExpr(args[i]);

    // fill arguments
    i = dflt.size();
    while (i-- > len) fargs.push_back(dflt[i]);

    args = fargs;
}

bool hasOperator(PVar& v, string o) {
    BEGIN("v=%s, op=%s", getTypeName(v), o.c_str());
    auto opr = operations[getType(v)];
    if (opr.find(o) != opr.end()) {
        END("-> %i", 1);
        return true;
    } else {
        auto pop = operators.find(o);
        if (pop != operators.end()) {
            o = pop->second.name;
            END("-> %i", opr.find(o) != opr.end());
            return opr.find(o) != opr.end();
        }
    }
    END("-> %i", 0);
    return false;
}

void cleanupCollector() {
    uint32_t n = 0;
    collector.remove_if([&n](PVar& v) {
        if (!v->refcnt) {
            n++;
            delete v;
        }
        return false;
    });
    INFO("freed %i variables", n);
}

void FreeVariables() {
    BEGIN();
    uint32_t n = 0;
    collector.remove_if([&n](PVar& v) {
        n++;
        delete v;
        return false;
    });
    INFO("freed %i variables", n);
    /*
    printf(
        "sizes:\n"
        "nil:%lu\nint:%lu\nflt:%lu\nstr:%lu\nlst:%lu\nobj:%lu\nbfn:%lu\n"
        "IVar:%lu\n",
        sizeof(TNil), sizeof(TInt), sizeof(TFlt), sizeof(TStr),
    sizeof(TLst), sizeof(TObj), sizeof(TBfn) - sizeof(TBltFunc*) +
    sizeof(TBltFunc), sizeof(IVar));
        */
    END();
}
