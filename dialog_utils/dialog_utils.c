#include "../logs_utils/log.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <C:/cJSON-master/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Fonction qui affiche un dialogue progressivement et attend un clic pour passer à la phrase suivante
void display_dialog(SDL_Renderer *renderer, char *text) {
    add_log_info("dialog_utils.c - display_dialog()", "Affichage du dialogue");

    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 48);
    if (!font) {
        add_log_error("dialog_utils.c - display_dialog()", "Erreur lors du chargement de la police");
        return;
    }

    SDL_Color color = {255, 255, 255, 255};
    SDL_Event event;
    int text_length = strlen(text);
    char *displayed_text = (char *)malloc(text_length + 1);
    int i = 0;
    int skip = 0;

    // Get window size
    int window_width, window_height;
    SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

    int max_text_width = (2 * window_width) / 5;   // 2/5 of the window width
    int max_text_height = window_height / 10;

    while (i < text_length) {
        displayed_text[i] = text[i];
        displayed_text[i + 1] = '\0';

        SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, displayed_text, color, max_text_width-20); // -20 for margin
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect text_rect;
        text_rect.w = surface->w;
        text_rect.h = surface->h;
        text_rect.x = (window_width - max_text_width) / 2 + 20; // Margin of 20 pixels from left
        text_rect.y = window_height - max_text_height - 50 + 10; // Margin of 10 pixels from top

        SDL_Rect background_rect;
        background_rect.w = max_text_width;
        background_rect.h = max_text_height;
        background_rect.x = (window_width - background_rect.w) / 2;
        background_rect.y = window_height - background_rect.h - 50;

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        //SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Semi-transparent black
        SDL_RenderFillRect(renderer, &background_rect);

        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        if (skip) {
            SDL_Delay(0);
        } 
        else {
            if (text[i] == '.') {
                SDL_Delay(100); // Pause when a period is displayed
            } else {
                SDL_Delay(50); // Delay between each letter
            }
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                skip = 1;
            }
        }

        i++;
    }

    free(displayed_text);

    // Wait for a click to proceed
    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            break;
        }
    }

    TTF_CloseFont(font);
}

char *load_dialogs_from_json(int id) {
    setlocale(LC_ALL, "en_US.UTF-8"); // Assurez-vous que l'encodage est UTF-8

    char *file_path = (char *)malloc(100);
    snprintf(file_path, 100, "res/dialogs/%d.json", id);
    
    FILE *file = fopen(file_path, "r");
    if (!file) {
        add_log_error("dialog_utils.c - load_dialogs_from_json()", "Erreur lors de l'ouverture du fichier JSON");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';
    fclose(file);

    cJSON *parsed_json = cJSON_Parse(file_content);
    if (!parsed_json) {
        add_log_error("dialog_utils.c - load_dialogs_from_json()", "Erreur lors de l'analyse du fichier JSON");
        free(file_content);
        return NULL;
    }

    cJSON *dialogues = cJSON_GetObjectItem(parsed_json, "dialogues");
    if (!cJSON_IsArray(dialogues)) {
        add_log_error("dialog_utils.c - load_dialogs_from_json()", "Le fichier JSON ne contient pas de tableau 'dialogues'");
        cJSON_Delete(parsed_json);
        free(file_content);
        return NULL;
    }

    int n_dialogues = cJSON_GetArraySize(dialogues);
    int total_length = 0;
    for (int i = 0; i < n_dialogues; i++) {
        cJSON *dialogue = cJSON_GetArrayItem(dialogues, i);
        if (cJSON_IsString(dialogue)) {
            total_length += strlen(dialogue->valuestring) + 1; // +1 for '\0'
        }
    }

    char *all_dialogues = (char *)malloc(total_length + 1);
    all_dialogues[0] = '\0';
    for (int i = 0; i < n_dialogues; i++) {
        cJSON *dialogue = cJSON_GetArrayItem(dialogues, i);
        if (cJSON_IsString(dialogue)) {
            strcat(all_dialogues, dialogue->valuestring);
            strcat(all_dialogues, "\n"); // Separate dialogues with newline
        }
    }

    cJSON_Delete(parsed_json);
    free(file_content);

    return all_dialogues;
}

void display_dialogs(SDL_Renderer *renderer, int id) {
    char *texts = load_dialogs_from_json(id);
    char *line = strtok(texts, "\n");
    while (line != NULL) {
        display_dialog(renderer, line);
        line = strtok(NULL, "\n");
    }
}