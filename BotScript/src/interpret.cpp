
#include "interpret.hpp"

// clang-format off
#define Op(SYM, NAM, PRI, DIR) { SYM, { NAM, PRI, DIR } }
// clang-format on

// all operators
const unordered_map<string, Operator> operators = {
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

// keyword types
const unordered_map<string, const uint8_t> keywords = {
    {"null", K_NIL},     {"true", K_TRU},   {"false", K_FLS},  {"break", K_BRK},
    {"continue", K_CNT}, {"return", K_RET}, {"if", C_CIF},     {"elif", C_EIF},
    {"else", C_ELS},     {"do", C_CDO},     {"while", C_WHL},  {"until", C_UNT},

    {"bool", T_BIN},     {"int", T_INT},    {"float", T_FLT}, // datatypes
    {"list", T_LST},     {"object", T_OBJ}, {"string", T_STR}, {"pin", T_PIN},
    {"func", T_FNC} /* , {"var", T_NIL} */
};

// value type after keyword
const uint8_t CtrlType[CCNT - KCNT] = {T_TRM, T_TRM, T_FNC,
                                       T_FNC, T_TRM, T_TRM};

// func return value
PVar funcResult;
var_fnc* curScope;

// execute single code line
PVar handleLine(var_lst& line) {
    BEGIN("line=%s", TOSTR(line));
    // reset return value
    REPVAR(funcResult, incRef(newNil()));

    if (line.empty()) {
        END("0 null");
        return funcResult;
    }

    // res: cur val, it[0]: 1st operand, it[1]: operator, it[2]: 2nd operand
    PVar res;
    uint32_t size           = line.size();
    var_lst::iterator begin = line.begin(), it = begin, end = line.end();
    uint8_t type = getType(*it), firstType = T_NIL;

    // handle single-parted control structures
    if (size == 1) {
        if (IDisType(type))
            REPVAR(funcResult, evalExpr(*it, false));
        else {
            switch (type) {
            // keywords
            case K_BRK: status = S_BREAK; break; // break
            case K_RET:                          // return
                if (size != 1) goto err_tok;
                status = S_RETURN;
                REPVAR(funcResult, evalExpr(*it, false));
                break;

            case K_CNT: status = S_CONTINUE; break; // continue
            default:
                // handle multi-part control statements
                if (type == C_WHL || type == C_UNT || type == C_CDO) goto ctrl;
                goto err_tok;
            }
        }

        END(" %s", TOSTR(funcResult));
        return funcResult;
    }

ctrl:
    // handle multi-parted control structures
    if (type >= KCNT) {
        res       = incRef(newNil());
        firstType = type;
        type      = size == 1 ? T_NIL : getType(it[1]);

        switch (firstType) {
        // if .. elif .. else
        case C_CIF:
            // find statement where condition is true
            do {
                type = getType(*it);
                // handle if and elif
                if (type == C_CIF || type == C_EIF) {
                    // parse condition
                    REPVAR(res, handleLine(getTrmRaw(*it)));
                    DEBUG("%s -> %s", TOSTR(*it), TOSTR(res));

                    it++; // go to next block

                    // if condition true -> break
                    if (getBin(res)) break;
                } else if (type == C_ELS) // else
                    break;
                else
                    goto err_tok;

                if (it == end) break;
                it++; // go to next block
            } while (it != end);

            // execute scope
            if (it != end) {
                // DEBUG("%s", TOSTR(*it));
                type = getType(*it);
                if (type == C_CDO || type == C_ELS)
                    handleScope(*it);
                else
                    goto err_tok;
            }

            break;

        // do [ .. while | until ]
        case C_CDO:
            switch (type) {
            // do
            case T_NIL: handleScope(*it); break;

            // do .. while
            case C_WHL:
                do {
                    handleScope(*it); // execute do{} block
                    // reset control flags
                    if (status == S_CONTINUE) status = S_EXEC;
                    if (BREAK) break;
                    // get condition result
                    REPVAR(res, handleLine(getTrmRaw(it[1])));
                    if (status == S_CONTINUE) status = S_EXEC;
                } while (getBin(res) && !BREAK);
                break;

            // do .. until
            case C_UNT:
                do {
                    handleScope(*it);
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

            if (!type && type != C_CDO) goto err_tok;
            goto whl_do;

            while (getBin(res) && !BREAK) {
                // allow while() without do{} block
                if (type) {
                    handleScope(it[1]);
                    if (status == S_CONTINUE) status = S_EXEC;
                    if (BREAK) break;
                }
            whl_do:
                REPVAR(res, handleLine(getTrmRaw(*it)));
                if (status == S_CONTINUE) status = S_EXEC;
            }

            if (status == S_BREAK) status = S_EXEC;
            break;

        // until .. do
        case C_UNT:
            if (!type && type != C_CDO) goto err_tok;
            goto unt_do;

            while (!getBin(res) && !BREAK) {
                if (type) {
                    handleScope(it[1]);
                    if (status == S_CONTINUE) status = S_EXEC;
                    if (BREAK) break;
                }
            unt_do:
                REPVAR(res, handleLine(getTrmRaw(*it)));
                if (status == S_CONTINUE) status = S_EXEC;
            }

            if (status == S_BREAK) status = S_EXEC;
            break;

        default: error_exit("invalid line %s", TOSTR(line));
        }

        END(" %s", TOSTR(funcResult));
        return funcResult;

    } // interpret other statements
    else {
        it++;
        PVar args;
        // get first operand
        res = incRef(evalExpr(*begin, false));

        do {
            if (size > 2) {
                // check whether operator available
                if (!hasOperator(res, getOprRaw(it[0])))
                    error_exit(
                        "%s has no operator %s", getTypeName(res),
                        TOSTR(it[0]));

                DEBUG("ARGS: %s", TOSTR(it[1]));
                args      = incRef(evalExpr(it[1], true)); // get 2nd operand
                PVar tRes = res; // for safe overriding & freeing

                // execute operator
                DEBUG("(%s %s %s)", TOSTR(tRes), TOSTR(it[0]), TOSTR(args));
                res = incRef(CALLOPR(res, getOprRaw(it[0]), args));
                DEBUG("(... %s ...) -> %s", TOSTR(it[0]), TOSTR(res));

                decRef(tRes); // delete previous result
                decRef(args); // delete copied args
            } else {
                REPVAR(res, it[0]);
                break;
            }

            // jump to next operator
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
void handleScope(PVar scope) {
    BEGIN("var_lst*scope");
    for (auto& line: getFncRaw(scope).func) {
        handleLine(getTrm(line));
        if (BREAK) break;
    }
    END();
}

// scope interpreter
void handleFunc(PVar func, PVar args) {
    BEGIN("var_lst*scope");
    var_fnc* lastScope = curScope;
    curScope           = &getFnc(func);

    if (curScope->vars["args"])
        REPVAR(curScope->vars["args"], args);
    else
        curScope->vars["args"] = incRef(args);

    handleScope(func);
    decRef(args);

    curScope = lastScope;
    END();
}

// converts scope string to term
var_lst toFunction(char*& c, char separator, char end, var_fnc& parent) {
    BEGIN("string::it*c='%c',sep='%c',end='%c'", *c, separator, end);

    var_lst scope, line;

    uint8_t lastType = T_NIL;
    bool decl = false, stop = false; // stop: interrupt on break,return,...
    string word;

    // lambda func to push value to line
    auto addVar = [&line, &lastType, &decl](PVar v, bool dbg = true) {
        if (!v->refcnt) incRef(v);
        lastType = getType(v);
        if (decl && lastType != T_VAR) decl = false;
        line.push_back(v);
        INFO("\033[1;32m%s: %s", TOSTR(v), typeName(lastType));
    };

    // iterate until end of scope
    while (*c != end && *c != ')' && *c != ']' && *c != '}') {
        // ignore statements after break or return
        if (stop) {
            while (*c != end) c++;
            break;
        }

        word = ""; // reset buffer

        // select type depending on start sequence
        switch (*c) {
        case '"': {
            char* begin = ++c;
            while (*c != '"' && *c != EOF) c++;
            // create string by start, length
            word = unescape(string(begin, c - begin));

            if (lastType == T_STR) // concat strings
                getStr(line.back()) += word;
            else
                addVar(newStrC(word));
        } break;

        case '[':
        case '(':
        case '{': {
            uint8_t type = 0;
            char sep = 0, end = 0;

            // select list properties
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

            if (type == T_TRM) {
                // lastType func -> add 'call' opr
                if (lastType == T_FNC || lastType == T_VAR ||
                    lastType == T_BFN || lastType == T_MFN) {
                    type = T_ARG;
                    if (lastType != T_MFN) addVar(newOprC("call"));
                }
                // lastType list/obj -> add 'at' opr
            } else if (
                type == T_LST && (lastType == T_LST || lastType == T_OBJ ||
                                  lastType == T_ARG || lastType == T_VAR)) {
                type = T_TRM;
                addVar(newOprC("at"));
            }

            // lastType control -> curType = expected keyType
            if (lastType >= KCNT) {
                if (type == keyType(lastType))
                    type = lastType;
                else if (
                    type == T_FNC && (lastType == C_CIF || lastType == C_EIF ||
                                      lastType == C_WHL || lastType == C_UNT)) {
                    type = C_CDO;
                } else
                    error_exit(
                        "expected %s got %s\nignored %s control",
                        typeName(type), keyTypeName(lastType),
                        typeName(lastType));
            }

            // scan functions
            c++;
            if (type == T_FNC || (type > KCNT && keyType(type) == T_FNC)) {
                var_fnc scope = {NULL, {{}}, {}, &parent};
                scope.func    = toFunction(c, sep, end, scope);
                addVar(NEWVAR(TFnc(scope, type, true)));

                // scan lists
            } else {
                // DEBUG("%c: %s begin", *c, typeName(type));
                var_lst lst = toFunction(c, sep, end, parent);
                // DEBUG("%c: %s end", *c, typeName(type));

                if (type == T_TRM && lst.size() == 1 && VARisType(lst[0]))
                    addVar(lst[0]);
                else
                    addVar(NEWVAR(TLst(lst, type, true)));
            }

        } break;

        default: {
            // INFO("0 %.*s", 20, c);
            // separator -> end current line, begin new
            if (*c == separator) {
                if (line.size() == 1 && VARisType(line[0])) {
                    if (!decl || getType(line[0]) != T_VAR)
                        scope.push_back(line[0]);
                } else
                    scope.push_back(incRef(newTrmC(line)));

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
                do { word += *c; } while (isOperator(word + *++c) && *c != end);
                // INFO("1 %s", word.c_str());
                addVar(newOprC(operators.at(word).name));
            }
            // keywords, (builtin) functions, vars, control statements, ...
            else {
                char* begin = c++;
                while (*c != end &&
                       (isAlnum(*c) ||
                        // prevent floating point to be "at" operator
                        (*c == '.' && (isdigit(c[-1]) || isdigit(c[1])))))
                    c++;
                word = string(begin, c - begin);
                // INFO("2 %s", word.c_str());
                if (*c == ' ') c++;

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
                    // variable
                    if (kwtype == keywords.end()) {
                        if (lastType == T_VAR) {
                            parent.vars[word] = funcResult;
                            addVar(newVarC(word));
                        } else {
                            if (word == "args" || findVar(word, &parent))
                                addVar(newVarC(word));
                            else
                                error_exit("unexpected token %s", word.c_str());
                        }
                        continue;
                    }

                    switch (kwtype->second) {
                    case K_NIL: addVar(newNilC()); break;
                    case K_TRU: addVar(newBinC(true)); break;
                    case K_FLS: addVar(newBinC(false)); break;
                    case K_BRK:
                        addVar(NEWVAR(TChr(0, K_BRK, true)));
                        stop = true;
                        break;
                    case K_CNT:
                        addVar(NEWVAR(TChr(0, K_CNT, true)));
                        stop = true;
                        break;
                    case K_RET:
                        addVar(NEWVAR(TLst(
                            toFunction(c, ',', ';', parent), K_RET, true)));
                        stop = true;
                        break;

                    default:
                        if (IDisType(kwtype->second)) {
                            decl = true;
                            switch (kwtype->second) {
                            // case T_NIL: funcResult = newNil(); break;
                            case T_BIN: funcResult = newBin(0); break;
                            case T_INT: funcResult = newInt(0); break;
                            case T_FLT: funcResult = newFlt(0); break;
                            case T_STR: funcResult = newStr(""); break;
                            case T_LST: funcResult = newLst({}); break;
                            case T_OBJ: funcResult = newObj({{}}); break;
                            case T_FNC: funcResult = newFnc({}); break;
                            case T_PIN: funcResult = newPin(0); break;
                            }

                            incRef(funcResult);
                            lastType = T_VAR;
                        } else
                            lastType = kwtype->second;
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
        scope.clear();
        // END();
        if (!end)
            error_exit("invalid token '%c' - expected EOF", *c);
        else
            error_exit("invalid token '%c' - expected '%c'", *c, end);
    }

    // if single value dont push list
    if (line.size() == 1 && VARisType(line[0])) {
        if (!decl || getType(line[0]) != T_VAR) scope.push_back(line[0]);
    }
    // dont push if line empty
    else if (!line.empty()) {
        scope.push_back(incRef(newTrmC(line)));
    }

    END("%s", TOSTR(scope));
    return scope;
}

// find variable by name
PVar findVar(string name, var_fnc* scope) {
    do {
        auto var = scope->vars.find(name); // search name in vars object
        // return value if variable found and defined
        if (var != scope->vars.end() && var->second) return var->second;
    } while ((scope = scope->parent));

    error_exit("variable '%s' not found", name.c_str());
    return NULL;
}

// start recursive conversation from string to term
var_fnc toCode(string& code) {
    BEGIN("string*code=\"%s\"", code.c_str());
    var_fnc scope = {"main", {{}}, {}, NULL};
    char* c       = &code[0];
    scope.func    = toFunction(c, ';', *code.end(), scope);
    END();
    return scope;
}
