
#define CUSTOM_BEGIN
#include "variables.hpp"

#include "interpret.hpp"


#define BEGIN(...) BEGIN_1("Variables::", __func__, __VA_ARGS__)
#define END(...) END_1("Variables::", __func__, __VA_ARGS__)

// clang-format off
#define DEFOPR(NAME, ...) {            \
    NAME, [](PVar& a, PVar& b) -> PVar { \
        __VA_ARGS__ return newNil();   \
}   }


extern const uint8_t CtrlType[CCNT - KCNT]; // keyword types

// base types
const uint8_t VAR_Type[CCNT] = {
// T
    T_NIL, T_NIL, T_INT, T_FLT, T_STR, T_LST, T_OBJ,
    T_INT, T_LST, T_FNC, T_STR, T_LST, T_BFN, T_STR, T_STR,
// K
    T_NIL, T_INT, T_INT, T_NIL, T_NIL, T_LST,
// C
    T_LST, T_LST, T_FNC, T_FNC, T_LST, T_LST
};
// clang-format on


extern PVar funcResult; // latest returned value

list<PVar> collector = {};
FuncMapOpr operations[TCNT];


extern PVar handleLine(var_lst& line); // execute code line

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
    case T_VAR: return "variable";

    case T_NUM: return "number";

    case K_NIL: return "null";
    case K_TRU: return "true";
    case K_FLS: return "false";
    case K_BRK: return "break";
    case K_CNT: return "continue";
    case K_RET: return "return";

    case C_CIF: return "if";
    case C_EIF: return "elif";
    case C_ELS: return "else";
    case C_CDO: return "do";
    case C_WHL: return "while";
    case C_UNT: return "until";

    default: error_exit("unknown type id %i - report", t); return "undefined";
    }
}


IVar::IVar() {
    collector.push_front(this);
    this->it = collector.begin();
}

IVar::~IVar() {
    if (this->refcnt && status != S_FREE)
        error( // DEBUG
            "freeing %p with refcount %i during execution (=%s)", this, refcnt,
            toString().c_str()),
            getchar();
    collector.erase(this->it);
}


#define TypeClassConstructor(TMPL, TYPE, TYPEID)                              \
    template <TMPL>                                                           \
    TVar<TYPE>::TVar(TYPE v, uint8_t typeID, bool Const) {                    \
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
    }

#define TypeClassDestructor(TMPL, TYPE, TYPEID) \
    template <TMPL>                             \
    TVar<TYPE>::~TVar() {}

TypeClassConstructor(typename T, T, T_NIL);
TypeClassConstructor(, var_chr, T_NIL);
TypeClassConstructor(, var_int, T_INT);
TypeClassConstructor(, var_flt, T_FLT);
TypeClassConstructor(, var_str, T_STR);
TypeClassConstructor(, var_lst, T_LST);
TypeClassConstructor(, var_obj, T_OBJ);
TypeClassConstructor(, var_bfn, T_BFN);
TypeClassConstructor(, var_fnc, T_FNC);

TypeClassDestructor(typename T, T, T_NIL);
TypeClassDestructor(, var_chr, T_NIL);
TypeClassDestructor(, var_int, T_INT);
TypeClassDestructor(, var_flt, T_FLT);
TypeClassDestructor(, var_str, T_STR);
TypeClassDestructor(, var_bfn, T_BFN);
TypeClassDestructor(, var_fnc, T_FNC);


template <>
TVar<var_lst>::~TVar() {
    if (status != S_FREE)
        for (PVar& v: value) decRef(v);
    // value.clear();
}

template <>
TVar<var_obj>::~TVar() {
    if (status != S_FREE)
        for (auto& v: value) decRef(v.second);
    // value.clear();
}


// clang-format off

