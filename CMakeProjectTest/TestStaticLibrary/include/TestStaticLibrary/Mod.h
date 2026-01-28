#pragma once
#ifndef TEST_STATIC_LIBRARY__MOD_H
#define TEST_STATIC_LIBRARY__MOD_H
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>

std::string StringFromWString(std::wstring a_wstring);

namespace TestStaticLibrary {
    class Mod;
}

#include <TestStaticLibrary/ModAPI.h>

namespace TestStaticLibrary {
    typedef void (ModCallPrefix *ModLoadFnptr)(ModAPI* const);
    typedef void (ModCallPrefix *ModInitializeFnptr)(ModAPI* const);
    typedef void (ModCallPrefix *ModUninitializeFnptr)(ModAPI* const);
    typedef void (ModCallPrefix *ModUnloadFnptr)(ModAPI* const);

    class Mod {
    public:
        Mod(std::string a_modName, std::wstring a_path);

        std::string name;
        ModLibraryHandle library;
        bool enabled = true;

        ModLoadFnptr modLoadFnptr = nullptr;
        ModInitializeFnptr modInitializeFnptr = nullptr;
        ModUninitializeFnptr modUninitializeFnptr = nullptr;
        ModUnloadFnptr modUnloadFnptr = nullptr;

        void Load(ModAPI* const a_modAPI);
        void Initialize(ModAPI* const a_modAPI);
        void Uninitialize(ModAPI* const a_modAPI);
        void Unload(ModAPI* const a_modAPI);

        ~Mod(void);
    };
}
#endif