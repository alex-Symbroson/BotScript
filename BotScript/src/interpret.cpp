
#include "interpret.hpp"

/*       priority
.   at       6
[]  at       6
()  call     5
**  pow      4
*   mul      3
/   div      3
%   mod      3
+   add      2
-   sub      2
=   assign   1
*/

#define Op(SYM, NAM, PRI, DIR) \
    {                          \
        SYM, {                 \
            NAM, PRI, DIR      \
        }                      \
    }

unordered_map<string, Operator> operators = {
    Op("()", "call", 2, 'R'),   Op(".", "at", 2, 'R'),
    Op("[]", "at", 2, 'R'),     Op("**", "pow", 3, 'R'),
    Op("*", "mul", 4, 'R'),     Op("/", "div", 4, 'R'),
    Op("%", "mod", 4, 'R'),     Op("+", "add", 5, 'R'),
    Op("-", "sub", 5, 'R'),     Op("<<", "shl", 6, 'R'),
    Op(">>", "shr", 6, 'R'),    Op("<", "smaller", 7, 'R'),
    Op(">", "bigger", 7, 'R'),  Op("<=", "smalleq", 7, 'R'),
    Op(">=", "bigeq", 7, 'R'),  Op("==", "equal", 8, 'R'),
    Op("!=", "nequal", 8, 'R'), Op("&", "band", 9, 'R'),
    Op("^", "bxor", 10, 'R'),   Op("|", "bor", 11, 'R'),
    Op("&&", "and", 12, 'R'),   Op("||", "or", 13, 'R'),
    Op("=", "assign", 14, 'L')};

PVar callBuiltin(var_bfn func, var_lst args) {
    BEGIN("func=%s, args=%s", func->name, TLst(args, T_ARGS).toStr().c_str());

    // fill minimum arguments
    int16_t i;

    for (i = args.size(); i < func->argc; i++)
        args.push_back(func->dflt[i]);

    // evaluate arguments
    i = args.size();
    var_lst fargs(i);

    while (i--) {
        if (getType(args[i]) == T_TRM)
            fargs[i] = handleLine(getLst(args[i]));
        else
            fargs[i] = args[i];
    }

    // call function
    PVar res = func->func(fargs);
    END("-> %s", res->toStr().c_str());
    return res;
}

PVar handleLine(var_lst& line) {
    BEGIN("line=%s", TLst(line).toStr().c_str());

    uint32_t size           = line.size();
    var_lst::iterator begin = line.begin();
    PVar res                = *begin;
    if (getType(res) == T_TRM) res = handleLine(getLst(res));

    if (size == 1) {
        END("-> %s", res->toStr().c_str());
        return res;
    }

    if (size) {
        var_lst::iterator it = begin + 1, end = line.end();
        PVar param;

        do {
            if (size > 2) {
                if (getType(it[1]) == T_TRM && getLst(it[1]).size())
                    param = handleLine(getLst(it[1]));
                else
                    param = it[1];

                res = callP(res, getStr(it[0]), res, param);
            }
            it += 2;
        } while (it != end);

        if (res) {
            END("-> %s", res->toStr().c_str());
            return res;
        }
    }
    END("-> null");
    return V_NULL;
}

// scope interpreter
void handleScope(var_lst& scope) {
    BEGIN("var_lst*scope");

    for (auto& v: scope)
        handleLine(getLst(v));

    END();
}

// converts scope string to term
var_lst toFunction(string::iterator& c, string::iterator end, char separator) {
    BEGIN("string::it*c='%c',string::it end", *c);

    var_lst block;

    TLst* vline   = new TLst({}, T_TRM);
    var_lst* line = getLstP(vline);
    block.push_back(vline->getVar());

    uint8_t lastType = 0;

    while (*c != ')' && *c != '}' && c < end) {
        switch (*c) {
            case '"': {
                string word = "";
                while (*++c != '"' && c < end)
                    word += *c;
                line->push_back(NEWVAR(TStr(word)));
                DEBUG("%s: %s", word.c_str(), typeName(T_STR));
                lastType = T_STR;
            } break;

            case '[':
            case '(':
            case '{': {
                uint8_t type;
                char sep;

                if (*c == '(') {
                    type = T_TRM;
                    sep  = ',';
                } else if (*c == '{') {
                    type = T_FNC;
                    sep  = ';';
                } else /*if (*c == '[')*/ {
                    // detect list or obj later
                    sep  = ',';
                    type = T_LST;
                }
                DEBUG("%c: %s begin", *c, typeName(type));

                if (type == T_TRM && lastType == T_BFN) {
                    type = T_ARGS;
                    line->push_back(NEWVAR(TStr("call")));
                }

                ++c;
                line->push_back(NEWVAR(TLst(toFunction(c, end, sep), type)));
                DEBUG("%c: %s end", *c, typeName(type));
                lastType = type;
            } break;

            default:
                string word = "";

                if (isSymbol(*c)) {
                    bool wasOp = false, isOp = false, isSep = false,
                         cond = false;

                    do {
                        isSep = *c == separator;

                        if (isSep) {
                            DEBUG("seperator '%c'", separator);
                            vline = new TLst({}, T_TRM);
                            line  = getLstP(vline);
                            block.push_back(vline->getVar());
                        } else {
                            cond  = isOperator(*(c + 1)) && (c + 1 < end);
                            wasOp = isOperator(word);
                            isOp  = isOperator(word + *c);

                            if ((isOp && (!cond || isSep)) ||
                                (wasOp && (!cond || isSep || !isOp))) {
                                if (isOp) word += *c;

                                DEBUG(
                                    "operator '%s' isop: %i", word.c_str(),
                                    isOp);

                                line->push_back(NEWVAR(
                                    TStr(string(operators[word].name), T_OPR)));
                                word = "";

                                lastType = T_OPR;
                            } else
                                word += *c;
                        }

                        c++;
                    } while (cond);
                } else {
                    do {
                        word += *c;
                    } while (!isSymbol(*++c) && c < end);

                    if (isBuiltin(word.c_str())) {
                        line->push_back(NEWVAR(TBfn(getBltin(word))));
                        DEBUG("%s: %s", word.c_str(), typeName(T_BFN));
                        lastType = T_BFN;

                    } else if ((word[0] >= '0') && (word[0] <= '9')) {
                        line->push_back(NEWVAR(TFlt(stod2(word))));
                        DEBUG("%s: %s", word.c_str(), typeName(T_FLT));
                        lastType = T_FLT;

                    } else {
                        DEBUG("%s: %s", word.c_str(), typeName(T_VAR));
                        lastType = T_VAR;
                    }
                }
                continue;
        }
        ++c;
    }
    if (!line->size()) block.pop_back();
    END();
    return block;
}

// start recursive conversation from string to term
var_lst toCode(string& code) {
    BEGIN("string*code=\"%s\"", code.c_str());
    string::iterator c = code.begin();
    END();
    return toFunction(c, code.end(), ';');
}
