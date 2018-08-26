
// TODO:
//      toFunction(): keywords, control statements
//      all isOperator to prev.hasOperator
//      operator precedence
//      RaspiBot functions

unsigned int debug_func_intd = 0;

#include "include.hpp"
#include "interpret.hpp"

void interrupt(int sig) {
    // free all allocated variables
    INFO("interrupt catched - freeing");
    FreeVariables();

    printf("\n");
    exit(0);
}

// initialisation
int main(int argc, char* argv[]) {
    BEGIN("argc=%i,argv", argc);
    PRINT_STATUS();

    signal(SIGINT, interrupt);

    INFO("init operations");
    initOperations();

    INFO("init builtins");
    initBuiltins();

    INFO("reading file");
    const char* path = argc > 1 ? argv[1] : "res/code.bsc";
    string code      = readFile(path, true);
    INFO("file input:\n%s\n", code.c_str());

#if DEBUG == 1
    printf("press enter to continue\n");
    wait_enter();
#endif

    // create code scope of content from default or argument file path
    INFO("formatting code");
    var_lst main = toCode(code);

    INFO("\nmain: %s\n", TLst(main, T_FNC).toStr().c_str());

    // execute code
    INFO("executing code");
    handleScope(main);

    // free all allocated variables
    INFO("freeing...");
    FreeVariables();
    printf("\n");

    INFO("end.");
    END();

    return 0;
}
