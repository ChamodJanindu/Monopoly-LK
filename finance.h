#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int calculateRent(Property board[], int squareIndex, int diceTotal);
void payRent(Player players[], int playerIndex, Property board[], int squareIndex, int diceTotal);
void payTax(Player *player, Property *taxSquare);

int calculateTotalMortgageValue(Player *player, Property board[]);
int calculateMaximumLoan(Player *player, Property board[]);
int createLoan(Player *player, Property board[], int loanAmount, int interestRate);
int lockLoanCollateral(Player *player, Property board[], int loanAmount);
void unlockLoanCollateral(Player *player, Property board[]);
void updateLoanAfterRound(Player *player);
int repayLoanPart(Player *player, int repaymentAmount);
int repayLoanFull(Player *player, Property board[]);
int extendLoan(Player *player);
int addLoanCollateral(Player *player, Property board[], int requiredLoanAmount);
int increaseLoan(Player *player, Property board[], int additionalAmount);
void handleLoanDefault(Player players[], int playerIndex, Property board[], int turnOrder[]);
void handleBank(Player players[], int playerIndex, Property board[], GameState *game);

#endif
