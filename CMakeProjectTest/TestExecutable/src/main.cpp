#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <TestStaticLibrary/ModAPI.h>
#include <print>

int main(int charc, char **argv) {
    uint64_t setSeed = static_cast<uint64_t>(time(0));
    srand(setSeed);
    std::println("Seed: {}", setSeed);

    SDL_SetAppMetadata(TeraToma::APPLICATION_TITLE, "0.1.0.0", "com.darthsae.teratoma");
    // SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Darthsae");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/Darthsae/TeraToma");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    if (!TTF_Init()) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

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
