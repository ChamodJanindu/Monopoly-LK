#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "game.h"

void handleLanding(Player *player, Property board[]);
void handleLandingTest(int testPostions[], int numTests, Player *player, Property board[]);

void testTurnRotation(Player players[], int numPlayers, Property board[], int numRounds);

#endif