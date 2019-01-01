
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
            fflush(stdout);
            FEND("print", "null");
        }),

        DEFFUNC( "println", {
            if (!args.empty())
                for (PVar& v: args) printf("%s\n", TOSTR(v));

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

        DEFFUNC( "random", {
                // clang-format on
                var_int res = 0;
                srand(clock() + rand());

                switch (args.size()) {
                case 1: res = rand() % getInt(args[0]); break;
                case 2:
                    res = getInt(args[0]) +
                          rand() % (getInt(args[1]) - getInt(args[0]) + 1);
                    break;
                default: err_iac("random", args, 1);
                }

                FEND("random", "%i", res);
                return newInt(res);
                // clang-format off
        }),

        DEFFUNC( "typeof", {
            if (args.size() == 1) {
                FEND("typeof", "%s", getTypeName(args[0]));
                return newStr(getTypeName(args[0]));
            } else
                err_iac("typeof", args, 1);
        }),

        DEFFUNC( "toString", {
            if (args.size() == 1) {
                FEND("toString", "%s", args[0]->toString(true).c_str());
                return newStr(args[0]->toString(true));
            } else
                err_iac("toString", args, 1);
        }),

        DEFFUNC( "stop", {
            status = S_STOP;
            if (args.size() == 1)
                return args[0];
            else return newInt(0);
        })

#if ISBOT
            ,
        DEFBOTFUNC( "BOT_IsBot", {
            if(!args.size())
                RaspiBot::Call("isBot", args);
            else err_iac("BOT_IsBot", args, 1);
        }),

        DEFBOTFUNC( "Bot_Write", {
            size_t size = args.size();
            if(size >= 1) {
                assertT(args[0], T_STR);     // text
                if (size == 3) {
                    assertT(args[1], T_INT); // x
                    assertT(args[2], T_INT); // y
                }
                RaspiBot::Call("write", args);
            } else if(size != 1)
                err_iac("Bot_write", args, 1);
        }),

        // Serial (Attiny)
        DEFBOTFUNC( "Bot_GetEncoders", {
            if (args.empty())
                RaspiBot::Call("getEncoders", args);
            else err_iac("Bot_GetEncoders", args, 0);
        }),

        DEFBOTFUNC( "Bot_ResetEncoders", {
            if (args.empty())
                RaspiBot::Call("resetEncoders", args);
            else err_iac("Bot_ResetEncoders", args, 0);
        }),

        DEFBOTFUNC( "Bot_StopMotors", {
            if (args.empty())
                RaspiBot::Call("stopMotors", args);
            else err_iac("Bot_StopMotors", args, 0);
        }),

        DEFBOTFUNC( "Bot_SetMotors", {
            size_t size = args.size();
            if (size == 2) {
                getInt(args[0]) *= 1.27; // left  [-100, 100] -> [-127, 127]
                getInt(args[1]) *= 1.27; // right [-100, 100] -> [-127, 127]
                RaspiBot::Call("setMotors", args);
            } else
                err_iac("Bot_SetMotors", args, 2);
        }),

        DEFBOTFUNC( "Bot_SetBuzzer", {
            size_t size = args.size();
            if (size == 3) {
                assertT(args[0], T_INT); // frequency (0, 65535)
                assertT(args[1], T_INT); // duration (0, 65535) ms

                // volume [0, 100] -> [0, 15]
                if(getType(args[2]) == T_FLT) {
                    var_flt vol = getFltRaw(args[2]);
                    REPVAR(args[2], newInt(vol * 0.15));
                } else
                    getInt(args[2]) *= 0.15;

                RaspiBot::Call("setBuzzer", args);
            } else
                err_iac("Bot_SetBuzzer", args, 3);
        }),

        DEFBOTFUNC( "Bot_StopBuzzer", {
            if (args.empty())
                RaspiBot::Call("stopBuzzer", args);
            else err_iac("Bot_StopBuzzer", args, 0);
        }),

        // Buttons
        DEFBOTFUNC( "Bot_SetRedLED", {
            size_t size = args.size();
            if (size == 2) {
                assertT(args[0], T_INT); // led [1, 3]
                // level [0, 100] -> [0, 255]
                if(getType(args[1]) == T_INT)
                    getIntRaw(args[1]) *= 2.55;
                else
                    getFlt(args[1]) *= 2.55;

                RaspiBot::Call("setRedLED", args);
            } else
                err_iac("Bot_SetRedLED", args, 2);
        }),

        DEFBOTFUNC( "Bot_SetGreenLED", {
            size_t size = args.size();
            if (size == 2) {
                assertT(args[0], T_INT); // [1, 3]
                // level [0, 100] -> [0, 255]
                if(getType(args[1]) == T_INT)
                    getIntRaw(args[1]) *= 2.55;
                else
                    getFlt(args[1]) *= 2.55;

                RaspiBot::Call("setGreenLED", args);
            } else
                err_iac("Bot_SetGreenLED", args, 2);
        }),

        DEFBOTFUNC( "Bot_WaitForBtnPress", {
            if (args.size() == 1) {
                assertT(args[1], T_INT);
                RaspiBot::Call("waitForBtnPress", args);
            } else
                err_iac("waitForBtnPress", args, 1);
        }),

        DEFBOTFUNC( "Bot_WaitForBtnRelease", {
            if (args.size() == 1) {
                assertT(args[1], T_INT);
                RaspiBot::Call("waitForBtnRelease", args);
            } else
                err_iac("Bot_WaitForBtnRelease", args, 1);
        }),

        DEFBOTFUNC( "Bot_WaitForBtn", {
            if (args.size() == 1) {
                assertT(args[1], T_INT);
                RaspiBot::Call("waitForBtn", args);
            } else
                err_iac("Bot_WaitForBtn", args, 1);
        }),

        DEFBOTFUNC( "Bot_IsBtnPressed", {
            if (args.size() == 1) {
                assertT(args[1], T_INT);
                RaspiBot::Call("isBtnPressed", args);
            } else
                err_iac("Bot_IsBtnPressed", args, 1);
        }),

        DEFBOTFUNC( "Bot_GetSharp", {
            if (args.size() == 1) {
                assertT(args[1], T_INT);
                RaspiBot::Call("getSharp", args);
            } else
                err_iac("Bot_GetSharp", args, 1);
        }),

        DEFBOTFUNC( "Bot_GetBattery", {
            if (args.size() == 1) {
                assertT(args[1], T_INT);
                RaspiBot::Call("getBattery", args);
            } else
                err_iac("Bot_GetBattery", args, 1);
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
