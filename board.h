#ifndef BOARD_H
#define BOARD_H

void initBoard(Property board[]);
void printBoard(Property board[]);


DiceRoll rollDice(void);
int isDoubles(DiceRoll roll);

void printDie(void);

#endif


