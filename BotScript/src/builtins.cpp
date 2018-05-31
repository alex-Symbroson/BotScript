
//$ g++ -std=c++11 -O3 -c builtins.cpp

#include "builtins.hpp"

#include <stdlib.h>
#include <time.h>

#include "include.hpp"
#include "macros.hpp"

#define cstr const char*

#define newFunc(NAME, ARGC, ...)                         \
    {                                                    \
        NAME, {                                          \
            NAME, ARGC, [](PVar param[ARGC]) __VA_ARGS__ \
        }                                                \
    }

unordered_map<string, TBltFnc> builtins = { // builtin functions
    newFunc(
        "print", 1,
        {
            BEGIN("PVar param=\"%s\"", param[0]->toStr().c_str());
            printf("%s", param[0]->toStr().c_str());
            END();
            return V_NULL;
        }),

    newFunc(
        "input", 1,
        {
            BEGIN("PVar param=\"%s\"", param[0]->toStr().c_str());
            printf("%s", param[0]->toStr().c_str());
            string inp;
            cin >> inp;
            END();
            return (new TStr(inp))->getVar();
        }),

    newFunc(
        "delay", 1,
        {
            BEGIN("PVar param=\"%s\"", param[0]->toStr().c_str());
            delay(getInt(param[0]));
            END();
            return V_NULL;
        }),

    newFunc("clock", 0, {
        BEGIN("");
        PVar ret = (new TInt(clock() / 1000))->getVar();
        END();
        return ret;
    })};

// returns wether builtin name exists
bool builtin_exists(const char* s) {
    BEGIN("string s=%s", s);
    END();
    return builtins.find(s) != builtins.end();
}
