
#include <main.hpp>

int main() {

    // test TInt as Pin
    TInt v1(8, T_PIN);
    printf("v: %i = %ld\n", v1.type, v1.value);

    // test TFlt
    TFlt v2(4.5);
    printf("v: %i = %lf\n", v2.type, v2.value);

    // test TStr
    TStr v3("hello");
    printf("v: %i = %s\n", v3.type, v3.value.c_str());

    // test TLst
    TLst v6({v1.getVar(), v2.getVar(), v3.getVar()});
    printf("%s\n", v6.toStr().c_str());

    // test TObj
    TObj v7({{"v1", v1.getVar()}, {"v2", v2.getVar()}, {"v3", v3.getVar()}});
    printf("%s\n", v7.toStr().c_str());

    // test getVar
    PVar s = v3.getVar();

    // test direct Pvar creation
    //! attention: TStr(" World").getVar() causes instant destruction
    PVar pv = (new TStr(" World"))->getVar();

    // test subfunction "add" for strings
    printf("test add\n");
    callP(s, "add", s, pv);
    printf("v: %i = %s\n", getType(s), getStr(s).c_str());
    delete pv;

    // test velocity of addition
    PVar i = v1.getVar();
    PVar a = (new TInt(1))->getVar();

    // measure
    uint32_t start = clock();
    for (uint64_t n = 0; n < 10000000; n++) {
        callP(i, "add", i, a);
    }
    uint32_t end = clock();

    // result value
    printf("i = %ld\n", getInt(i));

    // result time
    uint32_t d = end - start;
    printf("time: %i = %ius ", d, d % 1000);
    printf("%ims ", (d /= 1000) % 1000);
    printf("%is\n", (d /= 1000) % 1000);

    // free subfunctin maps
    FreeVariables();
    printf("press enter to exit\n");
    getchar();
}
