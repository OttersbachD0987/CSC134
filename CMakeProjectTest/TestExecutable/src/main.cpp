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

    SDL_SetAppMetadata("Test Executable", "0.1.0.0", "com.darthsae.teratoma");
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
    
    /*
    TeraToma::GameClientAPI clientAPI;

    if (!SDL_CreateWindowAndRenderer(TeraToma::APPLICATION_TITLE, TeraToma::WINDOW_WIDTH, TeraToma::WINDOW_HEIGHT, SDL_WINDOW_VULKAN, &clientAPI.window, &clientAPI.renderer)) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    clientAPI.audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    
    std::println("Pre API");
    clientAPI.gameAPI = TeraToma::GameAPI();
    std::println("Pre Assets");
    clientAPI.assets = TeraToma::Assets::Assets();
    std::println("Pre UI Manager");
    clientAPI.uiManager = TeraToma::UI::UIManager();
    clientAPI.assets.uiManager = &clientAPI.uiManager;

    #pragma region Loading Mods and Assets
    std::println("Pre Mods");
    std::println("Mods {}", std::filesystem::current_path().append("Mods").string());
    clientAPI.gameAPI.LoadMods();

    std::println("Pre Images");
    for (std::filesystem::directory_entry const& dir_entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path().append("Assets"))) {
        if (dir_entry.path().has_extension()) {
            if (dir_entry.path().extension().string() == ".png") {
                clientAPI.assets.AddTexture(clientAPI.renderer, dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".ttf") {
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 12), 12);
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 16), 16);
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 18), 18);
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 20), 20);
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 24), 24);
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 30), 30);
                clientAPI.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 36), 36);
            } else if (dir_entry.path().extension().string() == ".wav") {
                clientAPI.assets.AddSound(clientAPI.audioDevice, dir_entry.path().string(), dir_entry.path().stem().string());
            }
        }
    }
    #pragma endregion
    */

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
    
    //SDL_CloseAudioDevice(clientAPI.audioDevice);
    //SDL_DestroyRenderer(clientAPI.renderer);
    //SDL_DestroyWindow(clientAPI.window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
