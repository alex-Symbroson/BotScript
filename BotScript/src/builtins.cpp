
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
                        printf("%s", v->toStr().c_str());
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
                        printf("%s\n", v->toStr().c_str());
                }
                END("println");
                return V_NULL;
            }),

        newFunc(
            "input", 1, {NEWVAR(TStr(""))},
            {
                BEGIN("input params=%s", TLst(params).toStr().c_str());
                printf("%s\n", params[0]->toStr().c_str());
                string inp;
                cin >> inp;
                END("input");
                return (new TStr(inp))->getVar();
            }),

        newFunc(
            "delay", 1, {NEWVAR(TInt(0))},
            {
                BEGIN("delay params=%s", TLst(params).toStr().c_str());
                if (params.size()) {
                    uint8_t type = getType(params[0]);
                    if (type == T_INT)
                        delay(getInt(params[0]));
                    else if (type == T_STR)
                        delay(stoll(getStr(params[0])));
                }
                END("delay");
                return V_NULL;
            }),

        newFunc("clock", 0, {}, {
            BEGIN("clock params=%s", TLst(params).toStr().c_str());
            PVar ret = (new TInt(clock() / 1000))->getVar();
            END("clock");
            return ret;
        })};
}

// returns wether builtin name exists
bool builtin_exists(const char* s) {
    BEGIN("s=\"%s\"", s);
    END();
    return builtins.find(s) != builtins.end();
}
