#include <SDL.h>
#include <SDL_ttf.h>

typedef struct CTA{
    int pos_x; //Position en x du CTA
    int pox_y; //Position en y du CTA
    int w; //Longueur du CTA
    int h; //Largeur du CTA
} CTA;

CTA draw_button(SDL_Renderer *renderer, int pos_x, int pos_y, int multiply_size, const char *text, TTF_Font *font) {
    SDL_Color textColor = {255, 255, 255, 255}; // Text color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect text_rect = {pos_x, pos_y, multiply_size*text_width, multiply_size*text_height}; //Taille et pos du texte

    //SDL_Rect bg_text_rect = {pos_x, pos_y, multiply_size*text_width, multiply_size*text_height}; //Taille et pos du texte


    SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    CTA button;
    button.h = text_rect.h;
    button.w = text_rect.w;
    button.pos_x = pos_x;
    button.pox_y = pos_y;

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