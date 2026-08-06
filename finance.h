#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int calculateRent(Property board[], int squareIndex, int diceTotal);
void payRent(Player players[], int playerIndex, Property board[], int squareIndex, int diceTotal);

#endif