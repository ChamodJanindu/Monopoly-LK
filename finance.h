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

int canInsureProperty(Property board[], int playerIndex, int squareIndex, InsurancePolicyType policyType);
int calculateInsurancePremium(Property *property, InsurancePolicyType policyType);
int purchaseInsurance(Player players[], int playerIndex, Property board[], int squareIndex, InsurancePolicyType policyType, InsuranceProvider provider, GameState *game);
void handleInsurance(Player players[], int playerIndex, Property board[], int insuranceSquareIndex, GameState *game);
void updateInsuranceAfterRound(Player players[], Property board[], GameState *game);
int isDisasterCovered(Property *property);
int calculateInsuranceCompensation(Property *property);
int processInsuranceClaim(Player players[], Property *property);
int repairDamagedProperty(Player players[], Property *property);
void checkAutomaticRepairs(Player players[], Property board[]);
void cancelInsurance(Property *property);

#endif
