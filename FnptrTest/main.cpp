#include <cstdint>
#include <iostream>

typedef void* (*AnyFnptr)(...);
typedef int32_t (*TestFunctionFnptr)(int32_t);
int32_t TestFunction(int32_t a_var) {
    return a_var * 2;
}

bool ThingFunction(void) {
    return true;
}

int main(int argc, char** argv) {
    AnyFnptr boo = (AnyFnptr)TestFunction;

    std::cout << reinterpret_cast<int32_t>(boo(2, 5)) << std::endl;
    return 0;
}