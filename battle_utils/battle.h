#ifndef BATTLE_H
#define BATTLE_H

#define NB_BATTLES 20
#include <SDL.h>
#include "../worlds/worlds_utils.h"
int start_battle(SDL_Renderer *renderer, int id, int *battles_done);
bool is_battle_not_done(int id, int *battles_done);

#endif