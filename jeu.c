#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "logs_utils/log.h"
#include "pause_menu/pause_menu.c"
#include "debug_var.h"
#include "worlds/worlds_utils.h"
#include "player_utils/player.h"


// Fonction pour vérifier les collisions en fonction du centre du joueur
int no_obstacle2(enum Direction direction, player *player, world world, int cellSize) {
    int centerX = player->player_rect.x + player->player_rect.w / 2;
    int centerY = player->player_rect.y + player->player_rect.h;

    if (direction == HAUT) {
        if (world.matrice[(centerY - 50) / cellSize][centerX / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    if (direction == BAS) {
        if (world.matrice[(centerY + 10) / cellSize][centerX / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    if (direction == GAUCHE) {
        if (world.matrice[centerY / cellSize][(centerX - 50) / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }

    if (direction == DROITE) {
        if (world.matrice[centerY / cellSize][(centerX + 50) / cellSize] == 1) {
            add_log("JEU", "Obstacle!\n");
            return 0;
        }
    }
    return 1;
}

// Taille d'une cellule de la matrice en pixels
int cellSize;
//On récupère la taille de la fenêtre pour bien placer les boutons par la suite
int width, height;

// Fonction pour initialiser la taille de la fenêtre
void initWindowSize() {
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;
    cellSize = width / 20;
}

void updatePlayerPositionInMatrix2(world world, player *player, int cellSize) {
    player->MposX = player->player_rect.x / cellSize;
    player->MposY = player->player_rect.y / cellSize;
}

int mouvement2(const Uint8 *state, world world, player *player){
    int oldMposX = player->MposX;
    int oldMposY = player->MposY;

    // printf("Joueur in matrix: %d, %d\n", player->MposX, player->MposY);
    
    if(state[SDL_SCANCODE_W] && no_obstacle2(HAUT, player, world, cellSize)){
        // add_log("MOUVEMENT","Z pressee\n");
        player->player_rect.y -= 10;
        player->player_texture = player->img_dir_joueur[HAUT];
    }

    if(state[SDL_SCANCODE_A] && no_obstacle2(GAUCHE, player, world, cellSize)){
        // add_log("MOUVEMENT","Q pressee\n");
        player->player_rect.x -= 10;
        player->player_texture = player->img_dir_joueur[GAUCHE];
    }

    if(state[SDL_SCANCODE_S] && no_obstacle2(BAS, player, world, cellSize)){
        // add_log("MOUVEMENT","S pressee\n");
        player->player_rect.y += 10;
        player->player_texture = player->img_dir_joueur[BAS];
    }

    if(state[SDL_SCANCODE_D] && no_obstacle2(DROITE, player, world, cellSize)){
        // add_log("MOUVEMENT","D pressee\n");
        player->player_rect.x += 10;
        player->player_texture = player->img_dir_joueur[DROITE];
    }
    
    // Mettre à jour la position du joueur dans la matrice si elle a changé
    updatePlayerPositionInMatrix2(world, player, cellSize);

    // Vérifier si la position dans la matrice a changé
    if (player->MposX != oldMposX || player->MposY != oldMposY) {
        // add_log("MOUVEMENT", "Position dans la matrice mise à jour\n");
    }

    if(player->MposX == world.end_spawn.x && player->MposY == world.end_spawn.y){ //Indication de passage au prochain niveau
        return 1;
    }
    if(player->MposX == world.start_spawn.x && player->MposY == world.start_spawn.y){ //Indication de passage au niveau précédant
        //return -1;
    }
    else{
        return 0;
    }

    return 0;
}

//Créér une texture pour les murs et la retourne
SDL_Texture* get_world_texture(SDL_Window *window, world world){

    SDL_Renderer *renderer = SDL_GetRenderer(window);

    //Texture du mur
    SDL_Texture *mur_texture = loadTexture(world.wall_texture_path, renderer);

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;

    SDL_Texture *world_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetRenderTarget(renderer, world_texture);

    SDL_Rect MurRect;
    MurRect.w = width / 20;
    MurRect.h = MurRect.w;

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 20; j++) {
            if (world.matrice[i][j] == 1) {
                MurRect.x = j * MurRect.w;
                MurRect.y = i * MurRect.h;
                SDL_RenderCopy(renderer, mur_texture, NULL, &MurRect);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);

    world.global_texture = world_texture;
    //Structure contenant la texture et le rectangle du mur
    //Map *map = (Map*)malloc(sizeof(Map));
    //map->texture = map_texture;

    return world_texture;
}

//Fonction de lancement du jeu
void jeu(SDL_Window *window, SDL_Renderer *renderer, world *actual_world){


    SDL_RenderClear(renderer);
    initWindowSize();

    Mix_FadeInMusic(actual_world->music, -1, 2000);

    actual_world->global_texture = get_world_texture(window, *actual_world);

    player player = init_player(renderer, *actual_world);
    player.img_dir_joueur[BAS]=loadTexture("res/joueur/joueurB.png", renderer);

    int continuer = 0;

    SDL_Event event; //Variable qui reçoit l'évènement
    const Uint8 *state = SDL_GetKeyboardState(NULL); //Récupère l'état du clavier
    Uint32 lastTime = SDL_GetTicks(), currentTime; //Variables pour le temps
    
    SDL_Rect player_hitboxRect;
    int centerX;
    int centerY;
    player_hitboxRect = player.player_rect; //rectangle de debugage du joueur
    
    while(continuer == 0){
        
        continuer = mouvement2(state, *actual_world, &player); //Déplace le joueur
        SDL_RenderClear(renderer); //Efface l'écran
        SDL_RenderCopy(renderer, actual_world->global_texture, NULL, NULL); //Rend la texture des murs
        SDL_RenderCopy(renderer, player.player_texture, NULL, &player.player_rect); //Rend le joueur car sa position à changé
        if(player_hitbox){
            centerX = player.player_rect.x + player.player_rect.w / 2;
            centerY = player.player_rect.y + player.player_rect.h / 2;
            player_hitboxRect.x = centerX - player.player_rect.w / 2;
            player_hitboxRect.y = centerY - player.player_rect.h / 2;
            player_hitboxRect.w = player.player_rect.w;
            player_hitboxRect.h = player.player_rect.h;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &player_hitboxRect);
        }
        SDL_RenderPresent(renderer); //Présente le rendu
        
        SDL_Delay(16); //Délai pour éviter de bouger trop vite

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                continuer = -2;
            }
            else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
                continuer = pause(renderer);
            }
        }
    }
    
    //Libération de ressources de la fonction jeu
    free_player(player);
    SDL_RenderClear(renderer); //Efface l'écran

    /*if(continuer == -1){ //Retour précédent
        add_log("JEU:", "PREVIOUS_WORLD\n");
        world previous_world = get_world_info((char *)actual_world->previous_world);
        jeu(window, renderer, &previous_world);
    }*/

    if(continuer == 1){ //Aller monde suivant
        char *next_world = (char*)malloc(strlen(actual_world->next_world) + 1);
        if (next_world == NULL) {
            add_log("JEU", "Memory allocation failed for next_world\n");
            return;
        }
        strcpy(next_world, actual_world->next_world);

        printf("\nnext:%s\n", actual_world->next_world);

        //free_world(actual_world);

        //actual_world = get_world_info(next_world);
        //jeu(window, renderer, &next_world);
    }
    add_log("JEU","Sortie while jeu()\n");
}