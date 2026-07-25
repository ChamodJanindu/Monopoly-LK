#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

#include <time.h>


int main(void) {

    srand(time(NULL));

    Player player1, player2;
    Property board[BOARD_SIZE];


    initBoard(board);
    initPlayer(&player1, "Bob", STRATEGY_AGGRESSIVE);
    initPlayer(&player2, "Alice", STRATEGY_CONSERVATIVE);

    testing001(&player1, board);
    //handlelandingTest(testPositions, numTests, &player1, board);    

    //printBoard(board);
    
    //printDie();
   
    return 0;
    
}