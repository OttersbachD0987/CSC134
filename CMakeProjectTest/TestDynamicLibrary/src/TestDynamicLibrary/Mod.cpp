#include <TestStaticLibrary/ModAPI.h>
#include <print>

extern "C" void MOD_API ModLoad(TestStaticLibrary::ModAPI* const a_modAPI) {
    std::println("Mod loaded.");
}

extern "C" void MOD_API ModInitialize(TestStaticLibrary::ModAPI* const a_modAPI) {
    std::println("Mod initialize.");
}

extern "C" void MOD_API ModUninitialize(TestStaticLibrary::ModAPI* const a_modAPI) {
    std::println("Mod uninitialize.");
}

extern "C" void MOD_API ModUnload(TestStaticLibrary::ModAPI* const a_modAPI) {
    std::println("Mod unloaded.");
}
