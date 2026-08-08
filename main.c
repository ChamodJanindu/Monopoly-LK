#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

int main(void){

    srand(time(NULL));

    Property board[BOARD_SIZE];
    Player players[NUM_PLAYERS];

    initBoard(board);

    initPlayer(&players[0], "Aggressive", STRATEGY_AGGRESSIVE);
    initPlayer(&players[1], "Conservative", STRATEGY_CONSERVATIVE);
    initPlayer(&players[2], "Risk Taker", STRATEGY_RISK_TAKER);
    initPlayer(&players[3], "Opportunistic", STRATEGY_OPPORTUNISTIC);

    int turnOrder[NUM_PLAYERS] = {0, 1, 2, 3};


    printf("=============================================\n");
    printf("JAIL MECHANICS TEST\n");
    printf("=============================================\n");


    /*
     * TEST 1
     * Normal landing on Jail square.
     * Player should only be visiting.
     */

    printf("\n---------------------------------------------\n");
    printf("TEST 1 - JUST VISITING JAIL\n");
    printf("---------------------------------------------\n");

    players[0].position = 10;

    printf("Before:\n");
    printf("Position: %d\n", players[0].position);
    printf("isInJail: %d\n", players[0].isInJail);

    handleLanding(players, 0, board, 0, turnOrder);

    printf("\nAfter:\n");
    printf("Position: %d\n", players[0].position);
    printf("isInJail: %d\n", players[0].isInJail);


    /*
     * TEST 2
     * Landing on Go To Jail.
     */

    printf("\n---------------------------------------------\n");
    printf("TEST 2 - GO TO JAIL\n");
    printf("---------------------------------------------\n");

    players[1].position = 30;

    printf("Before:\n");
    printf("Position: %d\n", players[1].position);
    printf("isInJail: %d\n", players[1].isInJail);
    printf("Jail turns remaining: %d\n",
           players[1].jailTurnsRemaining);

    handleLanding(players, 1, board, 0, turnOrder);

    printf("\nAfter:\n");
    printf("Position: %d\n", players[1].position);
    printf("isInJail: %d\n", players[1].isInJail);
    printf("Jail turns remaining: %d\n",
           players[1].jailTurnsRemaining);


    /*
     * TEST 3
     * Simulate the player's future turns while jailed.
     *
     * They may:
     * - escape early by rolling doubles
     * OR
     * - remain jailed for three failed turns
     */

    printf("\n---------------------------------------------\n");
    printf("TEST 3 - JAIL TURNS\n");
    printf("---------------------------------------------\n");

    int jailTurn = 1;

    while(players[1].isInJail == 1){

        printf("\n--- Jail Turn %d ---\n", jailTurn);

        int positionBefore = players[1].position;

        handleJailTurn(players, 1, board, turnOrder);

        printf("Position before jail turn: %d\n",
               positionBefore);

        printf("Position after jail turn: %d\n",
               players[1].position);

        printf("isInJail: %d\n",
               players[1].isInJail);

        printf("Jail turns remaining: %d\n",
               players[1].jailTurnsRemaining);

        jailTurn++;
    }


    /*
     * TEST 4
     * After release, player should now be able
     * to take a normal turn.
     */

    printf("\n---------------------------------------------\n");
    printf("TEST 4 - NORMAL MOVEMENT AFTER RELEASE\n");
    printf("---------------------------------------------\n");

    printf("Player is now out of Jail.\n");
    printf("Current position: %d\n", players[1].position);

    DiceRoll roll = rollDice();
    int total = roll.die1 + roll.die2;

    printf("%s rolled %d + %d = %d\n",
           players[1].name,
           roll.die1,
           roll.die2,
           total);

    movePlayer(&players[1], total);

    handleLanding(players, 1, board, total, turnOrder);

    printf("New position: %d\n", players[1].position);


    printf("\n=============================================\n");
    printf("JAIL TESTS FINISHED\n");
    printf("=============================================\n");

    return 0;
}