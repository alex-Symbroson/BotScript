
// TODO:
//      toFunction(): operators. variables, ...
//      RaspiBot functions

unsigned int debug_func_intd = 0;

#include "include.hpp" //includes special functions
#include "interpret.hpp"

// initialisation
int main(int argc, char* argv[]) {
    BEGIN("int argc=%i,char**argv", argc);
    PRINT_STATUS();

    INFO("init operations");
    initOperations();

    INFO("init builtins");
    initBuiltins();

    INFO("reading file");
    const char* path = argc > 1 ? argv[1] : "res/code.bsc";
    string code      = readFile(path, true);
    INFO("file input:\n%s", code.c_str());

#if DEBUG == 1
    printf("press enter to continue\n");
    wait_enter();
#endif

    // create code scope of content from default or argument file path
    INFO("formatting code");
    var_lst main = toCode(code);

    INFO("\nmain: %s", TLst(main, T_FNC).toStr().c_str());

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
