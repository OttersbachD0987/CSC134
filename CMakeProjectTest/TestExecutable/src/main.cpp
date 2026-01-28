#include <iostream>
#include <TestStaticLibrary/ModAPI.h>
#include <print>

int main(int charc, char **argv) {
    uint64_t setSeed = static_cast<uint64_t>(time(0));
    srand(setSeed);
    std::println("Seed: {}", setSeed);

    TestStaticLibrary::ModAPI modAPI = TestStaticLibrary::ModAPI();

    std::println("Pre Mods");
    modAPI.LoadMods();

    std::println("Pre Mod Load");
    modAPI.DoModLoading();

    std::println("Pre Mod Initialize");
    modAPI.DoModInitialization();

    std::cout << "All thyne armies. Or whatever.\n";

    std::println("Pre Mod Uninitialize");
    modAPI.DoModUninitialization();

    std::println("Pre Mod Unload");
    modAPI.DoModUnloading();

    modAPI.Destroy();
    return 0;
}
