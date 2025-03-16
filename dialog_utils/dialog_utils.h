#ifndef DIALOG_UTILS_H
#define DIALOG_UTILS_H

#include <SDL.h>
#include <SDL_ttf.h>
void display_dialog(SDL_Renderer *renderer, char *text);
void display_dialogs(SDL_Renderer *renderer, int id);
#endif