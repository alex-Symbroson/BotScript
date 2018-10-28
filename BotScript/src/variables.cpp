
#define CUSTOM_BEGIN
#include "variables.hpp"

#include "interpret.hpp"


#define BEGIN(...) BEGIN_1("Variables::", __func__, __VA_ARGS__)
#define END(...) END_1("Variables::", __func__, __VA_ARGS__)

#define DEFOPR(NAME, ...)                                                 \
    {                                                                     \
        NAME, [](PVar a, PVar b) -> PVar { __VA_ARGS__ return newNil(); } \
    }


// clang-format off
list<PVar> collector = {};
FuncMapOpr operations[TCNT];

// base types
uint8_t VAR_Type[CCNT] = {
// T
    T_NIL, T_NIL, T_INT, T_FLT, T_STR, T_LST, T_OBJ,
    T_INT, T_LST, T_LST, T_STR, T_LST, T_BFN, T_STR,
// K
    T_NIL, T_INT, T_INT, T_NIL, T_LST,
// C
    T_LST, T_LST, T_LST, T_LST, T_LST, T_LST
};
// clang-format on


// returns the name of the type of a Variable
const char* typeName(uint8_t t) {
    switch (t) {
        case T_NIL: return "nil";
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
        case K_BRK: return "break";
        case K_RET: return "return";

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
    // INFO("append %p", this);
    collector.push_front(this);
    this->it = collector.begin();
}

IVar::~IVar() {
    // INFO("remove %p %p", this, this->it);
    if (this->refcnt && status != S_FREE)
        error(
            "freeing %p with refcount %i during execution (=%s)", this, refcnt,
            toString().c_str()),
            getchar();
    collector.erase(this->it);
}


#define TypeClassConstructor(TMPL, TYPE, TYPEID)                              \
    template <TMPL>                                                           \
    TVar<TYPE>::TVar(TYPE v, uint8_t typeID, bool Const) {                    \
        /*BEGIN();*/                                                          \
        if (typeID) {                                                         \
            if (baseType(typeID) != baseType(TYPEID)) {                       \
                error_exit(                                                   \
                    "cannot assign %s of baseType %s to %s of baseType %s",   \
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
                                                                              \
        /*INFO(                                                               \
            "create %p %s%s %s", this, isConst ? "const " : "",               \
            typeName(type), toString().c_str());  */                          \
        /*END();*/                                                            \
    }

#define TypeClassDestructor(TMPL, TYPE, TYPEID)                 \
    template <TMPL>                                             \
    TVar<TYPE>::~TVar() {                                       \
        /*INFO(                                                 \
            "delete %p %s%s %s", this, isConst ? "const " : "", \
            typeName(type), toString().c_str());*/              \
    }

TypeClassConstructor(typename T, T, T_NIL);
TypeClassConstructor(, var_chr, T_NIL);
TypeClassConstructor(, var_int, T_INT);
TypeClassConstructor(, var_flt, T_FLT);
TypeClassConstructor(, var_str, T_STR);
TypeClassConstructor(, var_lst, T_LST);
TypeClassConstructor(, var_obj, T_OBJ);
TypeClassConstructor(, var_bfn, T_BFN);

TypeClassDestructor(typename T, T, T_NIL);
TypeClassDestructor(, var_chr, T_NIL);
TypeClassDestructor(, var_int, T_INT);
TypeClassDestructor(, var_flt, T_FLT);
TypeClassDestructor(, var_str, T_STR);
TypeClassDestructor(, var_bfn, T_BFN);


template <>
TVar<var_lst>::~TVar() {
    /*INFO(
        "delete %p %s%s %s", this, isConst ? "const " : "", typeName(type),
        toString().c_str());*/


    if (status != S_FREE)
        for (PVar& v: value) decRef(v);
    // value.clear();
}

template <>
TVar<var_obj>::~TVar() {
    // INFO(
    //     "delete %p %s%s %s", this, isConst ? "const " : "", typeName(type),
    //     toString().c_str());

    if (status != S_FREE)
        for (auto& v: value) decRef(v.second);
    // value.clear();
}


// clang-format off

    // dont modify const values!
bool initOperations() {
    BEGIN();
    operations[T_INT] = {
        DEFOPR("equal", {
            switch(getType(b)) {
                case T_INT: return newBin(getInt(a) == getInt(b));
                case T_FLT: return newBin(getInt(a) == getFlt(b));
                default   : return newBin(false);
            }
        }),
        DEFOPR("nequal", {
            switch(getType(b)) {
                case T_INT: return newBin(getInt(a) != getInt(b));
                case T_FLT: return newBin(getInt(a) != getFlt(b));
                default   : return newBin(true);
            }
        }),
        DEFOPR("add", {
            switch(getType(b)) {
                case T_INT: return newInt(getInt(a) + getInt(b));
                case T_FLT: return newFlt(getInt(a) + getFlt(b));
                default   : err_iop("+", a, b);
            }
        }),
        DEFOPR("sub", {
            switch(getType(b)) {
                case T_INT: return newInt(getInt(a) - getInt(b));
                case T_FLT: return newFlt(getInt(a) - getFlt(b));
                default   : err_iop("-", a, b);
            }
        }),
        DEFOPR("mul", {
            switch(getType(b)) {
                case T_INT: return newInt(getInt(a) * getInt(b));
                case T_FLT: return newFlt(getInt(a) * getFlt(b));
                default   : err_iop("*", a, b);
            }
        }),
        DEFOPR("div", {
            switch(getType(b)) {
                case T_INT: return newInt(getInt(a) / getInt(b));
                case T_FLT: return newFlt(getInt(a) / getFlt(b));
                default   : err_iop("/", a, b);
            }
        }),
        DEFOPR("mod", {
            switch(getType(b)) {
                case T_INT: return newInt(getInt(a) % getInt(b));
                case T_FLT: {
                    var_flt c = getInt(a), d = getFlt(b);
                    return newFlt(c - d * floor(c / d));
                }
                default   : err_iop("%", a, b);
            }
        }),
        DEFOPR("toStr", {
            var_lst args = getArg(b);

            if(!args.empty()) {
                FILLARGS(args, {});
                if(getType(args[0]) == T_INT) {
                    PVar res = newStr(dtos2(getInt(a), getInt(args[0])));
                    return res;
                }
                else err_iat(a, args[0], "toString", T_INT);
            }
            else return newStr(dtos2(getInt(a), 10));
        })
    },

    operations[T_FLT] = {
        DEFOPR("equal", {
            switch(getType(b)) {
                case T_INT: return newBin(getFlt(a) == getInt(b));
                case T_FLT: return newBin(getFlt(a) == getFlt(b));
                default   : return newBin(false);
            }
        }),
        DEFOPR("nequal", {
            switch(getType(b)) {
                case T_INT: return newBin(getFlt(a) != getInt(b));
                case T_FLT: return newBin(getFlt(a) != getFlt(b));
                default   : return newBin(true);
            }
        }),
        DEFOPR("add", {
            switch(getType(b)) {
                case T_INT: return newFlt(getFlt(a) + getInt(b));
                case T_FLT: return newFlt(getFlt(a) + getFlt(b));
                default   : err_iop("+", a, b);
            }
        }),
        DEFOPR("sub", {
            switch(getType(b)) {
                case T_INT: return newFlt(getFlt(a) - getInt(b));
                case T_FLT: return newFlt(getFlt(a) - getFlt(b));
                default   : err_iop("-", a, b);
            }
        }),
        DEFOPR("mul", {
            switch(getType(b)) {
                case T_INT: return newFlt(getFlt(a) * getInt(b));
                case T_FLT: return newFlt(getFlt(a) * getFlt(b));
                default   : err_iop("*", a, b);
            }
        }),
        DEFOPR("div", {
            switch(getType(b)) {
                case T_INT: return newFlt(getFlt(a) / getInt(b));
                case T_FLT: return newFlt(getFlt(a) / getFlt(b));
                default   : err_iop("/", a, b);
            }
        }),
        DEFOPR("mod", {
            var_flt c = getFlt(a), d;
            switch(getType(b)) {
                case T_INT: d = getInt(b);
                case T_FLT: d = getFlt(b);
                default   : err_iop("%", a, b);
            }
            return newFlt(c - d * floor(c / d));
        }),
        DEFOPR("toStr", {
            var_lst args = getArg(b);

            if(!args.empty()) {
                FILLARGS(args, {});
                if(getType(args[0]) == T_INT) {
                    PVar res = newStr(dtos2(getFlt(a), getInt(args[0])));
                    return res;
                }
                else err_iat(a, args[0], "toString", T_INT);

            }
            else return newStr(dtos2(getFlt(a), 10));
        })
    };

    operations[T_STR] = {
        DEFOPR("equal", {
            if(getType(b) == T_STR)
                return newBin(getStr(a) == getStr(b));
            else
                return newBin(false);
        }),
        DEFOPR("nequal", {
            if(getType(b) == T_STR)
                return newBin(getStr(a) != getStr(b));
            else
                return newBin(true);
        }),
        DEFOPR("add", {
            if(getType(b) == T_STR)
                return newStr(getStr(a) + getStr(b));
            else err_iop("+", a, b);
        }),
        DEFOPR("toInt", {
            string s = getStr(a);
            int32_t pos = s.find('.');
            if(pos > -1) s[pos] = 0;
            return newInt(stod2(s));
        }),
        DEFOPR("toFlt", {
            return newFlt(stod2(getStr(a)));
        })
    };

    operations[T_FNC] = {
        // TODO: assign parameters
        DEFOPR("call", {
            if (getType(b) == T_ARG) {
                handleScope(getLst(a));
                if (status == S_RETURN) status = S_EXEC;
                return funcResult;
            }
            else err_iop("call", a, b);
        })
    };

    operations[T_BFN] = {
        DEFOPR("call", {
            if (getType(b) == T_ARG) {
                PVar res = getBfn(a)->func(getArg(b));
                return res;
            }
            else err_iop("call", a, b);
        })
    };

    operations[T_LST] = {
        DEFOPR("push", {
            if(a->isConst) warning("cannot \"push\" to constant list");
            else getLst(a).push_back(incRef(b));
            return newInt(getLst(a).size() + a->isConst);
        }),
        DEFOPR("pop", {
            var_lst& lst = getLst(a);
            if(a->isConst) warning("cannot \"pop\" from constant list");
            else lst.pop_back();
            PVar ret = lst.back();
            ret->refcnt--;
            return ret;
        }),
        DEFOPR("at", {
            if(getType(b) == T_INT) {
                var_int i = getInt(b);
                var_lst lst = getLst(a);
                uint32_t len = lst.size();
                if(i < 0) i += len;

                if(i < 0 || i >= len)
                    err_rng(a, b);
                else
                    return lst[i];
            }
            else err_iop("at", a, b);
        }),
        DEFOPR("join", {
            var_lst args = getArg(b);

            if(!args.empty()) {
                FILLARGS(args, {});
                string result = "", sep = "";

                if(getType(args[0]) == T_STR) sep = getStr(args[0]);
                else err_iat(a, args[0], "join", T_STR);

                PVar tmp, last = getLst(a).back();
                for (PVar& v: getLst(a)) {
                    tmp = incRef(evalExpr(v, false));
                    result += tmp->toString();
                    if(v != last) result += sep;
                    decRef(tmp);
                }

                return newStr(result);
            }
            else
                err_iac2(a, args, "join", 1);
        })
    };

    END("false");
    return false;
}

// clang-format on

PVar copyVar(PVar& var) {
    // BEGIN("%s", TOSTR(var));
    // END();
    switch (getBaseType(var)) {
        case T_NIL: return newNil();
        case T_INT: return NEWVAR(TInt(getIntRaw(var), getType(var)));
        case T_FLT: return NEWVAR(TFlt(getFltRaw(var), getType(var)));
        case T_STR: return NEWVAR(TStr(getStrRaw(var), getType(var)));
        case T_BFN: return NEWVAR(TBfn(getBfnRaw(var), getType(var)));
        case T_LST: {
            var_lst lst;
            for (PVar& v: getLstRaw(var)) {
                if (v->isConst)
                    lst.push_back(incRef(copyVar(v)));
                else
                    lst.push_back(incRef(v));
            }
            return NEWVAR(TLst(lst, getType(var)));
        }
        case T_OBJ: {
            var_obj obj;
            for (auto& v: getObjRaw(var)) {
                if (v.second->isConst)
                    obj[v.first] = incRef(copyVar(v.second));
                else
                    obj[v.first] = incRef(v.second);
            }
            return NEWVAR(TObj(obj, getType(var)));
        }
        default:
            error_exit("couldn't copy %s\n", getTypeName(var));
            return newNil();
    }
}

PVar evalExpr(PVar& expr, bool copy) {
    switch (getType(expr)) {
        case T_TRM:
        case K_RET: return handleLine(getLstRaw(expr));

        case T_ARG:
        case T_LST: {
            var_lst lst = getLstRaw(expr);
            int16_t i = 0, len = lst.size();
            var_lst flst(len);

            for (; i < len; i++) flst[i] = incRef(evalExpr(lst[i], true));
            return NEWVAR(TLst(flst, getType(expr), false));
        }
        default:
            if (copy)
                return copyVar(expr);
            else
                return expr;
    }
}

void fillArgs(var_lst& args, var_lst& dflt) {
    BEGIN();
    int16_t i = 0, len = dflt.size();

    // fill arguments
    for (i = args.size(); i < len; i++)
        args.push_back(incRef(copyVar(dflt[i])));
    END();
}

bool hasOperator(PVar& v, string o) {
    BEGIN("v=%s, op=%s", getTypeName(v), o.c_str());
    auto& opr = operations[getType(v)];
    bool res  = opr.find(o) != opr.end();
    if (!res) warning("operator not found"), getchar();
    END("%s", typeName(res ? K_TRU : K_FLS));
    return res;
    /*
    else {
        auto pop = operators.find(o);
        if (pop != operators.end()) {
            o = pop->second.name;
            END("%i", opr.find(o) != opr.end());
            return opr.find(o) != opr.end();
        }
    }*/
}


// freeing

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
        // INFO("del    %p", v);
        // INFO("del it %p", v->it);
        delete v;
        return false;
    });
    END();
}


// toStr

string toStr(PVar& v, uint8_t type) {
    return v->toString();
}

string toStr(var_chr& v, uint8_t type) {
    return typeName(type == T_BIN ? (v ? K_TRU : K_FLS) : type);
}

string toStr(var_int& v, uint8_t type) {
    if (type == T_PIN) return "Pin(" + to_string(v) + ")";
    if (type >= TCNT) return typeName(type);
    return to_string(v);
}

string toStr(var_flt& v, uint8_t type) {
    return dtos2(v);
}

string toStr(var_str& v, uint8_t type) {
    return v;
}

string toStr(var_lst& v, uint8_t type) {
    string result;
    char lstEnd;
    switch ((int)type >= KCNT ? KWType[type - KCNT] : type) {
        case (uint8_t)-1:
        case T_LST:
            result = "[";
            lstEnd = ']';
            break;

        case T_FNC:
            result = "{";
            lstEnd = '}';
            break;

        case K_RET:
        case T_TRM:
            result = "(";
            lstEnd = ')';
            break;

        case T_ARG:
            result = "<";
            lstEnd = '>';
            break;

        default:
            error_exit("unknown list type id %i", type);
            // result = "|"; lstEnd = '|';
    }

    for (PVar& v: v) result += v->toString(true) + ",";

    if (result.size() > 1)
        result[result.size() - 1] = lstEnd;
    else
        result += lstEnd;

    if (type >= TCNT) result = typeName(type) + (' ' + result);

    return result;
}

string toStr(var_obj& v, uint8_t type) {
    string result = "]";

    // builds string reversed because every new unordered_map
    // element is inserted at the front
    for (auto& v: v)
        result = ",\"" + v.first + "\":" + v.second->toString(true) + result;

    if (!v.empty())
        result[0] = '[';
    else
        result = result + "[";
    return result;
}

string toStr(var_bfn& v, uint8_t type) {
    string result = v->name;
    /*
    if (v->argc) {
        result += "(";
        for (uint16_t i = 1; i <= v->argc; i++)
            result += "p" + to_string(i) + ",";
        result[result.size() - 1] = ')';
    }
    else
        result += "()";
    result += " { [builtin] }"
    */
    return result;
}
