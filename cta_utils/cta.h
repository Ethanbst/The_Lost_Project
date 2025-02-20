#ifndef cta_h

#define cta_h

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct CTA{
    int pos_x; //Position en x du CTA
    int pox_y; //Position en y du CTA
    int w; //Longueur du CTA
    int h; //Largeur du CTA
} CTA;

CTA draw_button(SDL_Renderer *renderer, int pos_x, int pos_y, int multiply_size, const char *text, TTF_Font *font, int need_bg);
CTA draw_slider(SDL_Renderer *renderer, int pos_x, int pos_y, int w, int h, int value, int max_value);
#endif