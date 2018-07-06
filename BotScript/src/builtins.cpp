
#include "builtins.hpp"

#define cstr const char*

#define SEP ,

#define newFunc(NAME, ARGC, DFLT, ...)                \
    {                                                 \
        NAME, {                                       \
            .dflt = DFLT, .name = NAME, .argc = ARGC, \
            .func = [](var_lst params) __VA_ARGS__    \
        }                                             \
    }

unordered_map<string, TBltFnc> builtins;

void initBuiltins() {
    builtins = {
        // builtin functions
        newFunc(
            "print", 1, {NEWVAR(TStr(""))},
            {
                BEGIN("print params=%s", TLst(params).toStr().c_str());
                PVar end = params.end()[-1];
                for (PVar& v: params) {
                    if (getType(v) == T_STR)
                        printf("%s", unescape(getStr(v)).c_str());
                    else
                        printf("%s", TOSTR(v));
                    if (v != end) printf(" ");
                }
                END("print");
                return V_NULL;
            }),

        newFunc(
            "println", 1, {NEWVAR(TStr(""))},
            {
                BEGIN("println params=%s", TLst(params).toStr().c_str());
                for (PVar& v: params) {
                    if (getType(v) == T_STR)
                        printf("%s\n", unescape(getStr(v)).c_str());
                    else
                        printf("%s\n", TOSTR(v));
                }
                END("println");
                return V_NULL;
            }),

        newFunc(
            "input", 1, {NEWVAR(TStr(""))},
            {
                BEGIN("input params=%s", TLst(params).toStr().c_str());

                if (getType(params[0]) == T_STR)
                    printf("%s", unescape(getStr(params[0])).c_str());
                else
                    printf("%s", TOSTR(params[0]));

                string input;
                getline(std::cin, input);
                END("input");
                return NEWVAR(TStr(input));
            }),

        newFunc(
            "delay", 1, {NEWVAR(TInt(0))},
            {
                BEGIN("delay params=%s", TLst(params).toStr().c_str());
                if (params.size()) {
                    uint8_t type = getType(params[0]);

                    if (type == T_INT)
                        delay(getInt(params[0]));
                    else if (type == T_FLT)
                        delay(getFlt(params[0]));
                    else if (type == T_STR)
                        delay(stod2(getStr(params[0])));
                }
                END("delay");
                return V_NULL;
            }),

        newFunc("clock", 0, {}, {
            BEGIN("clock params=%s", TLst(params).toStr().c_str());
            PVar ret = NEWVAR(TFlt(clock() / 1000.0));
            END("clock");
            return ret;
        })};
}

// returns wether builtin name exists
bool isBuiltin(const char* s) {
    BEGIN("s=\"%s\"", s);
    END();
    return builtins.find(s) != builtins.end();
}
