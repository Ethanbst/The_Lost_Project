#include <stdio.h>
#include <stdlib.h>
#include "../logs_utils/log.h"
#include "../pause_menu/pause_menu.h"
#include <time.h>
#include "../cta_utils/cta.h"
#include "../mouse_utils/mouse.h"
#include <C:/cJSON-master/cJSON.h>
#include <SDL_mixer.h>
#include <SDL.h>
#include "../debug_var.h"

#define PLAYER_SIZE 40
#define LOOT_SIZE 20
#define STEP_SIZE 10
#define ANIMATION_DELAY 2
#define MOVEMENT_SPEED 10
#define MAX_HEALTH 100



typedef struct battle_info {
    int num_enemies;
    int num_loots;
    int e_min_speed;
    int e_max_speed;
    char *music_path;
} battle_info;

typedef struct enemy {
    SDL_Rect rect;
    int speed;
    char label[10];
    int size; //Taille de la police d'écriture
} enemy;

const char *compilation_errors[] = {
    "Segmentation",
    "fault",
    "Undefined",
    "Implicit",
    "Conflicting",
    "Unused",
    "Expected",
    "Redeclaration",
    "Incompatible",
    "Invalid",
    "Malloc",
    "failed",
    "Array",
    "Char*",
    "zero",
    "bounds",
    "out",
    "error",
};


// Initialise les ennemis avec des positions et des vitesses aléatoires
void initialize_enemies(enemy *enemies, SDL_Rect boundary, int num_enemies, int max_speed, int min_speed, SDL_Renderer *renderer) {
    for (int i = 0; i < num_enemies; i++) {
        
        int error_index = rand() % (sizeof(compilation_errors) / sizeof(compilation_errors[0]));
        sprintf(enemies[i].label, "%s", compilation_errors[error_index]);

        int size;
        do {
            size = (rand() % 11) + 30 - strlen(enemies[i].label) / 2; // Réduit la taille plus la chaîne est longue
        } while (size % 2 != 0);
        enemies[i].size = size;
        
        CTA a = get_button(renderer, enemies[i].label, enemies[i].size);

        enemies[i].rect.w = a.w;
        enemies[i].rect.h = a.h;
        enemies[i].rect.x = rand() % (boundary.w - enemies[i].rect.w) + boundary.x;
        enemies[i].rect.y = rand() % 100 + boundary.y - 100;
        enemies[i].speed = (rand() % max_speed) + min_speed;
    }
}

// Met à jour les positions des ennemis et vérifie les collisions avec le joueur renvoie -2 si partie perdu = plus de vie
int update_enemies(SDL_Renderer *renderer, enemy *enemies, SDL_Rect boundary, SDL_Rect player, int num_enemies, int *health) {
    SDL_Rect enemy_rects[num_enemies];
    for (int i = 0; i < num_enemies; i++) {
        enemies[i].rect.y += enemies[i].speed;
        if (enemies[i].rect.y > boundary.y + boundary.h) { // Réinitialiser la position de l'ennemi lorsqu'il sort de l'écran
            enemies[i].rect.y = boundary.y;
            enemies[i].rect.x = rand() % (boundary.w - enemies[i].rect.w) + boundary.x;
        }
        enemy_rects[i] = enemies[i].rect;
    }

    for (int i = 0; i < num_enemies; i++) {
        color red = {255, 0, 0, 255};
        CTA a = draw_button(renderer, enemy_rects[i].x + enemy_rects[i].w / 2, enemy_rects[i].y + enemy_rects[i].h / 2, 1, enemies[i].label, 0, enemies[i].size, red);
        if (enemy_hitbox) { // Rendu de la boite de collision des ennemis
            SDL_RenderDrawRect(renderer, &enemy_rects[i]);
        }
        if (SDL_HasIntersection(&player, &enemy_rects[i])) {
            add_log_info("battle.c - start_battle()", "Le joueur a touché un ennemi");
            *health -= 2;
            if (*health <= 0) {
                return -2;
            }
        }
    }
    return 0;
}

// Initialise les loots avec des positions aléatoires
void initialize_loots(SDL_Rect *loots, SDL_Rect boundary, int num_loots) {
    for (int i = 0; i < num_loots; i++) {
        loots[i].x = rand() % (boundary.w - LOOT_SIZE) + boundary.x;
        loots[i].y = rand() % (boundary.h - LOOT_SIZE) + boundary.y;
        loots[i].w = LOOT_SIZE;
        loots[i].h = LOOT_SIZE;
    }
}

