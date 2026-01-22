#include <TestStaticLibrary/ModLibrary.h>

extern "C" void MOD_API ModLoad() {}

extern "C" void MOD_API ModInitialize() {}

extern "C" void MOD_API ModUninitialize() {}

extern "C" void MOD_API ModUnload() {}
