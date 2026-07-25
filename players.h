#ifndef PLAYERS_H
#define PLAYERS_H 

#include "types.h"

void initPlayer(Player *player, const char *name, PlayerStrategy strategy);
void movePlayer(Player *player, int diceTotal);

#endif