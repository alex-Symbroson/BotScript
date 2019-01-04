
/* TODO
    IR-Sensors -> cm
    Encoder -> cm
    check btn id, ir id, motor id
    battery
*/


#include "include.hpp"
#include "interpret.hpp"

#if ISBOT
#    include "RaspiBot.hpp"
#endif

bool debg        = false;
uint debug_depth = 0;
uint8_t status   = 0;


bool Init() {
    status = S_INIT;
    srand(clock()); // randomize

    INFO("init operations");
    if (initOperations()) return true;

    INFO("init builtins");
    if (initBuiltins()) return true;

#if ISBOT
    INFO("init RaspiBot");
    if (RaspiBot::Init()) return true;
#endif

    return false;
}

void Free() {
    BEGIN();
    status = S_FREE;

#if _INFO_
    int alloc = collector.size();
#endif

#if ISBOT
    INFO("freeing RaspiBot");
    RaspiBot::Free();
    INFO("freed " FMT_SIZE " vars", alloc - collector.size());
#endif

    INFO("freeing collector (%i)", alloc);
    FreeVariables();
    INFO("freed " FMT_SIZE " of %i vars", alloc - collector.size(), alloc);
    END();
}

void interrupt(int sig) {
    error("\nkeyboard interrupt");
    Exit();
    printf("\n");
}

// initialisation
int main(int argc, char* argv[]) {
    BEGIN("argc=%i,argv", argc);
    PRINT_STATUS();

    signal(SIGINT, interrupt);

    if (Init()) {
        error("Initialization failed");
        Exit();
        return 1;
    }
    INFO("variables: " FMT_SIZE, collector.size());

    int res = 0;

    if (argc > 1) {
        INFO("reading file");
        FILE* input = fopen(argv[1], "r");
        string code = readFile(input, true);
        INFO("file input:\n%s\n", code.c_str());

#if _INFO_
        uint alloc = collector.size(), delloc;
#endif

        // create code scope of content from default or argument file path
        INFO("formatting code");
        status       = S_FORMAT;
        var_fnc main = toCode(code);
        INFO("allocated %u variables", alloc = collector.size() - alloc);

        INFO("\nglobal: %s\n", TOSTR(main.func));

        // create var_lst of program (sys)args
        var_lst vargs = {incRef(newStrC(argv[1]), 2)};
        for (int i = 2; i < argc; i++)
            vargs.push_back(incRef(newStrC(argv[i]), 2));

        // execute code
        INFO("execute global");
        status     = S_EXEC;
        funcResult = incRef(newNil());
        handleFunc(newFnc(main), newArgC(vargs));

        INFO("execute init");
        if (status == S_RETURN) status = S_EXEC;
        PVar func = findVar("init", &main, false);
        if (func && getType(func) == T_FNC) handleFunc(func, newArgC({}));

        INFO("execute main");
        if (status == S_RETURN) status = S_EXEC;
        func = findVar("main", &main, false);
        if (func && getType(func) == T_FNC) handleFunc(func, newArgC(vargs));

        INFO("execute loop");
        if (status == S_RETURN) status = S_EXEC;
        func = findVar("loop", &main, false);
        if (func && getType(func) == T_FNC)
            while (status < S_STOP) handleFunc(func, newArgC({}));

        INFO("freeing code");
        status = S_FREE;
        for (PVar& v: main.func) delete v;
        INFO("freed");

        INFO("stop program");
        switch (getBaseType(funcResult)) {
        case T_INT: res = getInt(funcResult); break;
        case T_FLT: res = getFlt(funcResult); break;
        case T_NIL: break;
        default:
            error_exit("expected numeric exit code. got %s", baseTypeName(res));
            break;
        }

        decRef(funcResult);
    } else { /*
         while (status != S_STOP) {
             INFO("reading file");
             printf("> ");
             fflush(stdout);
             string code = readFile(stdin, true);

             // create code scope of content from default or argument file path
             INFO("formatting code");
             status       = S_FORMAT;
             var_fnc main = toCode(code);
             INFO("allocated %u variables", alloc = collector.size() - alloc);

             INFO("\nline: %s\n", TOSTR(main.func));
             // execute code
             INFO("execute line");
             status     = S_EXEC;
             funcResult = incRef(newNil());
             handleFunc(newFnc(main), newArgC({}));

             printf("< %s\n", TOSTR(funcResult));

             INFO("freeing code");
             for (PVar& v: main.func) delete v;
             INFO("freed");

             decRef(funcResult);
         }
         */
        error("no input file");
        res = 1;
    }

    // free all allocated variables
    status = S_FREE;
    Free();

    INFO("end.");
    END("%i", res);

    return res;
}
