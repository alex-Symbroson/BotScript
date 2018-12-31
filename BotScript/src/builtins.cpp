
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
#define DEFFUNC(NAME, ...)                          \
    { NAME, {                                       \
        .name = NAME, .func = [](var_lst args) {    \
            FBEGIN(NAME, "%s", TOSTR(args, T_ARG)); \
            __VA_ARGS__                             \
            return newNil();                        \
    }   }   }

#define DEFBOTFUNC(NAME, ...)                       \
    { NAME, {                                       \
        .name = NAME, .func = [](var_lst args) {    \
            FBEGIN(NAME, "%s", TOSTR(args, T_ARG)); \
            __VA_ARGS__                             \
            FEND(NAME, "%s", TOSTR(RaspiRes));      \
            return RaspiRes;                        \
    }   }   }

unordered_map<string, TBltFunc> builtins;

PVar& RaspiRes = RaspiBot::getRes();

bool initBuiltins() {
    BEGIN();

    builtins = { // builtin functions
        DEFFUNC( "print", {
            if (!args.empty())
                for (PVar& v: args) printf("%s", TOSTR(v));
            FEND("print", "null");
        }),

        DEFFUNC( "println", {
            if (!args.empty())
                for (PVar& v: args) printf("%s", TOSTR(v));
            printf("\n");

            FEND("println", "null");
        }),

        DEFFUNC( "input", {
            if (!args.empty())
                printf("%s", TOSTR(args[0]));

            string input;
            getline(std::cin, input);
            FEND("input", "%s", input.c_str());
            return newStr(input);
        }),

        DEFFUNC( "delay", {
            if (!args.empty()) {
                uint8_t type = getType(args[0]);

                if (type == T_INT)
                    delay(getInt(args[0]));
                else if (type == T_FLT)
                    delay(getFlt(args[0]));
            }
            FEND("delay", "null");
        }),

        DEFFUNC( "clock", {
            FEND("clock", "%s", dtos2(clock() / 1000.0));
            return newFlt(clock() / 1000.0);
        }),

        DEFFUNC( "typeof", {
            if (args.size() == 1) {
                FEND("typeof", "%s", getTypeName(args[0]));
                return newStr(getTypeName(args[0]));
            }
            else err_iac("typeof", args, 1);
        }),

        DEFFUNC( "toString", {
            if (args.size() == 1) {
                FEND("toString", "%s", args[0]->toString(true).c_str());
                return newStr(args[0]->toString(true));
            }
            else err_iac("toString", args, 1);
        }),

        DEFFUNC( "stop", {
            status = S_STOP;
            if (args.size() == 1)
                return args[0];
            else return newInt(0);
        })

#if ISBOT
            ,
        DEFBOTFUNC( "Bot_Write", {
            size_t size = args.size();
            if(size >= 1) {
                assertT(args[0], T_STR);     // text
                if (size == 3) {
                    assertT(args[1], T_INT); // x
                    assertT(args[2], T_INT); // y
                }
            } else if(size != 1) {
                error_exit(
                    "invalid argument count for Bot_write: got " FMT_SIZE "; "
                    "expected 1 to 3", size
                );
            }

            RaspiBot::Call("write", args);
        }),

        // Serial (Attiny)
        DEFBOTFUNC( "Bot_GetEncoders", {
            if (args.empty())
                error_exit(
                    "invalid argument count for Bot_GetEncoders: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("getEncoders", args);
        }),

        DEFBOTFUNC( "Bot_ResetEncoders", {
            if (args.empty())
                error_exit(
                    "invalid argument count for Bot_ResetEncoders: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("resetEncoders", args);
        }),

        DEFBOTFUNC( "Bot_StopMotors", {
            if (args.empty())
                error_exit(
                    "invalid argument count for Bot_StopMotors: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("stopMotors", args);
        }),

        DEFBOTFUNC( "Bot_SetMotors", {
            size_t size = args.size();
            if (size == 2) {
                assertT(args[0], T_INT); // left [-127, 127]
                assertT(args[1], T_INT); // right
            } else {
                error_exit(
                    "invalid argument count for Bot_SetMotors: got " FMT_SIZE "; "
                    "expected 2", size
                );
            }

            RaspiBot::Call("setMotors", args);
        }),

        DEFBOTFUNC( "Bot_SetBuzzer", {
            size_t size = args.size();
            if (size == 3) {
                assertT(args[0], T_INT); // frequency (0, 65535)
                assertT(args[1], T_INT); // duration (0, 65535) ms
                assertT(args[2], T_INT); // volume (0, 15)
            } else {
                error_exit(
                    "invalid argument count for Bot_SetBuzzer: got " FMT_SIZE "; "
                    "expected 3", size
                );
            }

            RaspiBot::Call("setBuzzer", args);
        }),

        DEFBOTFUNC( "Bot_StopBuzzer", {
            if (!args.empty())
                error_exit(
                    "invalid argument count for Bot_StopBuzzer: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("stopBuzzer", args);
        }),

        // Buttons
        DEFBOTFUNC( "Bot_SetRedLED", {
            size_t size = args.size();
            if (size == 2) {
                assertT(args[0], T_INT); // [1, 3]
                assertT(args[1], T_FLT); // [0, 255]
            } else {
                error_exit(
                    "invalid argument count for Bot_SetRedLED: got " FMT_SIZE "; "
                    "expected 2", size
                );
            }

            RaspiBot::Call("setRedLED", args);
        }),

        DEFBOTFUNC( "Bot_SetGreenLED", {
            size_t size = args.size();
            if (size == 2) {
                assertT(args[0], T_INT); // [1, 3]
                assertT(args[1], T_FLT); // [0, 255]
            } else {
                error_exit(
                    "invalid argument count for Bot_SetGreenLED: got " FMT_SIZE "; "
                    "expected 2", size
                );
            }

            RaspiBot::Call("setGreenLED", args);
        }),

        DEFBOTFUNC( "Bot_WaitForBtnPress", {
            if (args.empty())
                error_exit(
                    "invalid argument count for Bot_WaitForBtnPress: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("waitForBtnPress", args);
        }),

        DEFBOTFUNC( "Bot_WaitForBtnRelease", {
            if (args.empty())
               error_exit(
                    "invalid argument count for Bot_WaitForBtnRelease: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("waitForBtnRelease", args);
        }),

        DEFBOTFUNC( "Bot_WaitForBtn", {
            if (args.empty())
                error_exit(
                    "invalid argument count for Bot_WaitForBtn: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("waitForBtn", args);
        }),

        DEFBOTFUNC( "Bot_IsBtnPressed", {
            if (args.empty())
                error_exit(
                    "invalid argument count for Bot_IsBtnPressed: got " FMT_SIZE "; "
                    "expected 0", args.size()
                );

            RaspiBot::Call("isBtnPressed", args);
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
    bool res = builtins.find(s) != builtins.end();
    END("%s", typeName(res ? K_TRU : K_FLS));
    return res;
}
