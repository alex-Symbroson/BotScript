
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
    {"null", K_NIL},     {"true", K_TRU},   {"false", K_FLS}, {"break", K_BRK},
    {"continue", K_CNT}, {"return", K_RET}, {"if", C_CIF},    {"elif", C_EIF},
    {"else", C_ELS},     {"do", C_CDO},     {"while", C_WHL}, {"until", C_UNT}};

// type of the value after the keyword (nil - no value)
uint8_t CtrlType[CCNT - KCNT] = {T_TRM, T_TRM, T_FNC, T_FNC, T_TRM, T_TRM};

PVar funcResult;

PVar handleLine(var_lst& line) {
    BEGIN("line=%s", TOSTR(line));
    funcResult = incRef(newNil());

    if (line.empty()) {
        END("0 null");
        return funcResult;
    }

    // res: variable, it[0]: base var, it[1]: sub func, it[2]: args
    uint32_t size;
    PVar res;
    uint8_t type, firstType = T_NIL;
    var_lst::iterator it, begin, end;

    it   = line.begin();
    size = line.size();
    type = getType(*it);

    if (size == 1) {
        if (type < TCNT) {
            REPVAR(funcResult, evalExpr(*it, false));
            END(" %s", TOSTR(funcResult));
            return funcResult;
        } else {
            switch (type) {
                // control statements
                case C_CDO: handleScope(getFncRaw(*it)); break;

                case C_WHL:
                    do {
                        REPVAR(res, handleLine(getTrmRaw(*it)));
                        if (status == S_CONTINUE) status = S_EXEC;
                    } while (getBin(res) && !BREAK);

                    if (status == S_BREAK) status = S_EXEC;
                    break;

                case C_UNT:
                    do {
                        REPVAR(res, handleLine(getTrmRaw(*it)));
                        if (status == S_CONTINUE) status = S_EXEC;
                    } while (!getBin(res) && !BREAK);

                    if (status == S_BREAK) status = S_EXEC;
                    break;

                // keywords
                case K_BRK: status = S_BREAK; break;

                case K_RET:
                    if (size != 1) goto err_tok;
                    status = S_RETURN;
                    REPVAR(funcResult, evalExpr(*it, false));
                    break;

                case K_CNT: status = S_CONTINUE; break;

                default: goto err_tok;
            }
            END("%s", TOSTR(funcResult));
            return funcResult;
        }
    }

    begin = it;
    end   = line.end();

    // handle control structures
    if (type >= KCNT) {
        res       = incRef(newNil());
        firstType = type;
        type      = getType(it[1]);

        switch (firstType) {
            // if .. elif .. else
            case C_CIF:
                // find statement where condition is true
                do {
                    type = getType(*it);
                    if (type == C_CIF || type == C_EIF) {
                        // parse condition
                        DEBUG("%s", TOSTR(*it));
                        REPVAR(res, handleLine(getTrmRaw(*it)));
                        DEBUG("%s -> %s", TOSTR(*it), TOSTR(res));

                        it++; // go to next block

                        // execute block
                        if (getBin(res)) break;
                    } else if (type == C_ELS)
                        break;
                    else
                        goto err_tok;

                    if (it == end) break;
                    it++; // go to next block
                } while (it != end);

                // execute scope
                if (it != end) {
                    DEBUG("%s", TOSTR(*it));
                    type = getType(*it);
                    if (type == C_CDO || type == C_ELS)
                        handleScope(getFncRaw(*it));
                    else
                        goto err_tok;
                }

                break;

            // do [ .. while | until ]
            case C_CDO:
                switch (type) {
                    // do .. while
                    case C_WHL:
                        do {
                            handleScope(getFncRaw(*it));
                            if (status == S_CONTINUE) status = S_EXEC;
                            if (BREAK) break;
                            REPVAR(res, handleLine(getTrmRaw(it[1])));
                            if (status == S_CONTINUE) status = S_EXEC;
                        } while (getBin(res) && !BREAK);
                        break;

                    // do .. until
                    case C_UNT:
                        do {
                            handleScope(getFncRaw(*it));
                            if (status == S_CONTINUE) status = S_EXEC;
                            if (BREAK) break;
                            REPVAR(res, handleLine(getTrmRaw(it[1])));
                            if (status == S_CONTINUE) status = S_EXEC;
                        } while (!getBin(res) && !BREAK);
                        break;

                    default: goto err_tok;
                }

                break;

            // while .. do
            case C_WHL:

                if (type != C_CDO) goto err_tok;
                goto whl_do;

                while (getBin(res) && !BREAK) {
                    handleScope(getFncRaw(it[1]));
                    if (status == S_CONTINUE) status = S_EXEC;
                    if (BREAK) break;
                whl_do:
                    REPVAR(res, handleLine(getTrmRaw(*it)));
                    if (status == S_CONTINUE) status = S_EXEC;
                }

                if (status == S_BREAK) status = S_EXEC;
                break;

            // until .. do
            case C_UNT:
                if (type == C_CDO) {
                    goto unt_do;

                    while (!getBin(res) && !BREAK) {
                        handleScope(getFncRaw(it[1]));
                        if (status == S_CONTINUE) status = S_EXEC;
                        if (BREAK) break;
                    unt_do:
                        REPVAR(res, handleLine(getTrmRaw(*it)));
                        if (status == S_CONTINUE) status = S_EXEC;
                    }
                } else
                    goto err_tok;

                if (status == S_BREAK) status = S_EXEC;
                break;

            default: error_exit("invalid line %s", TOSTR(line));
        }

        END(" %s", TOSTR(funcResult));
        return funcResult;

    } // other statements
    else {
        it++;
        PVar args;
        res = incRef(evalExpr(*begin, false));

        do {
            if (size > 2) {
                // check wether operator available
                if (!hasOperator(res, getOprRaw(it[0])))
                    error_exit(
                        "%s has no operator %s", getTypeName(res),
                        TOSTR(it[0]));

                args      = incRef(evalExpr(it[1], true)); // get 2nd arg
                PVar tRes = res; // for safe overriding & freeing
                res       = incRef(CALLOPR(res, getOprRaw(it[0]), args));

                decRef(tRes); // delete previous result
                decRef(args); // delete copied args
            } else {
                REPVAR(res, it[0]);
                break;
            }

            it += 2;
            size -= 2;
        } while (it != end);

        END(" %s", TOSTR(res));
        return REPVAR(funcResult, res);
    }


err_tok:
    if (firstType == T_NIL)
        error_exit("unexpected token %s", typeName(type));
    else
        error_exit(
            "unexpected token %s in %s statement", typeName(type),
            typeName(firstType));
    return newNil();
}