// dont modify const values!
bool initOperations() {
    BEGIN();

    // Nil operators
    operations[T_NIL] = {};

    // Bool operators
    operations[T_BIN] = {
        DEFOPR("equal", { return newBin(getBinRaw(a) == getBin(b)); }),
        DEFOPR("and", { return newBin(getBinRaw(a) && getBin(b)); }),
        DEFOPR("or", { return newBin(getBinRaw(a) || getBin(b)); }),
        DEFOPR("xor", { return newBin(getBinRaw(a) ^ getBin(b)); }),
    };

    // Integer operators
    operations[T_INT] = {
        DEFOPR("assign", {
            switch (getType(b)) {
            case T_INT: getIntRaw(a) = getIntRaw(b); return a;
            case T_FLT: getIntRaw(a) = getFltRaw(b); return a;
            default: err_iat(a, b, "assign", T_INT);
            }
        }),
        DEFOPR("equal", {
            switch (getType(b)) {
            case T_INT: return newBin(getIntRaw(a) == getIntRaw(b));
            case T_FLT: return newBin(getIntRaw(a) == getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("nequal", {
            switch (getType(b)) {
            case T_INT: return newBin(getIntRaw(a) != getIntRaw(b));
            case T_FLT: return newBin(getIntRaw(a) != getFltRaw(b));
            default: return newBin(true);
            }
        }),
        DEFOPR("smaller", {
            switch (getType(b)) {
            case T_INT: return newBin(getIntRaw(a) < getIntRaw(b));
            case T_FLT: return newBin(getIntRaw(a) < getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("smalleq", {
            switch (getType(b)) {
            case T_INT: return newBin(getIntRaw(a) <= getIntRaw(b));
            case T_FLT: return newBin(getIntRaw(a) <= getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("bigger", {
            switch (getType(b)) {
            case T_INT: return newBin(getIntRaw(a) > getIntRaw(b));
            case T_FLT: return newBin(getIntRaw(a) > getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("bigeq", {
            switch (getType(b)) {
            case T_INT: return newBin(getIntRaw(a) >= getIntRaw(b));
            case T_FLT: return newBin(getIntRaw(a) >= getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("add", {
            switch (getType(b)) {
            case T_INT: return newInt(getIntRaw(a) + getIntRaw(b));
            case T_FLT: return newFlt(getIntRaw(a) + getFltRaw(b));
            default: err_iop("+", a, b);
            }
        }),
        DEFOPR("sub", {
            switch (getType(b)) {
            case T_INT: return newInt(getIntRaw(a) - getIntRaw(b));
            case T_FLT: return newFlt(getIntRaw(a) - getFltRaw(b));
            default: err_iop("-", a, b);
            }
        }),
        DEFOPR("mul", {
            switch (getType(b)) {
            case T_INT: return newInt(getIntRaw(a) * getIntRaw(b));
            case T_FLT: return newFlt(getIntRaw(a) * getFltRaw(b));
            default: err_iop("*", a, b);
            }
        }),
        DEFOPR("div", {
            switch (getType(b)) {
            case T_INT: return newInt(getIntRaw(a) / getIntRaw(b));
            case T_FLT: return newFlt(getIntRaw(a) / getFltRaw(b));
            default: err_iop("/", a, b);
            }
        }),
        DEFOPR("mod", {
            switch (getType(b)) {
            case T_INT: return newInt(getIntRaw(a) % getIntRaw(b));
            case T_FLT: {
                var_flt c = getIntRaw(a), d = getFltRaw(b);
                return newFlt(c - d * floor(c / d));
            }
            default: err_iop("%", a, b);
            }
        }),
        DEFOPR("pow", {
            switch (getType(b)) {
            case T_INT: return newFlt(pow(getIntRaw(a), getIntRaw(b)));
            case T_FLT: return newFlt(pow(getIntRaw(a), getFltRaw(b)));
            default: err_iop("**", a, b);
            }
        }),
        DEFOPR("shl", { return newInt(getIntRaw(a) << getInt(b)); }),
        DEFOPR("shr", { return newInt(getIntRaw(a) >> getInt(b)); }),
        DEFOPR("band", { return newInt(getIntRaw(a) & getInt(b)); }),
        DEFOPR("bor", { return newInt(getIntRaw(a) | getInt(b)); }),
        DEFOPR("bxor", { return newInt(getIntRaw(a) ^ getInt(b)); }),
        DEFOPR("toStr", {
            var_lst args = getArg(b);
            string str;

            if (!args.empty())
                str = dtos2(getIntRaw(a), getInt(args[0]));
            else
                str = dtos2(getIntRaw(a), 10);

            if(str[str.size() - 2] == '.') str.erase(str.size() - 2);
            return newStr(str);
        })
    };

    // Float operators
    operations[T_FLT] = {
        DEFOPR("assign", {
            switch (getType(b)) {
            case T_INT: getFltRaw(a) = getIntRaw(b); return a;
            case T_FLT: getFltRaw(a) = getFltRaw(b); return a;
            default: err_iat(a, b, "assign", T_FLT);
            }
        }),
        DEFOPR("equal", {
            switch (getType(b)) {
            case T_INT: return newBin(getFltRaw(a) == getIntRaw(b));
            case T_FLT: return newBin(getFltRaw(a) == getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("smaller", {
            switch (getType(b)) {
            case T_INT: return newBin(getFltRaw(a) < getIntRaw(b));
            case T_FLT: return newBin(getFltRaw(a) < getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("smalleq", {
            switch (getType(b)) {
            case T_INT: return newBin(getFltRaw(a) <= getIntRaw(b));
            case T_FLT: return newBin(getFltRaw(a) <= getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("bigger", {
            switch (getType(b)) {
            case T_INT: return newBin(getFltRaw(a) > getIntRaw(b));
            case T_FLT: return newBin(getFltRaw(a) > getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("bigeq", {
            switch (getType(b)) {
            case T_INT: return newBin(getFltRaw(a) >= getIntRaw(b));
            case T_FLT: return newBin(getFltRaw(a) >= getFltRaw(b));
            default: return newBin(false);
            }
        }),
        DEFOPR("nequal", {
            switch (getType(b)) {
            case T_INT: return newBin(getFltRaw(a) != getIntRaw(b));
            case T_FLT: return newBin(getFltRaw(a) != getFltRaw(b));
            default: return newBin(true);
            }
        }),
        DEFOPR("add", {
            switch (getType(b)) {
            case T_INT: return newFlt(getFltRaw(a) + getIntRaw(b));
            case T_FLT: return newFlt(getFltRaw(a) + getFltRaw(b));
            default: err_iop("+", a, b);
            }
        }),
        DEFOPR("sub", {
            switch (getType(b)) {
            case T_INT: return newFlt(getFltRaw(a) - getIntRaw(b));
            case T_FLT: return newFlt(getFltRaw(a) - getFltRaw(b));
            default: err_iop("-", a, b);
            }
        }),
        DEFOPR("mul", {
            switch (getType(b)) {
            case T_INT: return newFlt(getFltRaw(a) * getIntRaw(b));
            case T_FLT: return newFlt(getFltRaw(a) * getFltRaw(b));
            default: err_iop("*", a, b);
            }
        }),
        DEFOPR("div", {
            switch (getType(b)) {
            case T_INT: return newFlt(getFltRaw(a) / getIntRaw(b));
            case T_FLT: return newFlt(getFltRaw(a) / getFltRaw(b));
            default: err_iop("/", a, b);
            }
        }),
        DEFOPR("mod", {
            var_flt c = getFltRaw(a), d = 1;
            switch (getType(b)) {
            case T_INT: d = getIntRaw(b);
            case T_FLT: d = getFltRaw(b);
            default: err_iop("%", a, b);
            }
            return newFlt(c - d * floor(c / d));
        }),
        DEFOPR("pow", {
            switch (getType(b)) {
            case T_INT: return newFlt(pow(getFltRaw(a), getIntRaw(b)));
            case T_FLT: return newFlt(pow(getFltRaw(a), getFltRaw(b)));
            default: err_iop("**", a, b);
            }
        }),
        DEFOPR("toStr", {
            var_lst args = getArg(b);

            if (!args.empty()) {
                FILLARGS(args, {});
                PVar res = newStr(dtos2(getFltRaw(a), getInt(args[0])));
                return res;
            } else
                return newStr(dtos2(getFltRaw(a), 10));
        })
    };

    // String operators
    operations[T_STR] = {
        DEFOPR("assign", {
            getStrRaw(a) = getStr(b);
            return a;
        }),
        DEFOPR("equal", {
            if (getType(b) == T_STR)
                return newBin(getStrRaw(a) == getStrRaw(b));
            else
                return newBin(false);
        }),
        DEFOPR("nequal", {
            if (getType(b) == T_STR)
                return newBin(getStrRaw(a) != getStrRaw(b));
            else
                return newBin(true);
        }),
        DEFOPR("add", { return newStr(getStrRaw(a) + getStr(b)); }),
        DEFOPR("toInt", {
            string s    = getStrRaw(a);
            int32_t pos = s.find('.');
            if (pos > -1) s[pos] = 0;
            return newInt(stod2(s));
        }),
        DEFOPR("toFlt", { return newFlt(stod2(getStrRaw(a))); })
    };

    // Func operators
    operations[T_FNC] = {
        DEFOPR("assign", {
             if (!a->isConst) getFncRaw(a) = getFnc(b);
             return a;
         }),
        DEFOPR("call", {
            if (getType(b) == T_ARG) {
                handleFunc(a, b);
                if (status == S_RETURN) status = S_EXEC;
                return funcResult;
            } else
                err_iop("call", a, b);
        })
    };

    // BuiltinFunc operators
    operations[T_BFN] = {
        DEFOPR("call", {
            PVar res = getBfnRaw(a)->func(getArg(b));
            return res;
        })
    };

    // List operators
    operations[T_LST] = {
        DEFOPR("assign", {
            if (!a->isConst) getLstRaw(a) = getLst(b);
            return a;
        }),
        DEFOPR("push", {
            if (a->isConst)
                warning("cannot \"push\" to constant list");
            else
                getLstRaw(a).push_back(incRef(b));
            return newInt(getLstRaw(a).size() + a->isConst);
        }),
        DEFOPR("pop", {
            var_lst& lst = getLstRaw(a);
            if (a->isConst)
                warning("cannot \"pop\" from constant list");
            else
                lst.pop_back();
            PVar ret = lst.back();
            ret->refcnt--;
            return ret;
        }),
        DEFOPR("at", {
            if (getType(b) == T_INT) {
                var_int i    = getInt(b);
                var_lst& lst = getLstRaw(a);
                int len      = lst.size();
                if (i < 0) i += len;

                if (i < 0 || i >= len)
                    err_rng(a, b);
                else
                    return lst[i];
            } else
                err_iop("at", a, b);
        }),
        DEFOPR("join", {
            var_lst args = getArg(b);

            if (!args.empty()) {
                FILLARGS(args, {});
                string result = "", sep = "";

                if (getType(args[0]) == T_STR)
                    sep = getStr(args[0]);
                else
                    err_iat(a, args[0], "join", T_STR);

                PVar tmp, last = getLstRaw(a).back();
                for (PVar& v: getLstRaw(a)) {
                    tmp = incRef(evalExpr(v, false));
                    result += tmp->toString();
                    if (v != last) result += sep;
                    decRef(tmp);
                }

                return newStr(result);
            } else
                err_iac2(a, args, "join", 1);
        }),
        DEFOPR("getLength", { return newInt(getLstRaw(a).size()); })
    };

    // Argument operators
    operations[T_ARG] = {
        DEFOPR("at", {
            if (getType(b) == T_INT) {
                var_int i    = getInt(b);
                var_lst& lst = getArgRaw(a);
                int len      = lst.size();
                if (i < 0) i += len;

                if (i < 0 || i >= len)
                    err_rng(a, b);
                else
                    return lst[i];
            } else
                err_iop("at", a, b);
        }),
        DEFOPR("getLength", { return newInt(getArgRaw(a).size()); })
    };

    END("false");
    return false;
}

// clang-format on

PVar copyVar(PVar& var) {
    // BEGIN("%s", TOSTR(var));
    // END();
    switch (getBaseType(var)) {
    case T_NIL: return NEWVAR(TChr(getChrRaw(var), getType(var)));
    case T_INT: return NEWVAR(TInt(getIntRaw(var), getType(var)));
    case T_FLT: return NEWVAR(TFlt(getFltRaw(var), getType(var)));
    case T_STR: return NEWVAR(TStr(getStrRaw(var), getType(var)));
    case T_BFN: return NEWVAR(TBfn(getBfnRaw(var), getType(var)));
    case T_FNC: return var;
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

    case T_VAR: {
        PVar res = findVar(getVarRaw(expr), curScope);
        if (!res) error_exit("unknown variable '%s'", TOSTR(expr));
        return res;
    }

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
    if (!res) warning("operator not found");
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
    char lstEnd = '0';
    switch ((int)type >= KCNT ? keyType(type) : type) {
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
        error_exit("unknown list type '%s'", typeName(type));
        // result = "|"; lstEnd = '|';
    }

    for (PVar& var: v) result += var->toString(true) + ",";

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
    for (auto& var: v)
        result =
            ",\"" + var.first + "\":" + var.second->toString(true) + result;

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

string toStr(var_fnc& v, uint8_t type) {
    string result = toStr(v.func, T_FNC);
    if (type == T_FNC) {
        result = "()" + result; // args
        if (v.name) result = v.name + result;
    } else if (type >= TCNT)
        result = typeName(type) + (' ' + result);
    else
        error_exit("unknown func type '%s'", typeName(type));
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
