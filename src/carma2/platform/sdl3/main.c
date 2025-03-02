#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdl3.h"
#include "sdl3_platform.h"

#include "errors.h"
#include "globvars.h"
#include "loading.h"
#include "main.h"

int main(int argc, char *argv[]) {
    int i;
    char *path;

    C2V(gAFE) = 0;
    SetDefaultPedFolderNames();
    for (i = 1; i < argc; i++) {
        if (SDL_strcmp(argv[i], "-NOCUTSCENE") == 0 || SDL_strcmp(argv[i], "-NOCUTSCENES") == 0) {
            C2V(gNoCutscenes) = 1;
        } else if (SDL_strcmp(argv[i], "-ZOMBIE") == 0) {
            ConfigureZombiePedTexturePath();
        } else if (SDL_strcmp(argv[i], "-BLOOD") == 0) {
            ConfigurePedBloodPaths();
        } else if (SDL_strcmp(argv[i], "-ALIEN") == 0) {
            ConfigurePedAlienPaths();
        } else if (SDL_strcmp(argv[i], "-AFE") == 0) {
            C2V(gAFE) = 1;
//        } else if (SDL_strcmp(argv[i], "-SCALEMOUSE") == 0) {
//            gScaleMouse = 1;
        } else {
            dr_dprintf("Unknown argument: \"%s\"", argv[i]);
            return 1;
        }
    }
    path = SDL_GetCurrentDirectory();
    if (path == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetCurrentDirectory() failed (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_GetCurrentDirectory failed", SDL_GetError(), NULL);
        SDL_Quit();
        return 1;
    }
    SDL_strlcpy(gPathNetworkIni, path, SDL_arraysize(gPathNetworkIni));

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init failed", SDL_GetError(), NULL);
        return 1;
    }

    g_PerformanceFrequency = SDL_GetPerformanceFrequency();

    g_SDL_Window = SDL_CreateWindow("rec2", 640, 480, SDL_WINDOW_OPENGL);
    if (g_SDL_Window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed (%s)", SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow failed", SDL_GetError(), NULL);
        SDL_Quit();
        return 1;
    }
    gHWnd = g_SDL_Window;

    const char* args[] = { "rec2", NULL };

    GameMain(1, args);
    C2_HOOK_FINISH();

    SDL_Quit();
    return 0;
}
