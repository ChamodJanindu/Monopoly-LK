#ifndef GAME_H
#define GAME_H

#include "types.h"

void determineTurnOrder(Player players[], int turnOrder[]);
void handleLanding(Player players[], int playerIndex, Property board[], int diceTotal, int turnOrder[]);
void handleLandingTest(Player players[],int testPostions[], int numTests, Player *player, Property board[], int turnOrder[]);
void testTurnRotation(Player players[], int numPlayers, Property board[], int targetCompletedRounds, int turnOrder[], GameState *game);
void handlePropertyPurchase(Player players[], int playerIndex, Property property[], int squareIndex, int turnOrder[]);
void initGameState(GameState *game);
int assignPropertyToPlayer(Player players[], int playerIndex, Property board[], int squareIndex);

int calculateCompletedRounds(Player players[], int numPlayers, int currentCompletedRounds);
void updateCompletedRounds(GameState *game, Player players[], int numPlayers);

void handleAuction(Player players[], Property board[], int squareIndex, int turnOrder[]);

void sendPlayerToJail(Player *player);
void handleJailTurn(Player players[], int playerIndex, Property board[], int turnOrder[]);

#endif