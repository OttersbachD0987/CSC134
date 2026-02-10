#include <TestStaticLibrary/ModAPI.h>


namespace TestStaticLibrary {
    ModAPI::ModAPI(void) {
        this->mods = std::unordered_map<std::string, Mod>();
        this->modsDirectory = std::filesystem::current_path().append("Mods");
    }

    void ModAPI::LoadMods(void) {
        for (std::filesystem::directory_entry const& dirEntry : std::filesystem::directory_iterator(modsDirectory)) {
            if (dirEntry.is_regular_file() && dirEntry.path().extension().string() == MOD_FILE_EXTENSION) {
                //std::println("{} {} {}", dirEntry.path().stem().string(), dirEntry.path().extension().string(), dirEntry.path().string());
                mods.emplace(std::piecewise_construct, std::forward_as_tuple(dirEntry.path().stem().string()), std::forward_as_tuple(dirEntry.path().stem().string(), dirEntry.path().wstring()));
            }
        }
    }
    
    void ModAPI::DoModLoading(void) {
        for (std::pair<const std::string, Mod>& pair : mods) {
            pair.second.Load(this);
        }
    }

    void ModAPI::DoModInitialization(void) {
        for (std::pair<const std::string, Mod>& pair : mods) {
            pair.second.Initialize(this);
        }
    }

    void ModAPI::DoModUninitialization(void) {
        for (std::pair<const std::string, Mod>& pair : mods) {
            pair.second.Uninitialize(this);
        }
    }

    void ModAPI::DoModUnloading(void) {
        for (std::pair<const std::string, Mod>& pair : mods) {
            pair.second.Unload(this);
        }
    }

    
    void ModAPI::RegisterSystem(std::string_view a_id, AnyFnptr a_function) {
        systems.emplace(a_id, a_function);
    }

    void ModAPI::Destroy(void) {
        mods.clear();
        systems.clear();
    }
}