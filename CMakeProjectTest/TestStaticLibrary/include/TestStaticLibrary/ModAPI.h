#pragma once
#ifndef TEST_STATIC_LIBRARY__MOD_API_H
#define TEST_STATIC_LIBRARY__MOD_API_H
#include <string>
#include <string_view>
#include <unordered_map>
#include <filesystem>

namespace TestStaticLibrary {
    class ModAPI;
}

#include <TestStaticLibrary/ModLibrary.h>
#include <TestStaticLibrary/Mod.h>
#include <TestStaticLibrary/ModSystem.h>

namespace TestStaticLibrary {
    class ModAPI {
    public:
        ModAPI(void);

        std::unordered_map<std::string, Mod> mods;
        std::filesystem::path modsDirectory;

        std::unordered_map<std::string, ModSystem> systems;

        void LoadMods(void);
        void DoModLoading(void);
        void DoModInitialization(void);
        void DoModUninitialization(void);
        void DoModUnloading(void);

        void RegisterSystem(std::string_view a_id, AnyFnptr a_function);
        
        void Destroy(void);
    };
}
#endif
