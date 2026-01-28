#include <TestStaticLibrary/ModSystem.h>

namespace TestStaticLibrary {
    ModSystem::ModSystem(AnyFnptr a_function) {
        this->function = a_function;
    }
}