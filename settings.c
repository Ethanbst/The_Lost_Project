#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "screen_utils/screen_utils.h"
#include <string.h>
#include "logs_utils/log.h"
#include <stdio.h>
#include "cta_utils/cta.h"
#include "mouse_utils/mouse.h"
#include <C:/cJSON-master/cJSON.h>

#define slider_width 500
#define slider_height 20

// Retourne 1 si le fichier settings.txt existe, 0 sinon
int exist_settings2()
{
    FILE *file = fopen("settings.json", "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    else
    {
        return 0;
    }
}

// Fonction pour obtenir la valeur d'un paramètre dans le fichier settings.txt
int get_setting_value2(const char *setting_name)
{
    // Lire le fichier JSON
    const char *filename = "settings.json";
    FILE *file = fopen(filename, "r");
    if (!file) {
        add_log("SETTINGS", "Erreur: Impossible d'ouvrir le fichier settings.json\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        add_log("SETTINGS", "Erreur: Echec de l'allocation de memoire\n");
        fclose(file);
        return -1;
    }
    fread(data, 1, length, file);
    fclose(file);

    data[length] = '\0';

    // Parser le JSON
    cJSON *root = cJSON_Parse(data);
    if (root) {
        // Trouver le paramètre
        cJSON *parameter = cJSON_GetObjectItem(root, setting_name);
        if (parameter) {
            int value = parameter->valueint;
            cJSON_Delete(root);
            free(data);
            return value;
        } else {
            add_log("SETTINGS", "Erreur: Parametre non trouve dans le fichier JSON\n");
        }
        cJSON_Delete(root);
    } else {
        add_log("SETTINGS", "Erreur: Echec du parsing du fichier JSON\n");
    }

    free(data);
    return -1;
}

// Fonction pour définir la valeur d'un paramètre dans le fichier settings.txt
void set_setting_value2(const char *setting_name, int value){
    // Lire le fichier JSON
    const char *filename = "settings.json";
    FILE *file = fopen(filename, "r");
    if (!file) {
        add_log("SETTINGS", "Erreur: Impossible d'ouvrir le fichier settings.json\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        add_log("SETTINGS", "Erreur: Echec de l'allocation de memoire\n");
        fclose(file);
        return;
    }
    fread(data, 1, length, file);
    fclose(file);

    data[length] = '\0';

    // Parser le JSON
    cJSON *root = cJSON_Parse(data);
    if (root) {
        // Trouver et mettre à jour le paramètre
        cJSON *parameter = cJSON_GetObjectItem(root, setting_name);
        if (parameter) {
            add_log("SETTINGS", "Parametre trouve. Ancienne valeur mise a jour\n");
            cJSON_SetIntValue(parameter, value);
        } else {
            add_log("SETTINGS", "Erreur: Parametre non trouve dans le fichier JSON\n");
        }

        // Convertir le JSON en chaîne de caractères
        char *json_string = cJSON_Print(root);
        if (json_string) {
            // Écrire les modifications dans le fichier
            file = fopen(filename, "w");
            if (file) {
                fwrite(json_string, 1, strlen(json_string), file);
                fclose(file);
                add_log("SETTINGS", "Fichier mis a jour avec succes\n");
            } else {
                add_log("SETTINGS", "Erreur: Impossible d'ouvrir le fichier pour ecrire\n");
            }
            free(json_string);
        } else {
            add_log("SETTINGS", "Erreur: Echec de la conversion du JSON en chaine de caracteres\n");
        }

        // Libérer la mémoire
        cJSON_Delete(root);
    } else {
        add_log("SETTINGS", "Erreur: Echec du parsing du fichier JSON\n");
    }

    free(data);
}

void init_default_settings2()
{
    if (!exist_settings2())
    {
        // Objet JSON principal
        cJSON *root = cJSON_CreateObject();

        // On ajoute les paramètres par défaut au JSON
        cJSON_AddNumberToObject(root, "music_volume", 50);
        cJSON_AddNumberToObject(root, "fx_volume", 50);

        FILE *file = fopen("settings.json", "w");
        if (file)
        {
            fwrite(cJSON_Print(root), 1, strlen(cJSON_Print(root)), file);
            fclose(file);
        }

        // Libérer la mémoire
        cJSON_Delete(root);
    }
}

void options(SDL_Renderer *renderer)
{
    add_log("OPTIONS", "Entré dans les Options.\n");

    // Effacer l'écran et présenter un écran noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur noire
    SDL_RenderClear(renderer);

    // Variables pour le volume
    int musicVolume = get_setting_value2("music_volume"); // Volume initial de la musique (50%)
    int sfxVolume = get_setting_value2("fx_volume");      // Volume initial des effets sonores (50%)

    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 24);
    if (!font)
    {
        add_log_error("settings.c - options()", "Failed to load font.");
        return;
    }

    SDL_DisplayMode displayMode;
    displayMode = GetScreenSize();

    color couleur = {255, 255, 255, 255}; // Couleur blanche

    int mSlider_pos_x = displayMode.w / 2 + slider_width + slider_width / 2;
    int fxSlider_pos_x = mSlider_pos_x;

    // Dessiner les sliders

    CTA music_vol_txt = draw_button(renderer, displayMode.w / 2, displayMode.h/3, 2, "Volume de la musique", 0, 24, couleur);
    CTA music_slider = draw_slider(renderer,  mSlider_pos_x, displayMode.h/3, slider_width, slider_height, musicVolume, MIX_MAX_VOLUME);

    CTA fx_vol_txt = draw_button(renderer, displayMode.w / 2, music_vol_txt.pos_y+music_vol_txt.h*2, 2, "Volume des effets", 0, 24, couleur);
    CTA fx_slider = draw_slider(renderer, fxSlider_pos_x, music_vol_txt.pos_y+music_vol_txt.h*2, slider_width, slider_height, sfxVolume, MIX_MAX_VOLUME);

    // Dessiner le bouton "Appliquer"
    CTA apply_button = draw_button(renderer, displayMode.w/2, fx_slider.pos_y+fx_slider.pos_y/2, 2, "Appliquer", 0, 24, couleur);

    // Afficher les éléments
    SDL_RenderPresent(renderer);

    // Boucle pour gérer les événements des options
    SDL_Event event;
    int running = 1;
    int actual_cursor = SDL_SYSTEM_CURSOR_ARROW;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = 0;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    // Vérifier si le clic est sur le bouton "Appliquer"
                    if (is_mouse_on(apply_button))
                    {
                        Mix_VolumeMusic(musicVolume);
                        // Mix_Volume(-1, sfxVolume);
                        add_log("OPTIONS", "Modifications appliquées.\n");
                        add_log("OPTIONS", "Sortie des Options.\n");
                        running = 0;
                        SDL_RenderClear(renderer);
                        reset_cursor();
                        // On quitte les options
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                if (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) //Si mouvement souris ET clique
                {
                    // Vérifier si le curseur est sur le slider de la musique
                    if (is_mouse_on(music_slider))
                    {
                        musicVolume = (mouseX - music_slider.pos_x) * MIX_MAX_VOLUME / (slider_width*displayMode.w / 1920); //slider_width*displayMode.w / 1920 permet d'adapté la taille du slider en fonction de la résolution de l'écran
                        printf("musicVolume: %d\n", musicVolume);
                        if (musicVolume < 0) musicVolume = 0;
                        if (musicVolume > MIX_MAX_VOLUME) musicVolume = MIX_MAX_VOLUME;
                        music_slider = draw_slider(renderer,  mSlider_pos_x, displayMode.h/3, slider_width, slider_height, musicVolume, MIX_MAX_VOLUME);
                        set_setting_value2("music_volume", musicVolume);
                        SDL_RenderPresent(renderer);
                        Mix_VolumeMusic(musicVolume);
                    }

                    // Vérifier si le curseur est sur le slider des effets sonores
                    if (is_mouse_on(fx_slider))
                    {
                        sfxVolume = (mouseX - fx_slider.pos_x) * MIX_MAX_VOLUME / (slider_width*displayMode.w / 1920); //slider_width*displayMode.w / 1920 permet d'adapté la taille du slider en fonction de la résolution de l'écran
                        if (sfxVolume < 0) sfxVolume = 0;
                        if (sfxVolume > MIX_MAX_VOLUME) sfxVolume = MIX_MAX_VOLUME;
                        fx_slider = draw_slider(renderer, fxSlider_pos_x, music_vol_txt.pos_y+music_vol_txt.h*2, slider_width, slider_height, sfxVolume, MIX_MAX_VOLUME);
                        set_setting_value2("fx_volume", sfxVolume);
                        SDL_RenderPresent(renderer);
                        Mix_VolumeMusic(musicVolume);
                    }
                }
                // Vérifier si passage sur bouton appliquer
                if (is_mouse_on(apply_button))
                {
                    if (actual_cursor != SDL_SYSTEM_CURSOR_HAND)
                    {
                        actual_cursor = set_hand_cursor();
                    }
                }
                else if (actual_cursor != SDL_SYSTEM_CURSOR_ARROW)
                {
                    actual_cursor = reset_cursor();
                }
            }
        }
    }
    return;
}