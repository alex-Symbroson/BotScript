
//$ g++ -std=c++11 -O3 BotScript.cpp -lwiringPi -pthread  include.o variables.o
// pins.o threads.o builtins.o

// TODO:
//      toFunction()
//      handleScope()
//      callBuiltin()

#include "include.hpp" //includes special functions
#include "interpret.hpp"

using namespace std;

// list of defined variables
// var_lst variables;

// initialisation
int main(int argc, char* argv[]) {
    BEGIN("int argc=%i,char**argv", argc);
    PRINT_STATUS();

    const char* path = argc > 1 ? argv[1] : "res/code.bsc";
    string code      = readFile(path, true);
    INFO("file input:\n%s", code.c_str());
    if (code == "") error("file \"%s\" is empty", path);

#if DEBUG == 1
    printf("press enter to continue\n");
    wait_enter();
#endif

    // create code scope of content from default or argument file path
    INFO("formatting code");
    var_lst main = toCode(&code);

    printf("main: %s\n", TLst(main).toStr().c_str());

    // execute code
    INFO("executing code");
    PVar res = handleScope(main);
    printf("\n");
    DEBUG("returned: %s\n", res->toStr().c_str());

    // free all allocated variables
    INFO("freeing...");
    FreeVariables();
    printf("\n");

    INFO("end.");
    END();
    return 0;
}
