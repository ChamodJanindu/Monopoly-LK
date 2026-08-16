#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

int main(void){

    Player players[NUM_PLAYERS];
    Property board[BOARD_SIZE];
    GameState game;
    int turnOrder[NUM_PLAYERS];

    unsigned int seed = (unsigned int)time(NULL);

    srand(seed);

    initBoard(board);
    initGameState(&game);

    initPlayer(&players[0], "Aggressive", STRATEGY_AGGRESSIVE);
    initPlayer(&players[1], "Conservative", STRATEGY_CONSERVATIVE);
    initPlayer(&players[2], "Risk Taker", STRATEGY_RISK_TAKER);
    initPlayer(&players[3], "Opportunistic", STRATEGY_OPPORTUNISTIC);

    determineTurnOrder(players, turnOrder);

    int targetRounds = 35;
    int totalTurns = 0;

    printf("\n============================================================\n");
    printf("          PHASE 10 FULL STRATEGY INTEGRATION TEST\n");
    printf("============================================================\n");

    printf("Random seed: %u\n", seed);
    printf("Target global rounds: %d\n", targetRounds);

    while(game.completedRounds < targetRounds){

        for(int i = 0; i < NUM_PLAYERS; i++){

            int playerIndex = turnOrder[i];

            playTurn(players, playerIndex, board, turnOrder, &game);

            totalTurns++;

            updateCompletedRounds(&game, players, NUM_PLAYERS, board);

            if(game.completedRounds >= targetRounds){
                break;
            }
        }
    }

    printf("\n============================================================\n");
    printf("                    SIMULATION FINISHED\n");
    printf("============================================================\n");

    printf("Random seed: %u\n", seed);
    printf("Total player turns: %d\n", totalTurns);
    printf("Completed global rounds: %d\n", game.completedRounds);

    printf("\n============================================================\n");
    printf("FINAL PLAYER STATE\n");
    printf("============================================================\n");

    for(int i = 0; i < NUM_PLAYERS; i++){

        Player *player = &players[i];

        printf("\n%s\n", player->name);
        printf("------------------------------------------------------------\n");

        printf("Cash: LKR %d\n", player->cash);
        printf("Position: %d\n", player->position);
        printf("Personal laps: %d\n", player->lapCount);
        printf("Properties owned: %d\n", player->numOwnedProperties);

        if(player->isInJail == 1){
            printf("In jail: YES\n");
        }
        else{
            printf("In jail: NO\n");
        }

        if(player->isBankrupt == 1){
            printf("Bankrupt: YES\n");
        }
        else{
            printf("Bankrupt: NO\n");
        }

        if(player->loan.isActive == 1){

            printf("Loan active: YES\n");
            printf("Outstanding loan: LKR %d\n", player->loan.amount);
            printf("Loan interest rate: %d%%\n", player->loan.interestRate);
            printf("Loan rounds remaining: %d\n",
                   player->loan.roundsRemaining);
        }
        else{

            printf("Loan active: NO\n");
        }
    }

    printf("\n============================================================\n");
    printf("BOARD OWNERSHIP\n");
    printf("============================================================\n");

    for(int i = 0; i < BOARD_SIZE; i++){

        Property *square = &board[i];

        if(square->type != SQUARE_PROPERTY &&
           square->type != SQUARE_RAILWAY &&
           square->type != SQUARE_UTILITY){

            continue;
        }

        printf("Square %2d | %-35s | ", i, square->name);

        if(square->owner == -1){

            printf("Owner: BANK\n");
        }
        else{

            printf("Owner: %s\n",
                   players[square->owner].name);
        }
    }

    printf("\n============================================================\n");
    printf("        PHASE 10 INTEGRATION TEST FINISHED\n");
    printf("============================================================\n");

    return 0;
}