#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

//void add_log(const char *tag, const char *message);

void draw_slider(SDL_Renderer *renderer, int x, int y, int w, int h, int value, int max_value) {
    SDL_Rect slider_bg = {x, y, w, h};
    SDL_Rect slider_fg = {x, y, (w * value) / max_value, h};

    // Draw background
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Background color (gray)
    SDL_RenderFillRect(renderer, &slider_bg);

    // Draw foreground
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Foreground color (light red)
    SDL_RenderFillRect(renderer, &slider_fg);

    // Draw border
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Border color (black)
    SDL_RenderDrawRect(renderer, &slider_bg);
}



void draw_button(SDL_Renderer *renderer, int x, int y, int w, int h, const char *text, TTF_Font *font) {
    SDL_Rect button_rect = {x, y, w, h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur du bouton
    
    SDL_RenderFillRect(renderer, &button_rect);

    SDL_Color textColor = {255, 255, 255, 255}; // Text color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect text_rect = {x + (w - text_width) / 2, y + (h - text_height) / 2, text_width, text_height};

    SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void options(SDL_Renderer *renderer, SDL_Window *window) {
    add_log("OPTIONS", "Entré dans les Options.\n");

    // Variables pour le volume
    int musicVolume = MIX_MAX_VOLUME / 2; // Volume initial de la musique (50%)
    int sfxVolume = MIX_MAX_VOLUME / 2;   // Volume initial des effets sonores (50%)

    // Initialisation de TTF
    if (TTF_Init() == -1) {
        add_log("ERROR", "Failed to initialize TTF.\n");
        return;
    }
    TTF_Font *font = TTF_OpenFont("res/font/Pixelfy.ttf", 24);
    if (!font) {
        add_log("ERROR", "Failed to load font.\n");
        return;
    }

    // Rendu des options
    SDL_RenderClear(renderer);

    // Dessiner les sliders
    draw_slider(renderer, 100, 100, 200, 20, musicVolume, MIX_MAX_VOLUME);
    draw_slider(renderer, 100, 200, 200, 20, sfxVolume, MIX_MAX_VOLUME);

    // Dessiner le bouton "Appliquer"
    draw_button(renderer, 350, 400, 100, 50, "Appliquer", font);

    SDL_RenderPresent(renderer);

    // Boucle pour gérer les événements des options
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = 0;
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;

                        // Vérifier si le clic est sur le bouton "Appliquer"
                        if (mouseX >= 350 && mouseX <= 450 && mouseY >= 400 && mouseY <= 450) {
                            Mix_VolumeMusic(musicVolume);
                            //Mix_Volume(-1, sfxVolume);
                            add_log("OPTIONS", "Modifications appliquées.\n");
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                        int mouseX = event.motion.x;
                        int mouseY = event.motion.y;

                        // Vérifier si le curseur est sur le slider de la musique
                        if (mouseX >= 100 && mouseX <= 300 && mouseY >= 100 && mouseY <= 120) {
                            musicVolume = (mouseX - 100) * MIX_MAX_VOLUME / 200;
                            draw_slider(renderer, 100, 100, 200, 20, musicVolume, MIX_MAX_VOLUME);
                            SDL_RenderPresent(renderer);
                            Mix_VolumeMusic(musicVolume);
                        }

                        // Vérifier si le curseur est sur le slider des effets sonores
                        if (mouseX >= 100 && mouseX <= 300 && mouseY >= 200 && mouseY <= 220) {
                            sfxVolume = (mouseX - 100) * MIX_MAX_VOLUME / 200;
                            draw_slider(renderer, 100, 200, 200, 20, sfxVolume, MIX_MAX_VOLUME);
                            SDL_RenderPresent(renderer);
                            Mix_VolumeMusic(musicVolume);
                        }
                    }
                    break;
            }
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();

    add_log("OPTIONS", "Sortie des Options.\n");
}