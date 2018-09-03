
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

// all operators
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

// type of the keyword itself
unordered_map<string, const uint8_t> keywords = {
    {"null", K_NIL}, {"true", K_TRU},  {"false", K_FLS},
    {"if", C_CIF},   {"elif", C_EIF},  {"else", C_ELS},
    {"do", C_CDO},   {"while", C_WHL}, {"until", C_UNT}};

// type of the value after the keyword (nil - no value)
uint8_t KWType[CCNT - KCNT] = {T_TRM, T_TRM, T_FNC, T_FNC, T_TRM, T_TRM};

#define getKeyType(k) KWType[k - KCNT]

PVar handleLine(var_lst& line) {
    BEGIN("line=%s", TLst(line).toStr().c_str());

    if (line.empty()) {
        END("-> null");
        return V_NULL;
    }

    // res: variable, it[0]: base var, it[1]: sub func, it[2]: args
    uint32_t size;
    PVar res;
    uint8_t type, firstType = T_NIL;
    var_lst::iterator it, begin, end;

    size  = line.size();
    begin = line.begin();
    type  = getType(*begin);

    if (size == 1) {
        if (type < TCNT) {
            res = evalExpr(*begin);
            END("-> %s", TOSTR(res));
            return res;
        } else if (type < KCNT) {
            goto err_tok;
        }
    }

    it  = begin;
    end = line.end();

    // handle control structures
    if (type >= KCNT) {
        res       = V_NULL;
        firstType = type;

        switch (firstType) {
            // if .. elif .. else
            case C_CIF:
                do {
                    type = getType(*it);
                    if (type == C_CIF || type == C_EIF) {
                        REPVAR(res, handleLine(getLst(*it)));

                        if (getType(res) == T_BIN) {
                            // go to next condition
                            if (!getInt(res)) it++;
                        } else {
                            goto err_bin;
                        }
                    } else if (type == C_ELS) {
                        break;
                    } else {
                        goto err_tok;
                    }

                    if (it == end) break;
                    // go to next block
                    it++;
                } while (it != end);

                if (it != end) {
                    if (getType(*it) == C_CDO || getType(*it) == C_ELS) {
                        handleScope(getLst(*it));
                        it++;
                    } else {
                        type = getType(*it);
                        goto err_tok;
                    }
                }

                if (it != end) {
                    type = getType(*it);
                    goto err_tok;
                }
                break;

            case C_CDO:
                if (size == 1) { // do
                    handleScope(getLst(*it));
                } else {
                    type = getType(it[1]);
                    switch (type) {
                        case C_WHL: // do .. while
                            do {
                                handleScope(getLst(*it));
                                REPVAR(res, handleLine(getLst(it[1])));
                                if (getType(res) != T_BIN) goto err_bin;
                            } while (getInt(res));
                            break;

                        case C_UNT: // do .. until
                            do {
                                handleScope(getLst(*it));
                                REPVAR(res, handleLine(getLst(it[1])));
                                if (getType(res) != T_BIN) goto err_bin;
                            } while (!getInt(res));
                            break;

                        default: goto err_tok;
                    }
                }
                break;

            case C_WHL:
                if (size == 1) { // while
                    do {
                        REPVAR(res, handleLine(getLst(*it)));
                        if (getType(res) != T_BIN) goto err_bin;
                    } while (getInt(res));
                } else {
                    type = getType(it[1]);

                    if (type == C_CDO) { // while .. do
                        REPVAR(res, handleLine(getLst(*it)));
                        if (getType(res) != T_BIN) goto err_bin;

                        while (getInt(res)) {
                            handleScope(getLst(*it));
                            REPVAR(res, handleLine(getLst(*it)));
                            if (getType(res) != T_BIN) goto err_bin;
                        }
                    } else {
                        goto err_tok;
                    }
                }
                break;

            case C_UNT:
                if (size == 1) { // until
                    do {
                        REPVAR(res, handleLine(getLst(*it)));
                        if (getType(res) != T_BIN) goto err_bin;
                    } while (!getInt(res));
                } else {
                    type = getType(it[1]);

                    if (type == C_CDO) { // until .. do
                        REPVAR(res, handleLine(getLst(*it)));
                        if (getType(res) != T_BIN) goto err_bin;

                        while (!getInt(res)) {
                            handleScope(getLst(*it));
                            REPVAR(res, handleLine(getLst(*it)));
                            if (getType(res) != T_BIN) goto err_bin;
                        }
                    } else {
                        goto err_tok;
                    }
                }
                break;

            default:
                error_exit("invalid line %s", TLst(line).toStr().c_str());
                delete res;
                return V_NULL;
        }
    } else {
        // other statements
        res      = evalExpr(*begin);
        PVar arg = V_NULL;
        it++;

        do {
            if (size > 2) {
                // check wether operator available
                if (!hasOperator(res, getStr(it[0])))
                    error_exit(
                        "%s has no operator %s", getTypeName(res),
                        TOSTR(it[0]));

                // calculate term of 2nd arg if required
                if (getType(it[1]) == T_TRM) {
                    if (!getLst(it[1]).empty()) {
                        REPVAR(arg, evalExpr(it[1]));
                    } else {
                        error_exit("empty term");
                        REPVAR(arg, V_NULL);
                    }
                } else {
                    arg = evalExpr(it[1]);
                }
                // PVar t = res;
                res = callP(res, getStr(it[0]), arg);
                // delete t;
            } else {
                delete res;
                return it[0];
            }

            it += 2;
            size -= 2;

        } while (it != end);
    }

    if (res) {
        END("-> %s", TOSTR(res));
        return res;
    }

    error_exit("no result of line %s", TLst(line).toStr().c_str());
    return V_NULL;

err_tok:
    if (firstType == T_NIL)
        error_exit("unexpected token %s", typeName(type));
    else
        error_exit(
            "unexpected token %s in %s statement", typeName(type),
            typeName(firstType));
    return V_NULL;

err_bin:
    error_exit(
        "expected boolean in %s condition, got %s", typeName(type),
        getTypeName(res));
    return V_NULL;
}

