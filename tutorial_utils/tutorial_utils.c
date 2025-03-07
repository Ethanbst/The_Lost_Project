#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "../cta_utils/cta.h"
#include "../logs_utils/log.h"

//Fonction qui affiche à l'écran le touches de déplacement
void display_controls(SDL_Renderer *renderer, int x, int y){
    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 72);
    if (!font) {
        add_log_error("menu.c - menu()", "Erreur lors du chargement de la police.");
        return;
    }

    CTA lettre = draw_button(renderer, x, y, 2, "Z", 1, 72); //Lettre de placement initial

    draw_button(renderer, x, y-lettre.h/2-lettre.h/6, 1, "Fais bouger ton personnage", 1, 72);
    
    draw_button(renderer, x, y+lettre.h/2+lettre.h/6, 2, "S", 1, 72);
    draw_button(renderer, x-lettre.w, y+lettre.h/2+lettre.h/6, 2, "Q", 1, 72);
    draw_button(renderer, x+lettre.w, y+lettre.h/2+lettre.h/6, 2, "D", 1, 72);

}