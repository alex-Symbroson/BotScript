
#include <main.hpp>

int main() {

    TInt v1(8);
    TFlt v2(4.5);
    TStr v3("hello");
    PVar pv = TStr(" World").getVar();

    printf("1\n");
    PVar s = v3.getVar();
    printf("2\n");
    callP(s, "add", s, pv);
    printf("3\n");

    printf("v: %i = %ld\n", v1.type, v1.value);
    printf("v: %i = %lf\n", v2.type, v2.value);
    printf("v: %i = %s\n", v3.type, v3.value.c_str());

    //
    // PVar i         = v.getVar();
    // uint32_t start = clock();
    //
    // for (uint64_t n = 0; n < 10000000; n++) {
    //     call(i, "add", i, TInt(1).getVar());
    // }
    //
    // uint32_t end = clock();
    // printf("i: %i = %ld\n", getType(i), getInt(i));
    // printf(
    //     "time: %ius %ims %is\n", (end - start), (end - start) / 1000,
    //     (end - start) / 1000000);
    //
    // PVar str  = TStr("Hello").getVar();
    // PVar str2 = TStr(" World!").getVar();
    // TStr str3 = TStr("hi");
    // getStr(str) += getStr(str2);
    // // call(str, "add", str, str /**/);
    // printf("%s\n", getStr(str).c_str());

    FreeVariables();
    printf("press enter to exit\n");
    getchar();
}
