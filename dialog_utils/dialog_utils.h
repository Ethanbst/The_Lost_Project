#ifndef DIALOG_UTILS_H
#define DIALOG_UTILS_H

#include <SDL.h>
#include <SDL_ttf.h>

#define NB_DIALOGS 20
void display_dialog(SDL_Renderer *renderer, char *text);
void display_dialogs(SDL_Renderer *renderer, int id);
bool is_dialog_not_done(int id, int *dialogs_done);
#endif