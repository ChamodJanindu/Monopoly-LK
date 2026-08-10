#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

int main(void){

    Property board[BOARD_SIZE];
    Player players[NUM_PLAYERS];
    GameState game;

    int turnOrder[NUM_PLAYERS] = {0, 1, 2, 3};

    srand(time(NULL));

    initBoard(board);

    initPlayer(&players[0], "Aggressive", STRATEGY_AGGRESSIVE);
    initPlayer(&players[1], "Conservative", STRATEGY_CONSERVATIVE);
    initPlayer(&players[2], "Risk Taker", STRATEGY_RISK_TAKER);
    initPlayer(&players[3], "Opportunistic", STRATEGY_OPPORTUNISTIC);

    initGameState(&game);

    printf("=============================================\n");
    printf("FULL TURN ROTATION TEST\n");
    printf("=============================================\n");

    printf("\nStarting positions:\n");

    for(int i = 0; i < NUM_PLAYERS; i++){

        printf("%s: Position %d, Cash %d\n",
               players[i].name,
               players[i].position,
               players[i].cash);
    }

    printf("\n=============================================\n");

    testTurnRotation(players,
                     NUM_PLAYERS,
                     board,
                     2,
                     turnOrder,
                     &game);

    printf("\n=============================================\n");
    printf("FINAL STATE\n");
    printf("=============================================\n");

    printf("Completed rounds: %d\n",
           game.completedRounds);

    for(int i = 0; i < NUM_PLAYERS; i++){

        printf("\n%s\n", players[i].name);

        printf("Position: %d\n",
               players[i].position);

        printf("Lap count: %d\n",
               players[i].lapCount);

        printf("Cash: %d\n",
               players[i].cash);

        printf("Properties: %d\n",
               players[i].numOwnedProperties);

        printf("In Jail: %d\n",
               players[i].isInJail);

        printf("Bankrupt: %d\n",
               players[i].isBankrupt);
    }

    printf("\n=============================================\n");
    printf("TURN ROTATION TEST FINISHED\n");
    printf("=============================================\n");

    return 0;
}