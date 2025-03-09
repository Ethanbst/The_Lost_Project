#include <stdio.h>
#include <stdlib.h>
#include <sdl.h>
#include "../logs_utils/log.h"
#include "../pause_menu/pause_menu.h"
#include <time.h>

int start_battle(SDL_Renderer *renderer, int battle_id)
{
    add_log_info("battle.c - start_battle()", "Lancement du combat");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int battle = 0;
    int score = 0;

    SDL_Event event;
    // Initialize random seed
    srand(time(NULL));

    // Get window size
    int window_width, window_height;
    SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

    // Define boundary properties
    SDL_Rect boundary = {
        (window_width - 700) / 2, // Center horizontally
        (window_height - 500) / 2, // Center vertically
        700,
        500
    };

    // Define player properties
    SDL_Rect player = {boundary.x + 100, boundary.y + 100, 20, 20}; // Example position within boundary

    // Define enemy properties
    int num_enemies = 5;
    SDL_Rect enemies[num_enemies];
    int enemy_speeds[num_enemies];

    // Initialize enemies with random positions and speeds within boundary
    for (int i = 0; i < num_enemies; i++)
    {
        enemies[i].x = rand() % (boundary.w - 20) + boundary.x;
        enemies[i].y = rand() % (boundary.h - 20) + boundary.y;
        enemies[i].w = 20;
        enemies[i].h = 20;
        enemy_speeds[i] = (rand() % 5) + 1; // Random speed between 1 and 5
    }

    // Define loot properties
    int num_loots = 2; // Fewer loots than enemies
    SDL_Rect loots[num_loots];

    // Initialize loots with random positions within boundary
    for (int i = 0; i < num_loots; i++)
    {
        loots[i].x = rand() % (boundary.w - 20) + boundary.x;
        loots[i].y = rand() % (boundary.h - 20) + boundary.y;
        loots[i].w = 20;
        loots[i].h = 20;
    }

    // Array to keep track of key states
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    while (battle == 0)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return -2;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    // Pause menu
                    battle = pause(renderer);
                }
            }
        }

        // Update player position based on key states
        if (keystates[SDL_SCANCODE_Z] && player.y > boundary.y)
        {
            player.y -= 10;
        }
        if (keystates[SDL_SCANCODE_S] && player.y + player.h < boundary.y + boundary.h)
        {
            player.y += 10;
        }
        if (keystates[SDL_SCANCODE_Q] && player.x > boundary.x)
        {
            player.x -= 10;
        }
        if (keystates[SDL_SCANCODE_D] && player.x + player.w < boundary.x + boundary.w)
        {
            player.x += 10;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw white rectangle (boundary)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &boundary);

        // Draw player (white square)
        SDL_RenderFillRect(renderer, &player);

        // Move and draw enemies
        for (int i = 0; i < num_enemies; i++)
        {
            enemies[i].y += enemy_speeds[i];
            if (enemies[i].y > boundary.y + boundary.h) // Reset enemy position if it goes out of bounds
            {
                enemies[i].y = boundary.y;
                enemies[i].x = rand() % (boundary.w - 20) + boundary.x;
            }
            SDL_RenderFillRect(renderer, &enemies[i]);

            // Check for collision
            if (SDL_HasIntersection(&player, &enemies[i]))
            {
                add_log_info("battle.c - start_battle()", "Player hit an enemy");
                return -2; // Player loses
            }
        }

        // Draw and check for loot collection
        for (int i = 0; i < num_loots; i++)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for loot
            SDL_RenderFillRect(renderer, &loots[i]);

            // Check for collision with loot
            if (SDL_HasIntersection(&player, &loots[i]))
            {
                add_log_info("battle.c - start_battle()", "Player collected loot");
                score += 10; // Increase score
                // Move loot to a new random position
                loots[i].x = rand() % (boundary.w - 20) + boundary.x;
                loots[i].y = rand() % (boundary.h - 20) + boundary.y;
            }
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay to control frame rate
        SDL_Delay(16);
    }
    return -2;
}