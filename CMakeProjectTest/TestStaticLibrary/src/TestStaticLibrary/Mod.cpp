#include <TestStaticLibrary/Mod.h>

std::string StringFromWString(std::wstring a_wstring) {
    size_t len = wcstombs(nullptr, a_wstring.c_str(), 0) + 1;
    char* buffer = new char[len];
    wcstombs(buffer, a_wstring.c_str(), len);
    std::string str(buffer);
    delete[] buffer;
    return str;
}

namespace TestStaticLibrary {
    Mod::Mod(std::string a_modName, std::wstring a_path) {
        this->name = a_modName;
        this->library = ModLoadLibrary(a_path);
        this->modLoadFnptr = (ModLoadFnptr)ModLoadFunction(library, "ModLoad");
        this->modInitializeFnptr = (ModInitializeFnptr)ModLoadFunction(library, "ModInitialize");
        this->modUninitializeFnptr = (ModUninitializeFnptr)ModLoadFunction(library, "ModUninitialize");
        this->modUnloadFnptr = (ModUnloadFnptr)ModLoadFunction(library, "ModUnload");
    }

    void Mod::Load(ModAPI* const a_modAPI) {
        if (modLoadFnptr != NULL) {
            modLoadFnptr(a_modAPI);
        }
    }

    void Mod::Initialize(ModAPI* const a_modAPI) {
        if (modInitializeFnptr != NULL) {
            modInitializeFnptr(a_modAPI);
        }
    }

    void Mod::Uninitialize(ModAPI* const a_modAPI) {
        if (modUninitializeFnptr != NULL) {
            modUninitializeFnptr(a_modAPI);
        }   
    }

    void Mod::Unload(ModAPI* const a_modAPI) {
        if (modUnloadFnptr != NULL) {
            modUnloadFnptr(a_modAPI);
        }
    }

    Mod::~Mod(void) {
        ModCloseLibrary(library);
    }
}