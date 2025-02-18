#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "../logs_utils/log.h"
#include "../settings.c"
#include "../jeu.c"

//Fonction pour afficher du texte à l'écran
SDL_Texture* loadText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color){
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

//Libère les ressources du menu une fois qu'elles ont été rendu.
void free_menu(SDL_Renderer *renderer, SDL_Texture *bg_menu_texture, SDL_Surface *bg_menu_surface, TTF_Font *font) {
    //Libération des ressources
    if (bg_menu_texture) SDL_DestroyTexture(bg_menu_texture);
    if (bg_menu_surface) SDL_FreeSurface(bg_menu_surface);
    TTF_CloseFont(font);
    add_log("free_menu", "Ressources menu liberees.\n");
}


//Permet de réafficher le menu après être entré dans une autre fonction
void menu_start(SDL_Renderer *renderer, SDL_Surface *bg_menu_surface, SDL_Texture *bg_menu_texture, TTF_Font *font, Mix_Music *music){
    add_log("menu_start()","\n");
    //Rechargement de l'image de fond du menu:
    //Réattribution de la texture*/
    bg_menu_surface = IMG_Load("res/bg/menu_bg.bmp");
    if(!bg_menu_surface){
        add_log("MENU","Erreur du chargement de l'image de fond du menu\n");
    }
    bg_menu_texture = SDL_CreateTextureFromSurface(renderer, bg_menu_surface);
    SDL_RenderCopy(renderer, bg_menu_texture, NULL, NULL);

    font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 72);
    if (!font) {
        add_log("MENU","Erreur lors du chargement de la police\n");
        //return;
    }


    //Couleur pour le texte
    SDL_Color color = {255, 255, 255}; // Blanc
    
    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre = GetScreenSize();
    width = taille_fenetre.w;
    height = taille_fenetre.h;


    int t_bt_x = 400; //Longueur d'un bouton
    int t_bt_y = t_bt_x / 2; //Hauteur d'un bouton
    int esp_bt = t_bt_x; //Espacement entre chaque bouton

    int pos_bt_y = height - (height/3);
    int pos_bt_x = width - (width/2 + t_bt_x/2);


    CTA play_button  = draw_button(renderer, pos_bt_x, pos_bt_y, 2, "Jouer", font);
    CTA option_button  = draw_button(renderer, pos_bt_x, pos_bt_y+200, 2, "Options", font);
    CTA quit_button  = draw_button(renderer, pos_bt_x, option_button.pox_y+200, 2, "Quitter", font);


    //Présenter le rendu
    SDL_RenderPresent(renderer);
    //On libère les ressources maintenant qu'elles sont déjà rendu.
    free_menu(renderer, bg_menu_texture, bg_menu_surface, font);
}

//Fonction de gestion du menu principal:
void menu(SDL_Renderer *renderer, SDL_Window *window) {

    add_log("MENU","Menu principal.\n");

    init_default_settings();

    int music_volume = get_setting_value("music_volume"); //On récupère la valeur de notre fichier afin de l'appliquer

    //Première initialisation du menu
    //Implémentation de l'audio du menu:
    int init2 = Mix_Init(0);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); //Ouverture du fichier audio
    Mix_VolumeMusic(music_volume); // Réglez le volume de la musique en fonction de settings.txt
    
    //Musique:
    Mix_Music *music = Mix_LoadMUS("res/music/menu.wav");
    if(!music){
        add_log("MENU","Erreur chargement menu.wav\n");
    }
    else{
        add_log("MENU","menu.wav chargee avec succes.\n");
    }

    //Mix_PlayMusic(music, 1);
    Mix_FadeInMusic(music, -1, 2000);

    //Texte pour les touches:
    if (TTF_Init() == -1) {
        add_log("MENU","Erreur d'initialisation SDL_ttf\n");
        //return -1;
    }
    else{
        add_log("MENU","SDL_ttf initilise\n");
    }


    SDL_Surface *bg_menu_surface;
    SDL_Texture *bg_menu_texture;
    TTF_Font *font;
    font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 72);
    if (!font) {
        add_log("MENU","Erreur lors du chargement de la police\n");
        //return;
    }
    menu_start(renderer, bg_menu_surface, bg_menu_texture, font, music);

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre = GetScreenSize();
    width = taille_fenetre.w;
    height = taille_fenetre.h;

    int t_bt_x = 400; //Longueur d'un bouton
    int t_bt_y = t_bt_x / 2; //Hauteur d'un bouton
    int esp_bt = t_bt_x; //Espacement entre chaque bouton

    int pos_bt_y = height - (height/3);
    int pos_bt_x = width - (width/2 + t_bt_x/2);

    CTA play_button  = draw_button(renderer, pos_bt_x, pos_bt_y, 2, "Jouer", font);
    CTA option_button  = draw_button(renderer, pos_bt_x, pos_bt_y+200, 2, "Options", font);
    CTA quit_button  = draw_button(renderer, pos_bt_x, option_button.pox_y+200, 2, "Quitter", font);
    add_log("MENU","PASS\n");

    //Présenter le rendu
    //SDL_RenderPresent(renderer);
    //free_menu(renderer, playTexture, optionsTexture, quitTexture, bg_menu_texture);*/

    //Boucle principale:
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } 
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                //Position ou le clique a été effectué:
                int x = event.button.x;
                int y = event.button.y;

                //Vérifier si "Jouer" est cliqué
                if (x >= play_button.pos_x && x <= play_button.pos_x + play_button.w &&
                    y >= play_button.pox_y && y < play_button.pox_y + play_button.h) {
                    add_log("MENU","Jouer sélectionné\n");
                    Mix_FadeOutMusic(1000);
                    free_menu(renderer, bg_menu_texture, bg_menu_surface, font);
                    jeu(window, renderer);
                }

                //Vérifier si "Options" est cliqué
                if (x >= option_button.pos_x && x <= option_button.pos_x + option_button.w &&
                    y >= option_button.pox_y && y < option_button.pox_y + option_button.h) {
                    add_log("MENU","Options sélectionné\n");
                    free_menu(renderer, bg_menu_texture, bg_menu_surface, font);
                    options(renderer, window);
                    menu_start(renderer, bg_menu_surface, bg_menu_texture, font, music);
                }

                // Vérifier si "Quitter" est cliqué
                if (x >= quit_button.pos_x && x <= quit_button.pos_x + quit_button.w &&
                    y >= quit_button.pox_y && y < quit_button.pox_y + quit_button.h) {
                    add_log("MENU","Quitter sélectionné\n");
                    running = 0;  // Quitter le menu
                }
            }
        }
    }

    //Libérer les ressources
    //SDL_DestroyTexture(playTexture);
    //SDL_DestroyTexture(optionsTexture);
    //SDL_DestroyTexture(quitTexture);
    //SDL_DestroyTexture(bg_menu_texture);
    //TTF_Quit();
}