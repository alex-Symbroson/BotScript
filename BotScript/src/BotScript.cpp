
/* TODO:
    toFunction():
        control statements: for

    allow `while condition do function;` with variables
        add term type (dont eval terms on assignment)

    function args:
        declaration, type assertion

    Variables:
        IDis... not, >=
        decl var without initialisation

!   RaspiBot functions:
        drive, rotate, curve, accumulateTo, accumulateBy,
        getIRSensors, getButtonStates,
        setButtonColors
!    Python part AttinyProtocol

!   T_PIN operators & WiringPi

    copy const on modify
    operator precedence
    check stod2 //?
    type casts, int<>float automatic?
    // all isOperator to prev.hasOperator
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
    status    = S_FREE;
    int alloc = collector.size();

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
    var_fnc main = toCode(code);
    INFO("allocated %u variables", alloc = collector.size() - alloc);

    INFO("\nmain: %s\n", TOSTR(main.func));

    // create var_lst of program (sys)args
    var_lst vargs = {incRef(newStrC(path), 2)};
    for (int i = 2; i < argc; i++) vargs.push_back(incRef(newStrC(argv[i]), 2));

    // execute code
    INFO("execute global");
    status     = S_EXEC;
    funcResult = incRef(newNil());
    handleFunc(newFnc(main), newArgC(vargs));

    INFO("execute init");
    status    = S_EXEC;
    PVar func = findVar("init", &main);
    if (func && getType(func) == T_FNC) handleFunc(func, newArgC(vargs));

    INFO("execute main");
    status = S_EXEC;
    func   = findVar("main", &main);
    if (func && getType(func) == T_FNC) handleFunc(func, newArgC({}));

    INFO("execute loop");
    status = S_EXEC;
    func   = findVar("loop", &main);
    if (func && getType(func) == T_FNC)
        while (status < S_STOP) handleFunc(func, newArgC({}));


    INFO("stop program");
    int res = 0;
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
    INFO("freeing code (%u)", alloc);
    status = S_FREE;

#if _DEBUG_
    delloc = collector.size();
    for (PVar& v: main.func) delete v;
    delloc -= collector.size();
#else
    for (PVar& v: main.func) delete v;
#endif

    INFO("freed %u of %u code vars", delloc, alloc);
    Free();

    INFO("end.");
    END("%i", res);

    return res;
}
