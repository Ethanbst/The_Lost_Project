#include <SDL.h>
#include <SDL_ttf.h>

typedef struct CTA{
    int pos_x; //Position en x sur l'écran
    int pox_y; //Position en y sur l'écran
    int w; //Longueur en px du CTA
    int h; //Largeur en px du CTA
} CTA;

CTA draw_button(SDL_Renderer *renderer, int pos_x, int pos_y, int multiply_size, const char *text, TTF_Font *font, int need_bg) {

    SDL_Color textColor = {255, 255, 255, 255}; // Text color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect text_rect = {pos_x-((text_width/2)*multiply_size), pos_y, multiply_size*text_width, multiply_size*text_height}; //Taille et pos du texte

    if(need_bg){ //Texte supplémentaire pour effet 3D / Fond pour meilleurs lecture
        SDL_Color textColor2 = {0, 0, 0, 0}; // Text color
        SDL_Surface *textSurface2 = TTF_RenderText_Solid(font, text, textColor2);
        SDL_Texture *textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        SDL_Rect text_rect_bg = {text_rect.x, text_rect.y+10, text_rect.w, text_rect.h}; //Taille et pos du texte
        SDL_RenderCopy(renderer, textTexture2, NULL, &text_rect_bg);
        SDL_FreeSurface(textSurface2);
        SDL_DestroyTexture(textTexture2);    
    }

    SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    CTA button;
    button.h = text_rect.h;
    button.w = text_rect.w;
    button.pos_x = text_rect.x;
    button.pox_y = text_rect.y;

    return button;
}

CTA draw_slider(SDL_Renderer *renderer, int pos_x, int pos_y, int w, int h, int value, int max_value) {

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
    slider.h = h;
    slider.w = w;
    slider.pos_x = pos_x;
    slider.pox_y = pos_y;

    return slider;
}