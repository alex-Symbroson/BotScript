
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

PVar callBuiltin(var_bfn func, var_lst& args) {
    BEGIN("func=%s, args=%s", func->name, TLst(args).toStr().c_str());

    // fill minimum arguments
    int16_t i;

    for (i = args.size(); i < func->argc; i++)
        args.push_back(func->dflt[i]);

    // evaluate arguments
    i = args.size();
    var_lst fargs(i);

    while (i--) {
        if (getType(args[i]) == T_LST)
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
    uint32_t size = line.size();

    if (size == 1) {
        END("-> %s", line[0]->toStr().c_str());
        return line[0];
    }

    if (size) {
        var_lst::iterator begin = line.begin(), it, end = line.end();
        it       = begin + 1;
        PVar res = NULL;

        do {
            if (size > 2 && getType(it[1]) == T_TRM)
                res = callP(begin[0], getStr(it[0]), begin[0], it[1]);
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

    if (scope.size()) {
        var_lst::iterator it = scope.begin(), end = scope.end();
        do {
            handleLine(getLst(*it));
            it++;
        } while (it != end);
    }
    END();
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

            // case '[':
            case '(':
            case '{': {
                uint8_t type;
                char sep;

                if (*c == '(') {
                    type = T_TRM;
                    sep  = ',';
                } else /*if (*c == '{')*/ {
                    type = T_FNC;
                    sep  = ';';
                } /* else if (*c == '[') {
                     type = T_OBJ;
                     sep  = ',';
                 }*/
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
