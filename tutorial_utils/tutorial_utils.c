#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "../cta_utils/cta.h"
#include "../logs_utils/log.h"

//Fonction qui affiche à l'écran le touches de déplacement
void display_controls(SDL_Renderer *renderer, int x, int y, int z, int q, int s, int d) {
    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 72);
    if (!font) {
        add_log_error("menu.c - menu()", "Erreur lors du chargement de la police.");
        return;
    }

    int screen_width, screen_height;
    //Récupère la taille de la fenêtre
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

    SDL_Rect bg_rect = {screen_width/4, screen_height/4, screen_width/2, screen_height/2};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, &bg_rect);

    color c_z = {255, 255, 255, 255};
    color c_q = {255, 255, 255, 255};
    color c_s = {255, 255, 255, 255};
    color c_d = {255, 255, 255, 255};
    color white = {255, 255, 255, 255};

    if(z){
        c_z = (color){0, 255, 0, 255};
    }
    if(q){
        c_q = (color){0, 255, 0, 255};
    }
    if(s){
        c_s = (color){0, 255, 0, 255};
    }
    if(d){
        c_d = (color){0, 255, 0, 255};
    }

    CTA lettre = draw_button(renderer, x, y, 2, "Z", 1, 72, c_z); //Lettre de placement initial

    draw_button(renderer, x, y-lettre.h/2-lettre.h/6, 1, "Fais bouger ton personnage", 1, 72, white);
    
    draw_button(renderer, x, y+lettre.h/2+lettre.h/6, 2, "S", 1, 72, c_s);
    draw_button(renderer, x-lettre.w, y+lettre.h/2+lettre.h/6, 2, "Q", 1, 72, c_q);
    draw_button(renderer, x+lettre.w, y+lettre.h/2+lettre.h/6, 2, "D", 1, 72, c_d);

}