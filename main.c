#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

static void printPlayerRoundStatus(
    Player players[],
    int numPlayers
)
{
    printf("\nCurrent player round status:\n");

    for (int i = 0; i < numPlayers; i++) {

        printf(
            "%s: position=%d, laps=%d, jailed=%s, "
            "bankrupt=%s, cash=%d\n",
            players[i].name,
            players[i].position,
            players[i].lapCount,
            players[i].isInJail ? "YES" : "NO",
            players[i].isBankrupt ? "YES" : "NO",
            players[i].cash
        );
    }
}

int main(void)
{
    /*
     * Use a fixed seed during testing.
     * This makes the dice sequence repeatable.
     *
     * Later, change this back to:
     * srand((unsigned int)time(NULL));
     */
    srand(1);

    Property board[BOARD_SIZE];
    Player players[NUM_PLAYERS];
    GameState game;

    int turnOrder[NUM_PLAYERS] = {0, 1, 2, 3};

    initBoard(board);
    initGameState(&game);

    initPlayer(
        &players[0],
        "Aggressive Investor",
        STRATEGY_AGGRESSIVE
    );

    initPlayer(
        &players[1],
        "Conservative Banker",
        STRATEGY_CONSERVATIVE
    );

    initPlayer(
        &players[2],
        "Risk Taker",
        STRATEGY_RISK_TAKER
    );

    initPlayer(
        &players[3],
        "Opportunistic Trader",
        STRATEGY_OPPORTUNISTIC
    );

printf("\n=============================================\n");
printf("PROPERTY OWNERSHIP TEST\n");
printf("=============================================\n");

int propertyIndex = 5;
int playerIndex = 0;

printf("Before assignment:\n");
printf("Property: %s\n", board[propertyIndex].name);
printf("Owner: %d\n", board[propertyIndex].owner);
printf("Player property count: %d\n",
       players[playerIndex].numOwnedProperties);

int success = assignPropertyToPlayer(
    players,
    playerIndex,
    board,
    propertyIndex
);

printf("\nAssignment result: %s\n",
       success ? "SUCCESS" : "FAILED");

printf("\nAfter assignment:\n");
printf("Property owner: %d\n",
       board[propertyIndex].owner);
printf("Player property count: %d\n",
       players[playerIndex].numOwnedProperties);
printf("Stored property index: %d\n",
       players[playerIndex].ownedProperties[0]);

printf("\nExpected:\n");
printf("Owner should be 0\n");
printf("Player property count should be 1\n");
printf("Stored property index should be 5\n");
}