// scope interpreter
void handleScope(var_lst& scope) {
    BEGIN("var_lst*scope");
    for (auto& line: scope) delete handleLine(getLst(line));
    END();
}

// converts scope string to term
var_lst toFunction(string::iterator& c, char separator, char end) {
    BEGIN("string::it*c='%c',sep='%c',end='%c'\n", *c, separator, end);

    var_lst block;
    var_lst line;

    uint8_t lastType = T_NIL;
    string word;

    auto addVar = [&line, &lastType](PVar v, bool dbg = true) {
        line.push_back(v);
        lastType = getType(v);
        if (dbg) DEBUG("%s: %s", TOSTR(v), typeName(lastType));
    };

    while (*c != end && *c != ')' && *c != ']' && *c != '}') {
        word = "";

        switch (*c) {
            case '"': {
                while (*++c != '"' && *c != end) word += *c;

                if (lastType == T_STR)
                    getStr(line.back()) += unescape(word);
                else
                    addVar(newStr(unescape(word)));

                DEBUG("\"%s\": %s", word.c_str(), typeName(T_STR));
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
                } else if (*c == '[') {
                    // detect list or obj later
                    type = T_LST;
                    sep  = ',';
                    end  = ']';
                } else {
                    error_exit("unexpected token %c", *c);
                }

                if (type == T_TRM && lastType == T_MFN) {
                    type = T_ARG;
                } else if (type == T_TRM && lastType == T_BFN) {
                    type = T_ARG;
                    addVar(newStr("call"));
                } else if (
                    type == T_LST && (lastType == T_LST || lastType == T_OBJ)) {
                    type = T_TRM;
                    addVar(newOpr("at"));
                }
                if (lastType >= KCNT) {
                    if (type == getKeyType(lastType))
                        type = lastType;
                    else
                        error_exit("ignored %s control", typeName(lastType));
                }

                DEBUG("%c: %s begin", *c, typeName(type));
                ++c;
                var_lst list = toFunction(c, sep, end);
                DEBUG("%c: %s end", *c, typeName(type));

                if (type == T_TRM && list.size() == 1 &&
                    getType(list[0]) < TCNT)
                    addVar(list[0]);
                else
                    addVar(NEWVAR(TLst(list, type)), false);

            } break;

            default: {
                if (*c == separator) {
                    if (line.size() == 1 && getType(line[0]) < TCNT)
                        block.push_back(line[0]);
                    else
                        block.push_back(newTrm(line));

                    line.clear();
                    lastType = T_NIL;
                    word += *c++;

                } else if (
                    !line.empty() && (getType(line.back()) != T_OPR) &&
                    isOperator(*c) &&
                    (*c != '.' || ((c[-1] < '0' || c[-1] > '9') &&
                                   (c[01] < '0' || c[01] > '9')))) {
                    do {
                        word += *c;
                    } while (isOperator(word + *++c) && *c != end);

                    addVar(newOpr(operators.at(word).name));
                } else {
                    // clang-format off
                    do {
                        word += *c++;
                    } while (
                        *c != end &&
                            (isAlphaNum(*c) ||
                                // prevent floating point to be "at" operator
                                (*c == '.' && (
                                    (c[-1] >= '0' && c[-1] <= '9') ||
                                    (c[01] >= '0' && c[01] <= '9')
                                ))
                            )
                        );

                    if (*c == ' ') *c++;
                    // clang-format on

                    auto kwtype = keywords.find(word);
                    if (kwtype != keywords.end()) {
                        switch (kwtype->second) {
                            case K_NIL: addVar(newNil(0)); break;
                            case K_TRU: addVar(newBin(true)); break;
                            case K_FLS: addVar(newBin(false)); break;
                            default:
                                lastType = kwtype->second;
                                DEBUG("keyword %i", lastType);
                        }
                        continue;
                    } // else ..

                    // operator
                    if (lastType == T_OPR && getStr(line.back()) == "at") {
                        getStr(line.back()) = word;
                        setType(line.back(), T_MFN);
                        lastType = T_MFN;

                        // builtin
                    } else if (isBuiltin(word.c_str())) {
                        addVar(newBfn(getBltin(word)));

                        // number
                    } else if (
                        (word[0] >= '0' && word[0] <= '9') ||
                        (word[0] == '.' || word[0] == '-' || word[0] == '+')) {
                        if ((long)word.find(".") == -1)
                            addVar(newInt(stod2(word)));
                        else
                            addVar(newFlt(stod2(word)));

                    } else {
                        error_exit("unexpected token %s", word.c_str());
                    }
                }
                // if (lastType != T_NIL)
                //    DEBUG("%s: %s", word.c_str(), typeName(lastType));
                continue;
            }
        }
        ++c;
    }

    if (*c != end) {
        block.clear();
        END();
        if (!end)
            error_exit("invalid token '%c' - expected EOF", *c);
        else
            error_exit("invalid token '%c' - expected '%c'", *c, end);
    }

    if (line.size() == 1 && getType(line[0]) < TCNT)
        block.push_back(line[0]);
    else if (!line.empty())
        block.push_back(newTrm(line));

    END("-> %s\n", TLst(block).toStr().c_str());
    return block;
}

// start recursive conversation from string to term
var_lst toCode(string& code) {
    BEGIN("string*code=\"%s\"", code.c_str());
    string::iterator c = code.begin();
    END();
    return toFunction(c, ';', *code.end());
}
