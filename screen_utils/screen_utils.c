#include <SDL.h>

SDL_DisplayMode GetScreenSize()
{
    SDL_DisplayMode displayMode;

    // Obtenir la résolution de l'écran
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        // add_log("SDL_UTILS", "Erreur lors de la récupération de la résolution de l'écran");
    }
    return displayMode;
};

