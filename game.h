#ifndef GAME_H
#define GAME_H

#include "types.h"

void determineTurnOrder(Player players[], int turnOrder[]);

void handleLanding(Player players[], int playerIndex, Property board[], int diceTotal, int turnOrder[], GameState *game);
void playTurn(Player players[], int playerIndex, Property board[], int turnOrder[], GameState *game);
int handleJailTurn(Player players[], int playerIndex, Property board[], int turnOrder[], GameState *game);
void testTurnRotation(Player players[], int numPlayers, Property board[], int targetCompletedRounds, int turnOrder[], GameState *game);

void handlePropertyPurchase(Player players[], int playerIndex, Property property[], int squareIndex, int turnOrder[]);
void initGameState(GameState *game);
int assignPropertyToPlayer(Player players[], int playerIndex, Property board[], int squareIndex);

int calculateCompletedRounds(Player players[], int numPlayers, int currentCompletedRounds);
void updateCompletedRounds(GameState *game, Player players[], int numPlayers, Property board[]);

void handleAuction(Player players[], Property board[], int squareIndex, int turnOrder[]);

void sendPlayerToJail(Player *player);

int ownsCompleteGroup(Property board[], int playerIndex, PropertyGroup group);
int canBuildHouse(Property board[], int playerIndex, int squareIndex);
int buildHouse(Player players[], int playerIndex, Property board[], int squareIndex);
int canBuildHotel(Property board[], int playerIndex, int squareIndex);
int buildHotel(Player players[], int playerIndex, Property board[], int squareIndex);
void handleConstructionPhase(Player players[], int playerIndex, Property board[]);

void handleMaintenancePhase(Player players[], int playerIndex, Property board[]);


#endif