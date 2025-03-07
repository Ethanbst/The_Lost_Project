#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "../logs_utils/log.h"
#include "../settings.c"
#include "../jeu.c"
#include "../mouse_utils/mouse.h"
#include "../worlds/worlds_utils.h"
#include "../save_utils/save.h"

//Fonction pour afficher du texte à l'écran
SDL_Texture* loadText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color){
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

//Libère les ressources du menu une fois qu'elles ont été rendu.
void free_menu(SDL_Renderer *renderer, SDL_Texture *bg_menu_texture, SDL_Surface *bg_menu_surface, SDL_Texture *logo_menu_texture, SDL_Surface *logo_menu_surface) {
    add_log_info("menu.c - free_menu()", "Liberation des ressources du menu.");
    //Libération des ressources
    if (bg_menu_texture) SDL_DestroyTexture(bg_menu_texture);
    if (bg_menu_surface) SDL_FreeSurface(bg_menu_surface);
    add_log_info("menu.c - free_menu()", "Ressources du menu liberees.");
}


//Permet de réafficher le menu après être entré dans une autre fonction
void menu_start(SDL_Renderer *renderer, SDL_Surface *bg_menu_surface, SDL_Texture *bg_menu_texture, SDL_Texture *logo_menu_texture, SDL_Surface *logo_menu_surface){
    add_log_info("menu.c - menu_start()", "Reaffichage du menu.");
    //Rechargement de l'image de fond du menu:
    //Réattribution de la texture*/
    bg_menu_surface = IMG_Load("res/bg/menu_bg.bmp");
    if(!bg_menu_surface){
        add_log_error("menu.c - menu_start()","Erreur lors du chargement de l'image de fond du menu.");
        return;
    }
    bg_menu_texture = SDL_CreateTextureFromSurface(renderer, bg_menu_surface);
    SDL_RenderCopy(renderer, bg_menu_texture, NULL, NULL);

    //Logo du jeu pour le menu
    logo_menu_surface = IMG_Load("res/bg/logo.png");
    if(!logo_menu_surface){
        add_log_error("menu.c - menu_start()","Erreur lors du chargement de l'image du logo.");
        return;
    }
    logo_menu_texture = SDL_CreateTextureFromSurface(renderer, logo_menu_surface);

    SDL_DisplayMode ecran = GetScreenSize();
    // Définir la position et la taille du rectangle pour le logo
    SDL_Rect logo_rect;
    logo_rect.w = logo_menu_surface->w/2;
    logo_rect.h = logo_menu_surface->h/2;
    logo_rect.x = ecran.w/2 - logo_rect.w/2; // Centrer horizontalement
    logo_rect.y = -100; // Positionner en haut de l'écran
    SDL_RenderCopy(renderer, logo_menu_texture, NULL, &logo_rect);

    color couleur = {255, 255, 255, 255}; //Couleur blanche

    CTA version = draw_button(renderer, ecran.w-200, ecran.h-120, 1, "Alpha 0.28.11", 1, 48, couleur);
    draw_button(renderer, ecran.w-200, version.pos_y+version.h+20, 1, "By E. Bastien", 1, 48, couleur);


    //Couleur pour le texte
    SDL_Color color = {255, 255, 255}; // Blanc
    
    int screen_width, screen_height;
    //Récupère la taille de la fenêtre
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

    int esp_bt = screen_height / 8; // Espacement entre chaque bouton proportionnel à la hauteur de l'écran

    int pos_bt_y = screen_height - (screen_height/4);
    int pos_bt_x = screen_width/2;

    const char* playButtonText;
    if(exist_save()){
        playButtonText = "Continuer";
    }
    else{
        playButtonText = "Jouer";
    }


    CTA play_button  = draw_button(renderer, pos_bt_x, pos_bt_y, 1, playButtonText, 1, 72, couleur);
    CTA option_button  = draw_button(renderer, pos_bt_x, play_button.pos_y+esp_bt, 1, "Options", 1, 72, couleur);
    CTA quit_button  = draw_button(renderer, pos_bt_x, option_button.pos_y+esp_bt, 1, "Quitter", 1, 72, couleur);

    //Présenter le rendu
    SDL_RenderPresent(renderer);
    //On libère les ressources maintenant qu'elles sont déjà rendu.
    free_menu(renderer, bg_menu_texture, bg_menu_surface, logo_menu_texture, logo_menu_surface);
}

//Fonction de gestion du menu principal:
void menu(SDL_Renderer *renderer, SDL_Window *window) {
    add_log_info("menu.c - menu()", "Menu principal.");

    init_default_settings2();

    int music_volume = get_setting_value2("music_volume"); //On récupère la valeur de notre fichier afin de l'appliquer

    //Première initialisation du menu
    //Implémentation de l'audio du menu:
    int init2 = Mix_Init(0);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); //Ouverture du fichier audio
    Mix_VolumeMusic(music_volume); // Réglez le volume de la musique en fonction de settings.txt
    //Mix_PlayMusic(world.music, 1);

    //Musique:
    Mix_Music *music = Mix_LoadMUS("res/music/menu.wav");
    if(!music){
        add_log_error("menu.c - menu()", "Erreur lors du chargement de la musique du menu");
        return;
    }
    else{
        add_log("menu.c - menu()", "Musique du menu chargee.");
    }
    Mix_FadeInMusic(music, -1, 2000);

    //Texte pour les touches:
    if (TTF_Init() == -1) {
        add_log_error("menu.c - menu()", "Erreur d'initialisation SDL_ttf");
        return;
    }
    else{
        add_log("menu.c - menu()", "SDL_ttf initialise.");
    }


    SDL_Surface *bg_menu_surface;
    SDL_Texture *bg_menu_texture;

    SDL_Surface *logo_menu_surface;
    SDL_Texture *logo_menu_texture;

    menu_start(renderer, bg_menu_surface, bg_menu_texture, logo_menu_texture, logo_menu_surface);

    int screen_width, screen_height;

    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode); //Récupère les info de la fenêtre
    screen_height = displayMode.h;
    screen_width = displayMode.w;

    int esp_bt = screen_height / 8; // Espacement entre chaque bouton proportionnel à la hauteur de l'écran

    int pos_bt_y = screen_height - (screen_height/4);
    int pos_bt_x = screen_width/2;

    const char* playButtonText;
    if(exist_save()){
        playButtonText = "Continuer";
    }
    else{
        playButtonText = "Jouer";
    }

    color couleur = {255, 255, 255, 255}; //Couleur blanche

    CTA play_button  = draw_button(renderer, pos_bt_x, pos_bt_y, 1, playButtonText, 1, 72, couleur);
    CTA option_button  = draw_button(renderer, pos_bt_x, play_button.pos_y+esp_bt, 1, "Options", 1, 72, couleur);
    CTA quit_button  = draw_button(renderer, pos_bt_x, option_button.pos_y+esp_bt, 1, "Quitter", 1, 72, couleur);

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
                if (is_mouse_on(play_button)) {
                    add_log("menu.c - menu()","Jouer.");
                    
                    Mix_FadeOutMusic(1000);
                    
                    reset_cursor();
                    
                    //Mix_FadeOutMusic(1000);
                    
                    double music_pos = Mix_GetMusicPosition(music);

                    world *world = NULL;
                    //Contrôle si sauvegarde déjà existante
                    if(exist_save()){ //Si une sauvegarde existe on charge à partir du dernier monde sauvegardé
                        world = get_world_info(getlastworldname());
                    }
                    else{ //Sinon on charge le monde 1
                        add_log_info("menu.c - menu()","Aucune sauvegarde trouvée, chargement du monde 1.");
                        world = get_world_info((char *)"world1.json");
                    }

                    if(world == NULL){
                        add_log_error("menu.c - menu()","Echec de récupération des informations du monde");
                        return;
                    }

                    char *current_music_path = (char *)malloc(sizeof("res/music/menu.wav")); //Variable contenante le chemin de la musique actuelle
                    if(current_music_path == NULL){
                        add_log_error("menu.c - menu()","Echec allocation memoire pour la musique");
                        return;
                    }
                    strcpy(current_music_path, "res/music/menu.wav");
                    
                    jeu(window, renderer, world, current_music_path);
                    menu_start(renderer, bg_menu_surface, bg_menu_texture, logo_menu_texture, logo_menu_surface);
                    Mix_FadeInMusicPos(music, -1, 1000, music_pos+1);
                }

                //Vérifier si "Options" est cliqué
                if (is_mouse_on(option_button)) {
                    add_log_info("menu.c - menu()", "Options.");
                    //free_menu(renderer, bg_menu_texture, bg_menu_surface, font);
                    reset_cursor();
                    options(renderer);
                    menu_start(renderer, bg_menu_surface, bg_menu_texture, logo_menu_texture, logo_menu_surface);
                }

                // Vérifier si "Quitter" est cliqué
                if (is_mouse_on(quit_button)) {
                    add_log_info("menu.c - menu()", "Quitter.");
                    running = 0;  // Quitter le menu
                }
            }
            else if(event.type == SDL_MOUSEMOTION){ //évite le contrôle si la souris ne bouge pas
                //Récupération de la position de la souris
                int x, y;
                Uint32 mouse_state = SDL_GetMouseState(&x, &y);
                int actual_cursor;

                //Vérifier si passage sur un bouton
                if (is_mouse_on(play_button) || is_mouse_on(option_button) || is_mouse_on(quit_button)) {
                        if(actual_cursor != SDL_SYSTEM_CURSOR_HAND){
                            actual_cursor = set_hand_cursor();
                        }
                }
                else if(actual_cursor != SDL_SYSTEM_CURSOR_ARROW){
                    actual_cursor = reset_cursor();
                }

            }
        }
    }
    Mix_FreeMusic(music);
}