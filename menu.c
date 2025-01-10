#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "logs_utils/log.c"
#include "settings.c"

// Fonction pour afficher du texte à l'écran
SDL_Texture* loadText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color){
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Fonction pour libérer les ressources du menu et réinitialiser l'affichage
void menu_exit(SDL_Renderer *renderer, SDL_Texture *playTexture, SDL_Texture *optionsTexture, SDL_Texture *quitTexture, SDL_Texture *bg_menu_texture, TTF_Font *font, Mix_Music *music) {
    // Libérer les textures
    if (playTexture) SDL_DestroyTexture(playTexture);
    if (optionsTexture) SDL_DestroyTexture(optionsTexture);
    if (quitTexture) SDL_DestroyTexture(quitTexture);
    if (bg_menu_texture) SDL_DestroyTexture(bg_menu_texture);

    // Fermer la police
    if (font) TTF_CloseFont(font);

    // Libérer la musique
    //if (music) Mix_FreeMusic(music);

    // Quitter SDL_ttf et SDL_mixer
    TTF_Quit();
    //Mix_CloseAudio();
    //Mix_Quit();

    // Effacer l'écran et présenter un écran noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur noire
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

//Fonction de gestion du menu principal:
void menu(SDL_Renderer *renderer, SDL_Window *window) {

    create_log_file();
    add_log("MENU","Menu principal.\n");


    //Implémentation de l'audio du menu:
    int init2 = Mix_Init(0);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); //Ouverture du fichier audio
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Réglez le volume de la musique à 50%
    
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



    //Ajout de l'image de fond du menu:
    SDL_Surface *bg_menu_surface = IMG_Load("res/bg/menu_bg.bmp");
    if(!bg_menu_surface){
        add_log("MENU","Erreur du chargement de l'image de fond du menu\n");
    }
    SDL_Texture *bg_menu_texture = SDL_CreateTextureFromSurface(renderer, bg_menu_surface);
    SDL_FreeSurface(bg_menu_surface);
    


    //Texte pour les touches:
    if (TTF_Init() == -1) {
        add_log("MENU","Erreur d'initialisation SDL_ttf");
        //return -1;
    }

    //Charger la police d'écriture
    TTF_Font* font = TTF_OpenFont("res/font/Pixelfy.ttf", 28);
    if (!font) {
        add_log("MENU","Erreur lors du chargement de la police");
        //return;
    }

    //Couleur pour le texte
    SDL_Color color = {255, 255, 255}; // Blanc

    //Créer les textures pour les options du menu
    SDL_Texture* playTexture = loadText(renderer, font, "Jouer", color);
    SDL_Texture* optionsTexture = loadText(renderer, font, "Options", color);
    SDL_Texture* quitTexture = loadText(renderer, font, "Quitter", color);

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;
    //printf("Width = %d, Height = %d\n", width, height);
    add_log("MENU","Width = %d, Height = %d\n");


    int t_bt_x = 200; //Longueur d'un bouton
    int t_bt_y = t_bt_x / 2; //Hauteur d'un bouton
    int esp_bt = t_bt_x; //Espacement entre chaque bouton

    int pos_bt_y = height / 3;
    int pos_bt_x = width - (width/2 + t_bt_x/2);
    

    //Définir les positions des options du menu
    SDL_Rect playRect = {pos_bt_x, pos_bt_y, t_bt_x, t_bt_y};//Bouton "Jouer"
    SDL_Rect optionsRect = {pos_bt_x, pos_bt_y+esp_bt, t_bt_x, t_bt_y};//Bouton "Options"
    SDL_Rect quitRect = {pos_bt_x, pos_bt_y+esp_bt*2, t_bt_x, t_bt_y};//Bouton "Quitter"

    //Effacer l'écran
    //SDL_RenderClear(renderer);

    //Afficher le fond du menu, NULL et NULL = dessinée sur la fenêtre complète
    SDL_RenderCopy(renderer, bg_menu_texture, NULL, NULL);

    //Afficher les boutons du menu
    SDL_RenderCopy(renderer, playTexture, NULL, &playRect);
    SDL_RenderCopy(renderer, optionsTexture, NULL, &optionsRect);
    SDL_RenderCopy(renderer, quitTexture, NULL, &quitRect);

    //Présenter le rendu
    SDL_RenderPresent(renderer);

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
                if (x > playRect.x && x < playRect.x + playRect.w &&
                    y > playRect.y && y < playRect.y + playRect.h) {
                    add_log("MENU","Jouer sélectionné\n");
                    Mix_FadeOutMusic(2000);
                    //jeu(window, renderer);
                }

                //Vérifier si "Options" est cliqué
                if (x > optionsRect.x && x < optionsRect.x + optionsRect.w &&
                    y > optionsRect.y && y < optionsRect.y + optionsRect.h) {
                    add_log("MENU","Options sélectionné\n");
                    SDL_RenderClear(renderer);
                    menu_exit(renderer, playTexture, optionsTexture, quitTexture, bg_menu_texture, font, music);
                    options(renderer, window);
                }

                // Vérifier si "Quitter" est cliqué
                if (x > quitRect.x && x < quitRect.x + quitRect.w &&
                    y > quitRect.y && y < quitRect.y + quitRect.h) {
                    add_log("MENU","Quitter sélectionné\n");
                    running = 0;  // Quitter le menu
                }
            }
        }
    }

    //Libérer les ressources
    SDL_DestroyTexture(playTexture);
    SDL_DestroyTexture(optionsTexture);
    SDL_DestroyTexture(quitTexture);
    SDL_DestroyTexture(bg_menu_texture);
    TTF_CloseFont(font);
}