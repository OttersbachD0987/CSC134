#pragma once
#ifndef TEST_STATIC_LIBRARY__MOD_SYSTEM_H
#define TEST_STATIC_LIBRARY__MOD_SYSTEM_H
namespace TestStaticLibrary {
    typedef void* (*AnyFnptr)(...);
    struct ModSystem {
    public:
        ModSystem(AnyFnptr a_function);    

        AnyFnptr function = nullptr;
    };
}
#endif