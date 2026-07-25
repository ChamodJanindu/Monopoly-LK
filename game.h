#ifndef GAME_H
#define GAME_H

#include "types.h"

void handleLanding(Player *player, Property board[]);
void handleLandingTest(int testPostions[], int numTests, Player *player, Property board[]);

void testing001(Player *player, Property board[]);

#endif