#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"


int calculateRent(Player players[], Property board[], int squareIndex, int diceTotal, GameState *game);
void payRent(Player players[], int playerIndex, Property board[], int squareIndex, int diceTotal, GameState *game);

int calculateIncomeTax(Player players[], int playerIndex, Property board[], GameState *game);
void payTax(Player players[], int playerIndex, Property *taxSquare, Property board[], GameState *game);
int calculatePlayerTaxableAssets(Player players[], int playerIndex, Property board[], GameState *game);
int calculateCommunityFundTax(Player players[], int playerIndex, Property board[], GameState *game);
void payCommunityFundTax(Player players[], int playerIndex, Property board[], GameState *game);

int calculateTotalMortgageValue(Player *player, Property board[], GameState *game);
int calculateMaximumLoan(Player *player, Property board[], GameState *game);

int createLoan(Player *player, Property board[], int loanAmount, int interestRate, GameState *game);

int lockLoanCollateral(Player *player, Property board[], int loanAmount, GameState *game);
void unlockLoanCollateral(Player *player, Property board[]);

void updateLoanAfterRound(Player *player);

int repayLoanPart(Player *player, int repaymentAmount);
int repayLoanFull(Player *player, Property board[]);

int extendLoan(Player *player);

int addLoanCollateral(Player *player, Property board[], int requiredLoanAmount, GameState *game);
int increaseLoan(Player *player, Property board[], int additionalAmount, GameState *game);

void handleLoanDefault(Player players[], int playerIndex, Property board[], int turnOrder[], GameState *game);
void handleBank(Player players[], int playerIndex, Property board[], GameState *game);


int canInsureProperty(Property board[], int playerIndex, int squareIndex, InsurancePolicyType policyType);

int calculateInsurancePremium(Player players[], Property *property, InsurancePolicyType policyType, GameState *game);

int purchaseInsurance(Player players[], int playerIndex, Property board[], int squareIndex, InsurancePolicyType policyType, InsuranceProvider provider, GameState *game);

void handleInsurance(Player players[], int playerIndex, Property board[], int insuranceSquareIndex, GameState *game);

void updateInsuranceAfterRound(Player players[], Property board[], GameState *game);

int isDisasterCovered(Property *property);
int calculateInsuranceCompensation(Property *property);

int processInsuranceClaim(Player players[], Property *property);

int repairDamagedProperty(Player players[], Property *property);
void checkAutomaticRepairs(Player players[], Property board[]);

void cancelInsurance(Property *property);


void updatePropertyAges(Property board[]);
void updatePropertyDepreciation(Property board[]);

int calculateDepreciatedValue(Property *property);

int canRenovateProperty(Player players[], int playerIndex, Property board[], int squareIndex, GameState *game);

int calculateRenovationCost(Player players[], Property *property, GameState *game);

int renovateProperty(Player players[], int playerIndex, Property board[], int squareIndex, GameState *game);


void updateBuildingConditions(Property board[]);

int calculateAverageBuildingCondition(Property *property);
int hasStructuralDamage(Property *property);


int calculateHouseMaintenanceCost(Property *property, int houseIndex);
int calculateHotelMaintenanceCost(Property *property);

int maintainHouse(Player *player, Property *property, int houseIndex);
int maintainHotel(Player *player, Property *property);


int calculateHouseStructuralRenovationCost(Property *property, int houseIndex);
int calculateHotelStructuralRenovationCost(Property *property);

int renovateHouseStructuralDamage(Player *player, Property *property, int houseIndex);
int renovateHotelStructuralDamage(Player *player, Property *property);

int calculateCurrentPurchasePrice(Property *property, GameState *game);
int calculateCurrentPropertyValue(Player players[], Property *property, GameState *game);

void triggerRandomDisaster(Player players[], Property board[]);

#endif