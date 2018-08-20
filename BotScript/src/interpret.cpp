
#include "interpret.hpp"

/*       priority
.   at       6        []  at       6
()  call     5        **  pow      4
*   mul      3        /   div      3
%   mod      3        +   add      2
-   sub      2        =   assign   1
*/

// clang-format off
#define Op(SYM, NAM, PRI, DIR) { SYM, { NAM, PRI, DIR } }
// clang-format on

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

PVar handleLine(var_lst& line) {
    BEGIN("line=%s", TLst(line).toStr().c_str());

    uint32_t size           = line.size();
    var_lst::iterator begin = line.begin();
    PVar res                = *begin;
    res                     = evalExpr(res);

    if (size == 1) {
        END("-> %s", TOSTR(res));
        return res;
    }

    if (size) {
        var_lst::iterator it = begin + 1, end = line.end();
        PVar arg;
        // res: variable, it[0]: function, it[1]: arguments

        do {
            if (size > 2) {
                if (!hasOperator(res, getStr(it[0])))
                    error_exit(
                        "%s has no operator %s", getTypeName(res),
                        TOSTR(it[0]));

                if (getType(it[1]) == T_TRM && getLst(it[1]).size())
                    arg = evalExpr(it[1]);
                else
                    arg = it[1];

                res = callP(res, getStr(it[0]), arg);
            } else
                return it[0];

            it += 2;
            size -= 2;
        } while (it != end);

        if (res) {
            END("-> %s", TOSTR(res));
            return res;
        }
    }
    END("-> null");
    return V_NULL;
}

// scope interpreter
void handleScope(var_lst& scope) {
    BEGIN("var_lst*scope");

    for (auto& v: scope) handleLine(getLst(v));

    END();
}

// converts scope string to term
var_lst toFunction(string::iterator& c, char separator, char end) {
    BEGIN("string::it*c='%c',sep='%c',end='%c'", *c, separator, end);

    var_lst block;

    TLst* vline   = new TLst({}, T_TRM);
    var_lst* line = getLstP(vline);
    block.push_back(vline->getVar());

    uint8_t lastType = 0;

    while (*c != end && *c != ')' && *c != ']' && *c != '}') {
        switch (*c) {
            case '"': {
                string word = "";
                while (*++c != '"' && *c != end) word += *c;

                if (lastType == T_STR)
                    getStr(line->back()) += unescape(word);
                else
                    line->push_back(NEWVAR(TStr(unescape(word))));

                DEBUG("\"%s\": %s", word.c_str(), typeName(T_STR));
                lastType = T_STR;
            } break;

            case '[':
            case '(':
            case '{': {
                uint8_t type;
                char sep, end;

                if (*c == '(') {
                    type = T_TRM;
                    sep  = ',';
                    end  = ')';
                } else if (*c == '{') {
                    type = T_FNC;
                    sep  = ';';
                    end  = '}';
                } else /*if (*c == '[')*/ {
                    // detect list or obj later
                    type = T_LST;
                    sep  = ',';
                    end  = ']';
                }
                DEBUG("%c: %s begin", *c, typeName(type));

                if (type == T_TRM && lastType == T_MFN) {
                    type = T_ARGS;
                } else if (type == T_TRM && lastType == T_BFN) {
                    type = T_ARGS;
                    line->push_back(NEWVAR(TStr("call")));
                } else if (
                    type == T_LST && (lastType == T_LST || lastType == T_OBJ)) {
                    type = T_TRM;
                    line->push_back(NEWVAR(TStr("at", T_OPR)));
                }

                ++c;
                line->push_back(NEWVAR(TLst(toFunction(c, sep, end), type)));
                DEBUG("%c: %s end", *c, typeName(type));
                lastType = type;
            } break;

            default:
                string word = "";

                if (*c == separator) {
                    vline = new TLst({}, T_TRM);
                    line  = getLstP(vline);
                    block.push_back(vline->getVar());
                    lastType = 0;
                    word += *c++;

                } else if (
                    line->size() && isOperator(*c) &&
                    (*c != '.' || ((c[-1] < '0' || c[-1] > '9') &&
                                   (c[01] < '0' || c[01] > '9')))) {
                    do {
                        word += *c;
                    } while (isOperator(word + *++c) && *c != end);

                    line->push_back(NEWVAR(TStr(operators[word].name, T_OPR)));
                    lastType = T_OPR;

                } else {
                    do {
                        word += *c++;
                    } while (*c != end &&
                             (!isSymbol(*c) ||
                              // prevent floating point to be "at" operator
                              (*c == '.' && ((c[-1] >= '0' && c[-1] <= '9') ||
                                             (c[01] >= '0' && c[01] <= '9')))));

                    if (lastType == T_OPR && getStr(line->back()) == "at") {
                        getStr(line->back()) = word;
                        lastType = getType(line->back()) = T_MFN;

                    } else if (isBuiltin(word.c_str())) {
                        line->push_back(NEWVAR(TBfn(getBltin(word))));
                        lastType = T_BFN;

                    } else if (
                        (word[0] >= '0' && word[0] <= '9') ||
                        (word[0] == '.' || word[0] == '-' || word[0] == '+')) {
                        if ((long)word.find(".") == -1) {
                            line->push_back(NEWVAR(TInt(stod2(word))));
                            lastType = T_INT;
                        } else {
                            line->push_back(NEWVAR(TFlt(stod2(word))));
                            lastType = T_FLT;
                        }

                    } else {
                        DEBUG("%s: [ignored]", word.c_str());
                        lastType = T_NIL;
                    }
                }
                if (lastType != T_NIL)
                    DEBUG("%s: %s", word.c_str(), typeName(lastType));
                continue;
        }
        ++c;
    }

    if (*c != end) {
        block.clear();
        if (!end)
            error_exit("invalid token '%c' - expected EOF", *c);
        else
            error_exit("invalid token '%c' - expected '%c'", *c, end);
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
    return toFunction(c, ';', *code.end());
}
