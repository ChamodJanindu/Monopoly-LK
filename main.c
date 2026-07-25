#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

#include <time.h>


int main(void) {

    srand(time(NULL));

    Player players[NUM_PLAYERS];
    Property board[BOARD_SIZE];


    initBoard(board);
    initPlayer(&players[0], "Alice", STRATEGY_AGGRESSIVE);
    initPlayer(&players[1], "Bob", STRATEGY_CONSERVATIVE);
    initPlayer(&players[2], "Chan", STRATEGY_RISK_TAKER);
    initPlayer(&players[3], "Divya", STRATEGY_OPPORTUNISTIC);
    int numPlayers = NUM_PLAYERS;
    int numRounds = 20;

    testTurnRotation(players,numPlayers,board,numRounds);

    //handlelandingTest(testPositions, numTests, &player1, board);    

    //printBoard(board);
    
    //printDie();
   
    return 0;
    
}