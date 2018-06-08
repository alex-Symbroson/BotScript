
#include "interpret.hpp"

PVar callBuiltin(var_bfn func, var_lst& args) {
    DEBUG("calling builtin %s %s", func->name, TLst(args).toStr().c_str());
    /*
        // fill minimum arguments
        int16_t i = func->argc - args.size();
        while (i-- >= 0) printf("fill args\n"); // args->push_back(V_NULL);

        printf("wait enter");
        wait_enter();

        // evaluate arguments
        i = args.size();
        while (i--)
            if (getType(args[i]) == T_TRM)
                (*args)[i] = handleScope(getLstP(args[i]));
    */
    var_lst fargs = {};
    if (args.size()) fargs.push_back(handleScope(args));

    // call function
    return func->func(fargs);
}

// scope interpreter
PVar handleScope(var_lst& scope) {
    BEGIN("var_lst*scope");
    DEBUG("interpreting %s", TLst(scope).toStr().c_str());
    /*
        var_lst::iterator it = scope.begin();
        if (scope.size() == 1) return scope[0];
        if (scope.size() == 2) return callP(it[0], getStr(it[1]));
        return callP(it[0], getStr(it[1]), it[0], it[2]);
    */
    END();
    return V_NULL;
}

// converts scope string to term
var_lst toFunction(string::iterator& c, string::iterator end, char separator) {
    BEGIN("string::it*c=\"%c\",string::it end", *c);

    var_lst block;

    TLst* vline   = new TLst({});
    var_lst* line = getLstP(vline);
    block.push_back(vline->getVar());

    uint8_t lastType = 0;

    while (*c != ')' && *c != '}' && c < end) {
        switch (*c) {
            case '"': {
                string word = "";
                while (*++c != '"' && c < end)
                    word += *c;
                line->push_back((new TStr(format(word))));
                DEBUG("%s: %s", word.c_str(), typeName(T_STR));
                lastType = T_STR;
            } break;

            case '(':
            case '{': {
                uint8_t type;
                char sep;

                if (*c == '(') {
                    type = T_TRM;
                    sep  = ',';
                } else {
                    type = T_FNC;
                    sep  = ';';
                }
                DEBUG("%c: %s begin", *c, typeName(type));

                if (type == T_TRM && lastType == T_BFN) {
                    line->push_back((new TStr("call")));
                }

                ++c;
                line->push_back(
                    (new TLst(toFunction(c, end, sep), type))->getVar());
                DEBUG("%c: %s end", *c, typeName(type));
                lastType = type;
            } break;

            default:
                string word = "";

                if (symbols.find(*c) + 1) {
                    string symbol;
                    do {
                        symbol += *c;
                    } while (symbols.find(*++c) + 1 && c < end);
                    DEBUG("symbol \"%s\"", symbol.c_str());
                    if (symbol[0] == separator) {
                        vline = new TLst({});
                        line  = getLstP(vline);
                        block.push_back(vline->getVar());
                    }
                }

                do {
                    word += *c;
                } while (!(symbols.find(*++c) + 1) && c < end);

                if (builtin_exists(word.c_str())) {
                    line->push_back((new TBfn(getBltin(word)))->getVar());
                    DEBUG("%s: %s", word.c_str(), typeName(T_BFN));
                    lastType = T_BFN;
                } else {
                    DEBUG("%s: %s", word.c_str(), typeName(T_VAR));
                    lastType = T_VAR;
                }
                continue;
        }
        ++c;
    }
    // END("%s", TLst(block).toStr().c_str());
    return block;
}

// start recursive conversation from string to term
var_lst toCode(string& code) {
    BEGIN("string*code=\"%s\"", code.c_str());
    string::iterator c = code.begin();
    END();
    return toFunction(c, code.end());
}