// Anime la frontière du jeu jusqu'à atteindre la taille cible
void animate_boundary(SDL_Renderer *renderer, SDL_Rect *boundary, SDL_Rect target_boundary) {
    while (boundary->w < target_boundary.w || boundary->h < target_boundary.h) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (boundary->w < target_boundary.w) {
            boundary->x -= STEP_SIZE / 2;
            boundary->w += STEP_SIZE;
            if (boundary->w > target_boundary.w) {
                boundary->x = target_boundary.x - (target_boundary.w - boundary->w) / 2;
                boundary->w = target_boundary.w;
            }
        }
        if (boundary->h < target_boundary.h) {
            boundary->y -= STEP_SIZE / 2;
            boundary->h += STEP_SIZE;
            if (boundary->h > target_boundary.h) {
                boundary->y = target_boundary.y - (target_boundary.h - boundary->h) / 2;
                boundary->h = target_boundary.h;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, boundary);
        SDL_RenderPresent(renderer);
        SDL_Delay(ANIMATION_DELAY);
    }
}

// Gère le mouvement du joueur en fonction des touches pressées
void handle_player_movement(const Uint8 *keystates, SDL_Rect *player, SDL_Rect boundary) {
    if (keystates[SDL_SCANCODE_W] && player->y > boundary.y) player->y -= MOVEMENT_SPEED;
    if (keystates[SDL_SCANCODE_S] && player->y + player->h < boundary.y + boundary.h) player->y += MOVEMENT_SPEED;
    if (keystates[SDL_SCANCODE_A] && player->x > boundary.x) player->x -= MOVEMENT_SPEED;
    if (keystates[SDL_SCANCODE_D] && player->x + player->w < boundary.x + boundary.w) player->x += MOVEMENT_SPEED;
}

// Met à jour les positions des loots et vérifie les collisions avec le joueur
void update_loots(SDL_Renderer *renderer, SDL_Rect *loots, SDL_Rect boundary, SDL_Rect player, int *score, int num_loots, int *health) {
    for (int i = 0; i < num_loots; i++) {
        color green = {0, 255, 0, 255};
        draw_button(renderer, loots[i].x, loots[i].y, 1, "0", 0, 42, green);
        if (SDL_HasIntersection(&player, &loots[i])) {
            add_log_info("battle.c - start_battle()", "Le joueur a collecté un loot");
            *score += 1;
            loots[i].x = rand() % (boundary.w - LOOT_SIZE) + boundary.x;
            loots[i].y = rand() % (boundary.h - LOOT_SIZE) + boundary.y;
            *health += 10;
            if (*health > MAX_HEALTH) {
                *health = MAX_HEALTH;
            }
        }
    }
}

// Affiche la barre de vie du joueur
void render_health_bar(SDL_Renderer *renderer, int health, SDL_Rect boundary) {
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    SDL_Rect health_bar = {boundary.x, boundary.y+boundary.h+20, health*4, 40}; // La largeur de la barre de vie est proportionnelle à la santé
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &health_bar);
}

void render_score(SDL_Renderer *renderer, SDL_Rect *loots, int num_loots, SDL_Rect boundary, int *score) {
    color couleur = {0, 255, 0, 255};
    CTA a = draw_button(renderer, boundary.x, boundary.y-30, 1, "Bits: ", 0, 42, couleur);

    char *loot_obt = (char*)malloc(sizeof(char));
    sprintf(loot_obt, "%d", *score);
    CTA b = draw_button(renderer, a.pos_x+a.w+20, a.pos_y+a.h/2, 1, loot_obt, 0, 42, couleur);

    CTA c = draw_button(renderer, b.pos_x+b.w+10, a.pos_y+a.h/2, 1, "/", 0, 42, couleur);
    
    char *total_loots = (char*)malloc(sizeof(char));
    sprintf(total_loots, "%d", num_loots);
    draw_button(renderer, c.pos_x+c.w+10, a.pos_y+a.h/2, 1, total_loots, 0, 42, couleur);
}

void display_loose_screen(SDL_Renderer *renderer, int window_width, int window_height) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const char *lose_messages[] = {
        "Vous avez echoue.",
        "Defaite! Essayez encore.",
        "Vous avez perdu la bataille.",
        "La partie est terminee.",
        "Vous n'avez pas survecu.",
        "Game Over.",
        "Vous avez ete vaincu.",
        "Votre aventure s'arrête ici.",
        "Vous avez succombe à vos blessures.",
        "La defaite est amere, réessayez."
    };
    int message_index = rand() % 10;

    color white = {255, 255, 255, 255};
    CTA message = draw_button(renderer, window_width / 2, window_height / 2 - 100, 3, lose_messages[message_index], 0, 42, white);
    CTA button = draw_button(renderer, window_width / 2, window_height / 2 + 100, 2, "Je suis determine !", 0, 42, white);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while(1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (is_mouse_on(button)) {
                    return;
                }
            }
            else{
                if(is_mouse_on(button)){
                    set_hand_cursor();
                }
                else{
                    reset_cursor();
                }
            }
        }
    }
}

