#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "../debug_var.h"

typedef struct color{
    int r; //Red
    int g; //Green
    int b; //Blue
    int a; //Alpha
} color;

typedef struct CTA{
    int pos_x; //Position en x sur l'écran
    int pos_y; //Position en y sur l'écran
    int w; //Longueur en px du CTA
    int h; //Largeur en px du CTA
} CTA;

CTA draw_button(SDL_Renderer *renderer, int pos_x, int pos_y, int multiply_size, const char *text, int need_bg, int font_size, color couleur) {
    
    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", font_size);

    SDL_Color textColor; // Text color
    textColor.r = couleur.r;
    textColor.g = couleur.g;
    textColor.b = couleur.b;
    textColor.a = couleur.a;
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int text_width = textSurface->w;
    int text_height = textSurface->h;
    

    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    // Ajuster la taille du bouton en fonction de la résolution de l'écran
    int adjusted_width = (text_width * multiply_size * screen_w) / 1920; // 1920 est la largeur de référence
    int adjusted_height = (text_height * multiply_size * screen_h) / 1080; // 1080 est la hauteur de référence

    
    //SDL_Rect text_rect = {pos_x-((text_width/2)*multiply_size), pos_y, multiply_size*text_width, multiply_size*text_height}; //Taille et pos du texte
    SDL_Rect text_rect = {pos_x-(adjusted_width / 2), pos_y-(adjusted_height / 2), adjusted_width, adjusted_height}; // Taille et pos du texte

    
    if(need_bg){ //Texte supplémentaire pour effet 3D / Fond pour meilleurs lecture
        SDL_Color textColor2 = {0, 0, 0, 0}; // Text color
        SDL_Surface *textSurface2 = TTF_RenderText_Solid(font, text, textColor2);
        SDL_Texture *textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        SDL_Rect text_rect_bg = {text_rect.x, text_rect.y+(screen_h/300), text_rect.w, text_rect.h}; //Taille et pos du texte
        SDL_RenderCopy(renderer, textTexture2, NULL, &text_rect_bg);
        SDL_FreeSurface(textSurface2);
        SDL_DestroyTexture(textTexture2);    
    }

    SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);

    CTA button;
    button.h = text_rect.h;
    button.w = text_rect.w;
    button.pos_x = text_rect.x;
    button.pos_y = text_rect.y;

    //printf("x:%d, y:%d, w:%d, h:%d\n", button.pos_x, button.pos_y, button.w, button.h);
    if(button_hitbox){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &text_rect);
    }
    
    return button;
}

CTA get_button(SDL_Renderer *renderer, const char *text, int font_size) {
    
    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", font_size);

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, (SDL_Color){255, 255, 255, 255});
    int text_width = textSurface->w;
    int text_height = textSurface->h;

    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    // Ajuster la taille du bouton en fonction de la résolution de l'écran
    int adjusted_width = (text_width * 1 * screen_w) / 1920; // 1920 est la largeur de référence
    int adjusted_height = (text_height * 1 * screen_h) / 1080; // 1080 est la hauteur de référence

    
    //SDL_Rect text_rect = {pos_x-((text_width/2)*multiply_size), pos_y, multiply_size*text_width, multiply_size*text_height}; //Taille et pos du texte
    SDL_Rect text_rect = {(adjusted_width / 2), (adjusted_height / 2), adjusted_width, adjusted_height}; // Taille et pos du texte

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

    CTA button;
    button.h = text_rect.h;
    button.w = text_rect.w;
    button.pos_x = NULL;
    button.pos_y = NULL;
    
    return button;
}

CTA draw_slider(SDL_Renderer *renderer, int pos_x, int pos_y, int w, int h, int value, int max_value) {

    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    // Ajuster la taille du slider en fonction de la résolution de l'écran
    w = (w * screen_w) / 1920; // 1920 est la largeur de référence
    h = (h * screen_h) / 1080; // 1080 est la hauteur de référence

    // Rectangle contenant les éléments du slider
    SDL_Rect slider_bg = {pos_x, pos_y, w, h}; //Rectangle contenant le fond du slider
    SDL_Rect slider_fg = {pos_x, pos_y, (w * value) / max_value, h}; //Rectangle contenant le remplissage du slider

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
    slider.h = slider_bg.h;
    slider.w = slider_bg.w;
    slider.pos_x = slider_bg.x;
    slider.pos_y = slider_bg.y;

    return slider;
}