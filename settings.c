#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "GetScreenSize.c"
#include <string.h>
#include "logs_utils/log.h"
#include <stdio.h>

#define slider_width 600
#define slider_height 40

typedef struct _CTA{
    int pos_x; //Position en x du CTA
    int pox_y; //Position en y du CTA
    int w; //Longueur du CTA
    int h; //Largeur du CTA
} CTA;


//Retourne 1 si le fichier settings.txt existe, 0 sinon
int exist_settings() {
    FILE *file = fopen("settings.txt", "r");
    if (file) {
        fclose(file);
        return 1;
    } 
    else 
    {
        return 0;
    } 
}

// Fonction pour obtenir la valeur d'un paramètre dans le fichier settings.txt
int get_setting_value(const char *setting_name) {
    FILE *file = fopen("settings.txt", "r");
    if (file == NULL) {
        add_log("SETTINGS", "Erreur lors de l'ouverture du fichier settings.txt\n");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char name[128];
        int value;
        if (sscanf(line, "%127s %d", name, &value) == 2) {
            if (strcmp(name, setting_name) == 0) {
                fclose(file);
                return value;
            }
        }
    }

    fclose(file);
    return -1; // Retourne -1 si le paramètre n'est pas trouvé
}


// Fonction pour définir la valeur d'un paramètre dans le fichier settings.txt
void set_setting_value(const char *setting_name, int value) {
    FILE *file = fopen("settings.txt", "r");
    if (file == NULL) {
        add_log("SETTINGS", "Erreur lors de l'ouverture du fichier settings.txt\n");
        return;
    }

    FILE *temp = fopen("settings_tmp.txt", "w");
    if (temp == NULL) {
        add_log("SETTINGS", "Erreur lors de la création du fichier temporaire\n");
        fclose(file);
        return;
    }

    char line[256];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        char name[128];
        int current_value;
        
        if (sscanf(line, "%127s %d", name, &current_value) == 2) {
            if (strcmp(name, setting_name) == 0) {
                fprintf(temp, "%s %d\n", setting_name, value);
                found = 1;
            } else {
                fprintf(temp, "%s %d\n", name, current_value);
            }
        } else {
            fputs(line, temp); // Pour conserver les lignes invalides si nécessaire
        }
    }

    if (!found) {
        fprintf(temp, "%s %d\n", setting_name, value);
    }

    fclose(file);
    fclose(temp);

    // Remplace l'ancien fichier par le nouveau
    remove("settings.txt");
    rename("settings_tmp.txt", "settings.txt");
}


//Fonction pour initialiser les paramètres par défaut si le fichier n'existe pas déjà
void init_default_settings() {
    //Si le fichier existe, on ne fait rien
    if(exist_settings()){
        return;
    }
    else{
        FILE *file = fopen("settings.txt", "w");
        if (file == NULL) {
            add_log("SETTINGS", "Erreur lors de l'ouverture du fichier settings.txt\n");
            return;
        }
        fprintf(file, "music_volume 50\n");
        fprintf(file, "fx_volume 50\n");
        fclose(file);
        add_log("SETTINGS", "Fichier settings.txt écrit avec les valeurs par défaut\n");
    }
}


//void add_log(const char *tag, const char *message);

CTA draw_slider(SDL_Renderer *renderer, int x, int y, int w, int h, int value, int max_value) {

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

    CTA slider;
    slider.h = h;
    slider.w = w;
    slider.pos_x = x;
    slider.pox_y = y;

    return slider;
}



void draw_button(SDL_Renderer *renderer, int x, int y, int multiply_size, const char *text, TTF_Font *font) {
    SDL_Color textColor = {255, 255, 255, 255}; // Text color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect text_rect = {x, y, multiply_size*text_width, multiply_size*text_height}; //Taille et pos du texte
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Couleur du bouton
    SDL_RenderFillRect(renderer, &text_rect);

    SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}


void options(SDL_Renderer *renderer, SDL_Window *window) {
    add_log("OPTIONS", "Entré dans les Options.\n");

    // Effacer l'écran et présenter un écran noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur noire
    SDL_RenderClear(renderer);

    // Variables pour le volume
    int musicVolume = get_setting_value("music_volume"); // Volume initial de la musique (50%)
    int sfxVolume = get_setting_value("fx_volume");   // Volume initial des effets sonores (50%)

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

    SDL_DisplayMode displayMode;
    displayMode = GetScreenSize();


    // Dessiner les sliders
    CTA music_slider = draw_slider(renderer, displayMode.w/2-slider_width/2, 100, slider_width, slider_height, musicVolume, MIX_MAX_VOLUME);
    CTA fx_slider = draw_slider(renderer, displayMode.w/2-slider_width/2, 200, slider_width, slider_height, sfxVolume, MIX_MAX_VOLUME);

    // Dessiner le bouton "Appliquer"
    draw_button(renderer, displayMode.w/2-slider_width/2, 300, 2, "Appliquer", font);

    //Afficher les éléments
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
                        if (mouseX >= displayMode.w/2-slider_width/2 && mouseX <= displayMode.w/2+slider_width/2 && mouseY >= 300 && mouseY <= 350) {
                            Mix_VolumeMusic(musicVolume);
                            //Mix_Volume(-1, sfxVolume);
                            add_log("OPTIONS", "Modifications appliquées.\n");
                            add_log("OPTIONS", "Sortie des Options.\n");
                            running = 0;
                            SDL_RenderClear(renderer);
                            //On quitte les options
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                        int mouseX = event.motion.x;
                        int mouseY = event.motion.y;

                        // Vérifier si le curseur est sur le slider de la musique
                        if (mouseX >= displayMode.w/2-music_slider.w/2 && mouseX <= displayMode.w/2+music_slider.w/2 && mouseY >= 100 && mouseY <= 100+music_slider.h) {
                            musicVolume = (mouseX - (displayMode.w/2-slider_width/2)) * MIX_MAX_VOLUME / slider_width;
                            music_slider = draw_slider(renderer, music_slider.pos_x, music_slider.pox_y, slider_width, slider_height, musicVolume, MIX_MAX_VOLUME);
                            set_setting_value("music_volume", musicVolume);
                            SDL_RenderPresent(renderer);
                            Mix_VolumeMusic(musicVolume);
                        }


                        // Vérifier si le curseur est sur le slider des effets sonores
                        if (mouseX >= displayMode.w/2-fx_slider.w/2 && mouseX <= displayMode.w/2+fx_slider.w/2 && mouseY >= 200 && mouseY <= 200+fx_slider.h) {
                            sfxVolume = (mouseX - (displayMode.w/2-slider_width/2)) * MIX_MAX_VOLUME / slider_width;
                            fx_slider = draw_slider(renderer, displayMode.w/2-slider_width/2, 200, slider_width, slider_height, sfxVolume, MIX_MAX_VOLUME);
                            set_setting_value("fx_volume", sfxVolume);
                            SDL_RenderPresent(renderer);
                            Mix_VolumeMusic(musicVolume);
                        }
                    }
                    break;
            }
        }
    }
    return;
}