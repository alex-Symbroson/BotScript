
// TODO:
//      handle while(); or repeat(); statements (without do)
//      toFunction(): keywords, control statements
//      all isOperator to prev.hasOperator
//      operator precedence
//      RaspiBot functions

unsigned int debug_func_intd = 0;

#include "include.hpp"
#include "interpret.hpp"

#if ISBOT
#    include "RaspiBot.hpp"
#endif

bool Init() {
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
    INFO("freeing...");
#if ISBOT
    RaspiBot::Free();
#endif
    FreeVariables();
    printf("\n");
}

void interrupt(int sig) {
    // free all allocated variables
    error("\nkeyboard interrupt");
    Exit();
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

    INFO("reading file");
    const char* path = argc > 1 ? argv[1] : "res/code.bsc";
    string code      = readFile(path, true);
    INFO("file input:\n%s\n", code.c_str());

    // create code scope of content from default or argument file path
    INFO("formatting code");
    var_lst main = toCode(code);

    INFO("\nmain: %s\n", TLst(main, T_FNC).toStr().c_str());

    // execute code
    INFO("executing code");
    handleScope(main);

    // free all allocated variables
    Free();

    INFO("end.");
    END();

    return 0;
}
