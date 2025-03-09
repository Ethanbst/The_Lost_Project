#include <stdio.h>
#include <stdlib.h>
#include <sdl.h>
#include "../logs_utils/log.h"
#include "../pause_menu/pause_menu.h"
#include <time.h>

#define PLAYER_SIZE 20
#define ENEMY_SIZE 20
#define LOOT_SIZE 20
#define STEP_SIZE 10
#define ANIMATION_DELAY 2
#define MOVEMENT_SPEED 5
#define MAX_HEALTH 100

// Initialise les ennemis avec des positions et des vitesses aléatoires
void initialize_enemies(SDL_Rect *enemies, int *enemy_speeds, SDL_Rect boundary, int num_enemies) {
    for (int i = 0; i < num_enemies; i++) {
        int size = rand() % 20 + 10;
        enemies[i].x = rand() % (boundary.w - size) + boundary.x;
        enemies[i].y = rand() % (boundary.h - size) + boundary.y;
        enemies[i].w = size;
        enemies[i].h = size;
        enemy_speeds[i] = (rand() % 10) + 1;
    }
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

// Met à jour les positions des ennemis et vérifie les collisions avec le joueur
int update_enemies(SDL_Renderer *renderer, SDL_Rect *enemies, int *enemy_speeds, SDL_Rect boundary, SDL_Rect player, int num_enemies, int *health) {
    for (int i = 0; i < num_enemies; i++) {
        enemies[i].y += enemy_speeds[i];
        if (enemies[i].y > boundary.y + boundary.h) {
            enemies[i].y = boundary.y;
            enemies[i].x = rand() % (boundary.w - ENEMY_SIZE) + boundary.x;
        }
        SDL_RenderFillRect(renderer, &enemies[i]);
        if (SDL_HasIntersection(&player, &enemies[i])) {
            add_log_info("battle.c - start_battle()", "Le joueur a touché un ennemi");
            *health -= 2;
            if (*health <= 0) {
                return -2;
            }
        }
    }
    return 0;
}

// Met à jour les positions des loots et vérifie les collisions avec le joueur
void update_loots(SDL_Renderer *renderer, SDL_Rect *loots, SDL_Rect boundary, SDL_Rect player, int *score, int num_loots) {
    for (int i = 0; i < num_loots; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &loots[i]);
        if (SDL_HasIntersection(&player, &loots[i])) {
            add_log_info("battle.c - start_battle()", "Le joueur a collecté un loot");
            *score += 10;
            loots[i].x = rand() % (boundary.w - LOOT_SIZE) + boundary.x;
            loots[i].y = rand() % (boundary.h - LOOT_SIZE) + boundary.y;
        }
    }
}

// Affiche la barre de vie du joueur
void render_health_bar(SDL_Renderer *renderer, int health) {
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    SDL_Rect health_bar = {displayMode.w/2-100, displayMode.h/2-10, health*2, 20}; // La largeur de la barre de vie est proportionnelle à la santé
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &health_bar);
}

// Démarre la bataille, initialise les éléments et gère la boucle principale du combat
int start_battle(SDL_Renderer *renderer, int battle_id, int num_enemies, int num_loots) {
    add_log_info("battle.c - start_battle()", "Lancement du combat");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    srand(time(NULL));

    int window_width, window_height;
    SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

    SDL_Rect boundary = {window_width/2, window_height/ 2, 0, 0};
    SDL_Rect target_boundary = {window_width/2, window_height/2, window_width/3, window_height/3};

    animate_boundary(renderer, &boundary, target_boundary);

    SDL_Rect player = {boundary.x + 100, boundary.y + 100, PLAYER_SIZE, PLAYER_SIZE};
    SDL_Rect enemies[num_enemies];
    int enemy_speeds[num_enemies];
    initialize_enemies(enemies, enemy_speeds, boundary, num_enemies);

    SDL_Rect loots[num_loots];
    initialize_loots(loots, boundary, num_loots);

    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    int score = 0;
    int health = MAX_HEALTH;

    int battle = 0;

    while (battle == 0) {
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
        SDL_RenderFillRect(renderer, &player);

        battle = update_enemies(renderer, enemies, enemy_speeds, boundary, player, num_enemies, &health);
        update_loots(renderer, loots, boundary, player, &score, num_loots);

        render_health_bar(renderer, health);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return -2;
}
