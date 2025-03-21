#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "logs_utils/log.h"
#include "pause_menu/pause_menu.c"
#include "debug_var.h"
#include "worlds/worlds_utils.h"
#include "player_utils/player.h"
#include "save_utils/save.h"
#include "tutorial_utils/tutorial_utils.h"
#include "battle_utils/battle.h"
#include "dialog_utils/dialog_utils.h"

#include <unistd.h>
#include <stdio.h>

#define PLAYER_OFFSET 50

// Fonction pour vérifier les collisions en fonction du rectangle de collisions du joueur
int no_obstacle2(enum Direction direction, player *player, world world, int cellSize) {
    SDL_Rect hitbox = player->player_hitbox_rect;

    if (direction == HAUT) {
        if (world.matrice[(hitbox.y - 10) / cellSize][hitbox.x / cellSize] == 1 ||
            world.matrice[(hitbox.y - 10) / cellSize][(hitbox.x + hitbox.w) / cellSize] == 1) {
                printf(" Obstacle en haut!");
                fflush(stdout);
            return 0;
        }
    }

    if (direction == BAS) {
        if (world.matrice[(hitbox.y + hitbox.h + 10) / cellSize][hitbox.x / cellSize] == 1 ||
            world.matrice[(hitbox.y + hitbox.h + 10) / cellSize][(hitbox.x + hitbox.w) / cellSize] == 1) {
                printf(" Obstacle en bas!");
                fflush(stdout);
            return 0;
        }
    }

    if (direction == GAUCHE) {
        if (world.matrice[hitbox.y / cellSize][(hitbox.x - 10) / cellSize] == 1 ||
            world.matrice[(hitbox.y + hitbox.h) / cellSize][(hitbox.x - 10) / cellSize] == 1) {
            printf(" Obstacle a gauche!");
            fflush(stdout);
            return 0;
        }
    }

    if (direction == DROITE) {
        if (world.matrice[hitbox.y / cellSize][(hitbox.x + hitbox.w + 10) / cellSize] == 1 ||
            world.matrice[(hitbox.y + hitbox.h) / cellSize][(hitbox.x + hitbox.w + 10) / cellSize] == 1) {
            printf(" Obstacle a droite!");
            fflush(stdout);
            return 0;
        }
    }
    fflush(stdout);
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

// Fonction pour mettre à jour la position du joueur dans la matrice
void updatePlayerPositionInMatrix2(world world, player *player, int cellSize) {
    int centerX = player->player_rect.x + player->player_rect.w / 2;
    int centerY = player->player_rect.y + player->player_rect.h / 2;
    player->MposX = centerX/cellSize;
    player->MposY = centerY/cellSize;
    printf("\rPOS: %d:%d", player->MposX, player->MposY);
    fflush(stdout);
}

// Fonction pour gérer le mouvement du joueur retourne 0 si aucun événement n'est détecté, 1 si le joueur passe au niveau suivant, -1 si le joueur passe au niveau précédent et 2 si un combat est détecté
int mouvement2(const Uint8 *state, world world, player *player, int *battles_done, int *dialogs_done) {
    int oldMposX = player->MposX;
    int oldMposY = player->MposY;

    if(state[SDL_SCANCODE_W] && no_obstacle2(HAUT, player, world, cellSize)){ //Si appuie sur W et qu'il n'y a pas d'obstacle
        // add_log("MOUVEMENT","Z pressee\n");
        player->player_rect.y -= 10;
        player->player_hitbox_rect.y -= 10;

        player->player_texture = player->img_dir_joueur[HAUT];
    }

    if(state[SDL_SCANCODE_A] && no_obstacle2(GAUCHE, player, world, cellSize)){ //Si appuie sur A et qu'il n'y a pas d'obstacle
        // add_log("MOUVEMENT","Q pressee\n");
        player->player_rect.x -= 10;
        player->player_hitbox_rect.x -= 10;

        player->player_texture = player->img_dir_joueur[GAUCHE];
    }

    if(state[SDL_SCANCODE_S] && no_obstacle2(BAS, player, world, cellSize)){ //Si appuie sur S et qu'il n'y a pas d'obstacle
        // add_log("MOUVEMENT","S pressee\n");
        player->player_rect.y += 10;
        player->player_hitbox_rect.y += 10;

        player->player_texture = player->img_dir_joueur[BAS];
    }

    if(state[SDL_SCANCODE_D] && no_obstacle2(DROITE, player, world, cellSize)){ //Si appuie sur D et qu'il n'y a pas d'obstacle
        // add_log("MOUVEMENT","D pressee\n");
        player->player_rect.x += 10;
        player->player_hitbox_rect.x += 10;

        player->player_texture = player->img_dir_joueur[DROITE];
    }
    
    updatePlayerPositionInMatrix2(world, player, cellSize); //Met à jour la position du joueur dans la matrice

    for(int i = 0; i < world.nb_battles; i++){ //Pour chaque combat dans le monde
        if(player->MposX == world.battles[i].battle_coords.x && player->MposY == world.battles[i].battle_coords.y){ //Si le joueur est sur un trigger de combat
            printf("Id battle: %d, indice tab: %d\n", world.battles[i].id, battles_done[world.battles[i].id]);
            if(is_battle_not_done(world.battles[i].id, battles_done)){ //Si le combat n'a pas encore été fait
                add_log_info("jeu.c - mouvement2()", "Combat non fait");
                return world.battles[i].id+1;
            }
            else{ //Si le combat n'a pas encore été fait
                add_log_info("jeu.c - mouvement2()", "Combat déjà fait");
                return 0;
            }
        }
    }

    for(int i = 0; i < world.nb_dialogs; i++){ //Pour chaque dialogue dans le monde
        if(player->MposX == world.dialogs[i].dialog_coords.x && player->MposY == world.dialogs[i].dialog_coords.y){ //Si le joueur est sur un trigger de dialogue
            printf("Id dialog: %d\n", world.dialogs[i].id);
            if(is_dialog_not_done(world.dialogs[i].id, dialogs_done)){ //Si le dialogue n'a pas encore été fait
                add_log_info("jeu.c - mouvement2()", "Dialogue non fait");
            return world.dialogs[i].id+200;
            }
            else{ //Si le dialogue a déjà été fait
                add_log_info("jeu.c - mouvement2()", "Dialogue déjà fait");
                return 0;
            }
        }
    }

    if(player->MposX == world.next_portal.x && player->MposY == world.next_portal.y){ //Indication de passage au prochain niveau
        add_log("jeu.c - mouvement2()", "Passage au niveau suivant");
        return 1;
    }
    if(player->MposX == world.back_portal.x && player->MposY == world.back_portal.y && strcmp(world.previous_world, "null") != 0){ //Indication de passage au niveau précédant
        add_log("jeu.c - mouvement2()", "Passage au niveau précédant");
        return -1;
    }
    else{
        return 0;
    }

    return 0;
}

//Créér une texture pour les murs et la retourne
SDL_Texture* get_world_texture(SDL_Window *window, world world){

    // On récupère le renderer de la fenêtre
    SDL_Renderer *renderer = SDL_GetRenderer(window);

    //Texture du mur
    SDL_Texture *mur_texture = loadTexture(world.wall_texture_path, renderer);

    SDL_Texture *floor_texture = loadTexture(world.floor_texture_path, renderer);
    if(floor_texture == NULL){
        add_log_error("jeu.c - get_world_texture()", "Erreur de chargement de la texture du sol");
        return NULL;
    }

    //On récupère la taille de la fenêtre pour bien placer les boutons par la suite
    int width, height;
    SDL_DisplayMode taille_fenetre;
    SDL_GetCurrentDisplayMode(0, &taille_fenetre);
    width = taille_fenetre.w;
    height = taille_fenetre.h;

    // Création de la texture du monde
    SDL_Texture *world_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetRenderTarget(renderer, world_texture);

    // Rectangle pour les murs
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
            else{
                MurRect.x = j * MurRect.w;
                MurRect.y = i * MurRect.h;
                SDL_RenderCopy(renderer, floor_texture, NULL, &MurRect);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);

    world.global_texture = world_texture;

    return world_texture;
}

//Lance la partie à partir du monde donné
void jeu(SDL_Window *window, SDL_Renderer *renderer, world *actual_world, char *current_music_path, int *battles_done, int *dialogs_done){
    
    SDL_ShowCursor(SDL_DISABLE); //Cache le curseur
    
    add_log_info("jeu.c - jeu()", "Lancement de la partie");



    int jeu = 1; //Variable permettant revenir au menu principal si = 0
    
    int tutorial = 0;
    int Z_pressed = 0;
    int Q_pressed = 0;
    int S_pressed = 0;
    int D_pressed = 0;

    while(jeu != 0 && actual_world != NULL){
        print_world_info(actual_world);
    
        SDL_RenderClear(renderer);
        initWindowSize();
    
        Mix_Music *music;
        if(strcmp(current_music_path, actual_world->music_path) != 0){ //Joue la musique du monde uniquement si elle est différente de l'ancienne
            Mix_FadeOutMusic(1000);
            music =  Mix_LoadMUS(actual_world->music_path);
            Mix_FadeInMusic(music, -1, 2000);
            printf("last_music: %s size: %d\n", current_music_path, sizeof(current_music_path));
            printf("current_music: %s size: %d\n", actual_world->music_path, sizeof(actual_world->music_path));
            free(current_music_path);
            current_music_path = (char *)malloc(strlen(actual_world->music_path)+1);
            
            if(!current_music_path){
                add_log_error("jeu.c - jeu()","Erreur de realloc");
                return;
            }
    
            strcpy(current_music_path, actual_world->music_path);
        }
        
        actual_world->global_texture = get_world_texture(window, *actual_world);
    
        player player = init_player(renderer, *actual_world);
        if(player.player_texture == NULL){
            add_log_error("jeu.c - jeu()","Erreur d'initialisation du joueur");
            return;
        }
    
        player.img_dir_joueur[BAS]=loadTexture("res/joueur/joueurB.png", renderer);
        if(player.img_dir_joueur[BAS] == NULL){
            add_log_error("jeu.c - jeu()","Erreur de chargement de l'image du joueur");
            return;
        }
    
        int continuer = 0;
    
        SDL_Event event; //Variable qui reçoit l'évènement
        const Uint8 *state = SDL_GetKeyboardState(NULL); //Récupère l'état du clavier
        Uint32 lastTime = SDL_GetTicks(), currentTime; //Variables pour le temps
        
        SDL_Rect player_hitboxRect;
        int centerX;
        int centerY;

        //Partie tutorial
        if(exist_save() == 0 && tutorial == 0){
            add_log_info("jeu.c - jeu()", "Début du tutoriel");
            tutorial = 1;
            Z_pressed = 0;
            Q_pressed = 0;
            S_pressed = 0;
            D_pressed = 0;
        }

        
        while(continuer == 0){

            continuer = mouvement2(state, *actual_world, &player, battles_done, dialogs_done); //Déplace le joueur
            SDL_RenderClear(renderer); //Efface l'écran
            SDL_RenderCopy(renderer, actual_world->global_texture, NULL, NULL); //Rend la texture des murs
            SDL_RenderCopy(renderer, player.player_texture, NULL, &player.player_rect); //Rend le joueur car sa position à changé
            
            if(player_hitbox){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_Rect player_hitbox_rect_temp = player.player_hitbox_rect;
                player_hitbox_rect_temp.x -= 5;
                player_hitbox_rect_temp.y -= 5;
                player_hitbox_rect_temp.w += 15;
                player_hitbox_rect_temp.h += 15;
                SDL_RenderDrawRect(renderer, &player_hitbox_rect_temp);
            }

            //Partie tutorial
            if(tutorial && (!Z_pressed || !Q_pressed || !S_pressed || !D_pressed)){
                display_controls(renderer, width/2, height/2, Z_pressed, Q_pressed, S_pressed, D_pressed);
                if(state[SDL_SCANCODE_W]){
                    Z_pressed = 1;
                }
                if(state[SDL_SCANCODE_A]){
                    Q_pressed = 1;
                }
                if(state[SDL_SCANCODE_S]){
                    S_pressed = 1;
                }
                if(state[SDL_SCANCODE_D]){
                    D_pressed = 1;
                }
            }
    
            SDL_RenderPresent(renderer); //Présente le rendu

            
            SDL_Delay(16); //Délai pour éviter de bouger trop vite

    
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    continuer = -2;
                    jeu = 0;
                }
                else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
                    continuer = pause(renderer);
                    jeu = continuer+2;
                }
            }

            if(continuer >= 200){ //Affichage d'un dialogue
                display_dialogs(renderer, continuer-200);
                dialogs_done[continuer-201] = 1;
                for(int i = 0; i < 20; i++){
                    printf("%d ", dialogs_done[i]);
                }
                continuer = 0;
            }
        }
        
        //Libération de ressources de la fonction jeu
        free_player(player);
        SDL_RenderClear(renderer); //Efface l'écran
    
        if(continuer == -1){ //Retour au monde précédent
            add_log_info("jeu.c - jeu()", "Retour au monde précédent");
    
            char *previous_world = (char*)malloc(strlen(actual_world->previous_world) + 1);
            if (previous_world == NULL) {
                add_log_error("jeu.c - jeu()", "Erreur de malloc pour previous_world");
                return;
            }
            strcpy(previous_world, actual_world->previous_world);
    
            free_world(actual_world);
            actual_world = get_world_info(previous_world);
            if(actual_world == NULL){
                add_log_error("jeu.c - jeu()", "Erreur de récupération des informations du monde précédent");
                return;
            }
            actual_world->start_spawn = actual_world->end_spawn;
        }
    
        if(continuer == 1){ //Aller au monde suivant
            add_log_info("jeu.c - jeu()", "Passage au monde suivant");
    
            char *next_world = (char*)malloc(strlen(actual_world->next_world) + 1);
            if (next_world == NULL) {
                add_log_error("jeu.c - jeu()", "Erreur de malloc pour next_world");
                return;
            }
            strcpy(next_world, actual_world->next_world);
    
            free_world(actual_world);
            actual_world = get_world_info(next_world);
            if(actual_world == NULL){
                add_log_error("jeu.c - jeu()", "Erreur de récupération des informations du monde suivant");
            }
        }

        if(continuer > 1 && continuer < 200){ //Lancement d'un combat
            add_log_info("jeu.c - jeu()", "Lancement d'un combat");

            double music_pos = Mix_GetMusicPosition(music);
            Mix_FadeOutMusic(500);
            start_battle(renderer, continuer-1, battles_done);
            Mix_FadeInMusicPos(music, -1, 1000, music_pos+1);
            continuer = 0;
        }
    }

    //Libération de la mémoire
    add_log_info("jeu.c - jeu()", "Libération de la mémoire du monde");
    save_progress(actual_world->actual_world, battles_done, dialogs_done);
    free_world(actual_world);
    free(current_music_path);
    add_log_info("jeu.c - jeu()", "Fin de la partie");
    return;
}