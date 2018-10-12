
// TODO:
//      handle while(); or repeat(); statements (without do)
//      toFunction(): keywords, control statements
//      all isOperator to prev.hasOperator
//      operator precedence
//      RaspiBot functions

#include "include.hpp"
#include "interpret.hpp"
#if ISBOT
#    include "RaspiBot.hpp"
#endif


uint debug_depth = 0;
bool debg        = false;
uint8_t status   = 0;


bool Init() {
    status = S_INIT;
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
#if _DEBUG_
    uint alloc = collector.size();
#endif

#if ISBOT
    INFO("freeing RaspiBot");
    RaspiBot::Free();
    INFO("freed %lu vars", alloc - collector.size());
#endif

    INFO("freeing collector (%u)", alloc);
    FreeVariables();
    INFO("freed %lu of %u vars", alloc - collector.size(), alloc);
    END();
}

void interrupt(int sig) {
    // free all allocated variables
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
    INFO("variables: %lu", collector.size());

    INFO("reading file");
    const char* path = argc > 1 ? argv[1] : "res/code.bsc";
    string code      = readFile(path, true);
    INFO("file input:\n%s\n", code.c_str());

#if _DEBUG_
    uint alloc = collector.size(), delloc;
#endif

    // create code scope of content from default or argument file path
    INFO("formatting code");
    status       = S_FORMAT;
    var_lst main = toCode(code);
    INFO("allocated %u variables", alloc = collector.size() - alloc);

    INFO("\nmain: %s\n", toStr(main, T_FNC).c_str());

    // execute code
    INFO("executing code");
    status = S_EXEC;
    handleScope(main);

    // free all allocated variables
    INFO("freeing code (%u)", alloc);
    status = S_FREE;
#if _DEBUG_
    delloc = collector.size();
#endif
    for (PVar& v: main) delete v;
#if _DEBUG_
    delloc -= collector.size();
#endif
    INFO("freed %u of %u code vars", delloc, alloc);

    Free();

    INFO("end.");
    END();

    return 0;
}