// scope interpreter
void handleScope(var_lst& scope) {
    BEGIN("var_lst*scope");
    for (auto& line: scope) {
        handleLine(getTrm(line));
        if (BREAK) break;
    }
    END();
}

// converts scope string to term
var_lst toFunction(string::iterator& c, char separator, char end) {
    // BEGIN("string::it*c='%c',sep='%c',end='%c'", *c, separator, end);

    var_lst block;
    var_lst line;

    uint8_t lastType = T_NIL;
    string word;

    auto addVar = [&line, &lastType](PVar v, bool dbg = true) {
        if (!v->refcnt) incRef(v);
        if (!v->isConst) error_exit("err!\n"), getchar();
        lastType = getType(v);
        line.push_back(v);
        // if (dbg) DEBUG("\033[1;33m%s: %s", TOSTR(v), typeName(lastType));
    };

    while (*c != end && *c != ')' && *c != ']' && *c != '}') {
        word = "";

        switch (*c) {
            case '"': {
                while (*++c != '"' && *c != end) word += *c;
                // DEBUG("\"%s\": %s", word.c_str(), typeName(T_STR));

                if (lastType == T_STR)
                    getStr(line.back()) += unescape(word);
                else
                    addVar(newStrC(unescape(word)));
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
                    addVar(newOprC("call"));
                } else if (
                    type == T_LST && (lastType == T_LST || lastType == T_OBJ)) {
                    type = T_TRM;
                    addVar(newOprC("at"));
                }

                if (lastType >= KCNT) {
                    if (type == keyType(lastType))
                        type = lastType;
                    else
                        error_exit(
                            "expected %s got %s\nignored %s control",
                            typeName(type), keyTypeName(lastType),
                            typeName(lastType));
                }

                // DEBUG("%c: %s begin", *c, typeName(type));
                ++c;
                var_lst list = toFunction(c, sep, end);
                // DEBUG("%c: %s end", *c, typeName(type));

                if (type == T_TRM && list.size() == 1 &&
                    getType(list[0]) < TCNT)
                    addVar(list[0]);
                else
                    addVar(NEWVAR(TLst(list, type, true)));

            } break;

            default: {
                // separator -> end current line, begin new
                if (*c == separator) {
                    if (line.size() == 1 && getType(line[0]) < TCNT)
                        block.push_back(line[0]);
                    else
                        block.push_back(incRef(newTrmC(line)));

                    line.clear();
                    lastType = T_NIL;
                    word += *c++;
                }
                // operators
                else if (
                    !line.empty() && (getType(line.back()) != T_OPR) &&
                    isOperator(*c) &&
                    (*c != '.' || !(isdigit(c[-1]) || isdigit(c[1]))))
                // exclude +-.numbers. from operators
                {
                    do {
                        word += *c;
                    } while (isOperator(word + *++c) && *c != end);

                    addVar(newOprC(operators.at(word).name));
                }
                // keywords, (builtin) functions, vars, control statements, ...
                else {
                    do {
                        word += *c++;
                    } while (
                        *c != end &&
                        (isAlnum(*c) ||
                         // prevent floating point to be "at" operator
                         (*c == '.' && (isdigit(c[-1]) || isdigit(c[1])))));

                    if (*c == ' ') *c++;

                    // number
                    if (!isAlpha(word[0])) {
                        if ((long)word.find(".") == -1)
                            addVar(newIntC(stod2(word)));
                        else
                            addVar(newFltC(stod2(word)));
                    }
                    // subfunction
                    else if (lastType == T_OPR && getOpr(line.back()) == "at") {
                        getOpr(line.back()) = word;
                        setType(line.back(), T_MFN);
                        lastType = T_MFN;
                    }
                    // builtin
                    else if (isBuiltin(word.c_str())) {
                        addVar(newBfnC(getBltin(word)));
                    }
                    // keyword
                    else {
                        auto kwtype = keywords.find(word);
                        // invalid
                        if (kwtype == keywords.end())
                            error_exit("unexpected token %s", word.c_str());

                        switch (kwtype->second) {
                            case K_NIL: addVar(newNilC()); break;
                            case K_TRU: addVar(newBinC(true)); break;
                            case K_FLS: addVar(newBinC(false)); break;
                            case K_BRK:
                                addVar(NEWVAR(TChr(0, K_BRK, true)));
                                break;
                            case K_CNT:
                                addVar(NEWVAR(TChr(0, K_CNT, true)));
                                break;
                            case K_RET: {
                                // DEBUG("return begin");
                                var_lst list = toFunction(c, ',', ';');
                                // DEBUG("return end");
                                addVar(NEWVAR(TLst(list, K_RET, true)));
                                break;
                            }
                            default: {
                                lastType = kwtype->second;
                                /* DEBUG("keyword %i %s",
                                    lastType, typeName(lastType)); */
                            }
                        }
                    }
                }

                // if (lastType != T_NIL)
                // //    DEBUG("%s: %s", word.c_str(), typeName(lastType));
                continue;
            }
        }
        ++c;
    }

    if (*c != end) {
        block.clear();
        // END();
        if (!end)
            error_exit("invalid token '%c' - expected EOF", *c);
        else
            error_exit("invalid token '%c' - expected '%c'", *c, end);
    }

    // if single value dont push list
    if (line.size() == 1 && getType(line[0]) < TCNT) {
        block.push_back(line[0]);
    }
    // dont push if empty line
    else if (!line.empty()) {
        block.push_back(incRef(newTrmC(line)));
    }

    // END("%s", TOSTR(block));
    return block;
}

// start recursive conversation from string to term
var_lst toCode(string& code) {
    BEGIN("string*code=\"%s\"", code.c_str());
    string::iterator c = code.begin();
    END();
    return toFunction(c, ';', *code.end());
}