// Lit les paramètres depuis un fichier JSON et les stocke dans une structure battle_info
battle_info* get_battle_info(int id) {

    battle_info *info = (battle_info *)malloc(sizeof(battle_info));
    char filename[20];
    sprintf(filename, "res/battle/%d.json", id);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier JSON");
        return info;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    if (!data) {
        perror("Erreur lors de l'allocation de mémoire");
        fclose(file);
        return info;
    }

    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON *json = cJSON_Parse(data);
    if (!json) {
        perror("Erreur lors de l'analyse du fichier JSON");
        free(data);
        return info;
    }

    cJSON *enemy_speed = cJSON_GetObjectItem(json, "ennemy_speed");
    if (enemy_speed) {
        info->e_max_speed = cJSON_GetObjectItem(enemy_speed, "max")->valueint;
        info->e_min_speed = cJSON_GetObjectItem(enemy_speed, "min")->valueint;
    }

    info->num_enemies = cJSON_GetObjectItem(json, "ennemy_number")->valueint;
    info->num_loots = cJSON_GetObjectItem(json, "loot_number")->valueint;

    cJSON *music_path = cJSON_GetObjectItem(json, "music");
    if (music_path) {
        info->music_path = (char *)malloc(strlen(music_path->valuestring) + 1);
        strcpy(info->music_path, music_path->valuestring);
    }

    cJSON_Delete(json);
    free(data);
    return info;
}

void display_battle_info(battle_info *info) {
    printf("Nombre d'ennemis: %d\n", info->num_enemies);
    printf("Nombre de loots: %d\n", info->num_loots);
    printf("Vitesse minimale des ennemis: %d\n", info->e_min_speed);
    printf("Vitesse maximale des ennemis: %d\n", info->e_max_speed);
    printf("Chemin de la musique: %s\n", info->music_path);
}

// Démarre la bataille d'id id, initialise les éléments et gère la boucle principale du combat
int start_battle(SDL_Renderer *renderer, int id) {

    add_log_info("battle.c - start_battle()", "Lancement du combat");

    battle_info *info = get_battle_info(id);
    display_battle_info(info);

    
    Mix_Music *music = Mix_LoadMUS(info->music_path);
    if (!music) {
        add_log_error("battle.c - start_battle()", "Erreur lors du chargement de la musique de combat");
        return -1;
    }
    Mix_FadeInMusic(music, 0, 1000);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    srand(time(NULL));

    int window_width, window_height;
    SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    SDL_Event event;

    int continuer = 0;

    while(continuer == 0){ //Boucle pour démarrage du combat (ex: après mort)
        
        SDL_Rect boundary = {window_width/2, window_height/ 2, 0, 0};
        SDL_Rect target_boundary = {window_width/2, window_height/2, window_width/3, window_height/3};
    
        animate_boundary(renderer, &boundary, target_boundary);
    
        SDL_Rect player = {boundary.x+boundary.w/2-PLAYER_SIZE/2, boundary.y+boundary.h/2-PLAYER_SIZE/2, PLAYER_SIZE, PLAYER_SIZE};
        
        enemy enemies[info->num_enemies];
        //int enemy_speeds[info->num_enemies];
        initialize_enemies(enemies, boundary, info->num_enemies, info->e_max_speed, info->e_min_speed, renderer);
    
        SDL_Rect loots[1];
        initialize_loots(loots, boundary, 1);

        int score = 0;
        int health = MAX_HEALTH;


        // Dessiner le joueur pendant 2 secondes avant de passer au combat
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &player);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000);


        int battle = 0;

        int minutes = 10;
        int seconds = 5;
        int total_seconds = minutes * 60 + seconds;
        Uint32 start_time = SDL_GetTicks();

        while (battle == 0 && total_seconds > 0) { //Boucle de combat
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) return -2;
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    int battle = pause(renderer);
                    if (battle == -2) return -2;
                }
            }
    
            handle_player_movement(keystates, &player, boundary);
    
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
    
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &boundary);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &player);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
            battle = update_enemies(renderer, enemies, boundary, player, info->num_enemies, &health);
            update_loots(renderer, loots, boundary, player, &score, 1, &health);
    
            render_health_bar(renderer, health, boundary);
            render_score(renderer, loots, info->num_loots, boundary, &score);
            if(score >= info->num_loots){
                add_log_info("battle.c - start_battle()", "Tous les loots ont été collectés");
                battle = 1;
            }
        
            //Timer
            if (total_seconds > 0) {
                Uint32 current_time = SDL_GetTicks();
                Uint32 elapsed_time = (current_time - start_time) / 1000;
                total_seconds = minutes * 60 + seconds - elapsed_time;

                int display_minutes = total_seconds / 60;
                int display_seconds = total_seconds % 60;

                char time_string[10];
                sprintf(time_string, "%02d:%02d", display_minutes, display_seconds);

                color white = {255, 255, 255, 255};
                draw_button(renderer, boundary.x, boundary.y - 100, 1, time_string, 0, 42, white);
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        if(battle == -2 || total_seconds <= 0 && score < info->num_loots){ //partie perdu
            add_log_info("battle.c - start_battle()", "Partie perdu");
            display_loose_screen(renderer, window_width, window_height);
            health = MAX_HEALTH;
            score = 0;
        }
        else if(battle == 1){ //partie gagné
            add_log_info("battle.c - start_battle()", "Partie gagné");
            continuer = 1;
        }
    }
    return -2;
}
