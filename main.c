#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "board.h"
#include "players.h"

#include <time.h>


int main(void) {

    srand(time(NULL));

    Player player1, player2;
    Property board[BOARD_SIZE];
    
    initBoard(board);
    initPlayer(&player1, "Bob", STRATEGY_AGGRESSIVE);
    initPlayer(&player2, "Alice", STRATEGY_CONSERVATIVE);

    DiceRoll roll = rollDice();
    int Total = roll.die1 + roll.die2;
    movePlayer(&player1, Total);

    

    //printBoard(board);
    
    //printDie();
   
    return 0;
    
}