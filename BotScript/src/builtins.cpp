
#define CUSTOM_BEGIN
#include "builtins.hpp"

#if ISBOT
#    include "RaspiBot.hpp"
#endif


#define BEGIN(...) BEGIN_1("", __func__, __VA_ARGS__)
#define END(...) END_1("", __func__, __VA_ARGS__)

#define FBEGIN(func, ...) BEGIN_1("Blt::", func, __VA_ARGS__)
#define FEND(func, ...) END_1("Blt::", func, __VA_ARGS__)

// clang-format off
#define DEFFUNC(NAME, ...)                              \
    { NAME, {                                           \
            .name = NAME, .func = [](var_lst args) {    \
                FBEGIN(NAME, "%s", TOSTR(args, T_ARG)); \
                __VA_ARGS__                             \
            }                                           \
    } }


unordered_map<string, TBltFunc> builtins;


bool initBuiltins() {
    BEGIN();
    builtins = { // builtin functions
        DEFFUNC("print", {
            if (!args.empty()) {
                FILLARGS(args, {});
                for (PVar& v: args) printf("%s", TOSTR(v));
            }
            FEND("print", "null");
            return newNil();
        }),

        DEFFUNC("println", {
            if (!args.empty()) {
                FILLARGS(args, {});
                for (PVar& v: args) printf("%s\n", TOSTR(v));
            } else
                printf("\n");

            FEND("println", "null");
            return newNil();
        }),

        DEFFUNC("input", {
            if (!args.empty()) {
                FILLARGS(args, {});
                printf("%s", TOSTR(args[0]));
            }

            string input;
            getline(std::cin, input);
            FEND("input", "%s",input.c_str());
            return newStr(input);
        }),

        DEFFUNC("delay", {
            if (!args.empty()) {
                FILLARGS(args, {});
                uint8_t type = getType(args[0]);

                if (type == T_INT)
                    delay(getInt(args[0]));
                else if (type == T_FLT)
                    delay(getFlt(args[0]));
                else if (type == T_STR)
                    delay(stod2(getStr(args[0])));
            }
            FEND("delay", "null");
            return newNil();
        }),

        DEFFUNC("clock", {
            FEND("clock", "%s", dtos2(clock() / 1000.0));
            return newFlt(clock() / 1000.0);
        }),

        DEFFUNC("typeof", {
            if (args.size() == 1) {
                FILLARGS(args, {});
                FEND("typeof", "%s", getTypeName(args[0]));
                return newStr(getTypeName(args[0]));
            } else
                err_iac("typeof", args, 1);
        }),

        DEFFUNC("toString", {
            if (args.size() == 1) {
                FILLARGS(args, {});
                FEND("toString", "%s", args[0]->toString(true).c_str());
                return newStr(args[0]->toString(true));
            } else
                err_iac("toString", args, 1);
        })

#if ISBOT
        ,
        DEFFUNC("Bot_Write", {
            if (!args.empty()) {
                FILLARGS(args, {});

                int size = args.size();
                if(size == 3) {
                    if(getType(args[1]) != T_INT) error_exit("Bot_Write argument 2: expected integer");
                    if(getType(args[2]) != T_INT) error_exit("Bot_Write argument 3: expected integer");
                }

                RaspiBot::Call("write", args);
            }

            FEND("Bot_write");
            return newNil();
        })
#endif
    };

    END("false");
    return false;
}

// clang-format on

// returns wether builtin name exists
bool isBuiltin(const char* s) {
    BEGIN("s=\"%s\"", s);
    END();
    return builtins.find(s) != builtins.end();
}
