
#include "builtins.hpp"

// clang-format off

#define newFunc(NAME, ...)                   \
    { NAME, {                                \
        .name = NAME,                        \
        .func = [](var_lst args) __VA_ARGS__ \
    }}

unordered_map<string, TBltFunc> builtins;

void initBuiltins() {
    builtins = { // builtin functions
        newFunc( "print", {
            BEGIN("print args=%s", TLst(args).toStr().c_str());

            if (!args.empty()) {
                EVALARGS(args, {});

                for (PVar& v: args) printf("%s", TOSTR(v));
            }
            END("print");
            return V_NULL;
        }),

        newFunc( "println", {
            BEGIN("println args=%s", TLst(args).toStr().c_str());

            if (!args.empty()) {
                EVALARGS(args, {});
                for (PVar& v: args) printf("%s\n", TOSTR(v));

            } else
                printf("\n");

            END("println");
            return V_NULL;
        }),

        newFunc( "input", {
            BEGIN("input args=%s", TLst(args).toStr().c_str());

            if (!args.empty()) {
                EVALARGS(args, {});
                printf("%s", TOSTR(args[0]));
            }

            string input;
            getline(std::cin, input);
            END("input");
            return NEWVAR(TStr(input));
        }),

        newFunc( "delay", {
            BEGIN("delay args=%s", TLst(args).toStr().c_str());

            if (!args.empty()) {
                EVALARGS(args, {});
                uint8_t type = getType(args[0]);

                if (type == T_INT)
                    delay(getInt(args[0]));
                else if (type == T_FLT)
                    delay(getFlt(args[0]));
                else if (type == T_STR)
                    delay(stod2(getStr(args[0])));
            }
            END("delay");
            return V_NULL;
        }),

        newFunc( "clock", {
            BEGIN("clock args=%s", TLst(args).toStr().c_str());
            END("clock");
            return NEWVAR(TFlt(clock() / 1000.0));
        }),

        newFunc( "typeof", {
            BEGIN("typeof args=%s", TLst(args).toStr().c_str());
            END("typeof");

            if (!args.empty()) {
                EVALARGS(args, {V_NULL});
                return NEWVAR(TStr(getTypeName(args[0])));
            } else
                return NEWVAR(TStr(typeName(-1)));
        }),

        newFunc( "toString", {
            BEGIN("typeof args=%s", TLst(args).toStr().c_str());
            END("toString");

            if (!args.empty()) {
                EVALARGS(args, {});
                return NEWVAR(TStr(escape(args[0]->toStr())));
            } else
                return NEWVAR(TStr(""));
        })
    };
}

// clang-format on

// returns wether builtin name exists
bool isBuiltin(const char* s) {
    BEGIN("s=\"%s\"", s);
    END();
    return builtins.find(s) != builtins.end();
}
