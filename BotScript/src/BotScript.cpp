
#include "include.hpp"
#include "interpret.hpp"

#if ISBOT
#    include "RaspiBot.hpp"
#endif

bool debg       = false;
int debug_depth = 0;
uint8_t status  = 0;


// initialize libraries
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

// free variables
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

// free variables and exit program
void Exit() {
    if (status != S_FREE) {
        Free();
        exit(1);
    }
}

// called on keyboard interrupt
void interrupt(int sig) {
    error_exit("\nkeyboard interrupt");
}

// main
int main(int argc, char* argv[]) {
    BEGIN("argc=%i,argv", argc);
    PRINT_STATUS();

    // catch Keyboard interrupt
    signal(SIGINT, interrupt);

    if (argc < 2) error_exit("no input file");
    if (Init()) error_exit("Initialization failed");
    INFO("variables: " FMT_SIZE, collector.size());

    int res = 0;

    INFO("reading file");
    FILE* input = fopen(argv[1], "r");
    string code = readFile(input, true);
    INFO("file input:\n%s\n", code.c_str());

#if _INFO_
    uint alloc = collector.size();
#endif

    // create code scope of content from default or argument file path
    INFO("formatting code");
    status       = S_FORMAT;
    var_fnc main = toCode(code);
    INFO("allocated %u variables", alloc = collector.size() - alloc);
    INFO("\nglobal: %s\n", TOSTR(main.func));

    // create var_lst of program (sys)args
    var_lst vargs = {incRef(newStrC(argv[1]), 2)};
    for (int i = 2; i < argc; i++) vargs.push_back(incRef(newStrC(argv[i]), 2));

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

    // free all allocated variables
    status = S_FREE;
    Free();

    INFO("end.");
    END("%i", res);

    return res;
}
