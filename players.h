#ifndef PLAYERS_H
#define PLAYERS_H 

#include "types.h"

void initPlayer(Player *player, const char *name, PlayerStrategy strategy);
void movePlayer(Player *player, int diceTotal);

int decidePurchase(Player *player, Property *property);
int addPropertyToPlayer(Player *player, int squareIndex);
int decideAuctionBid(Player *player, Property *property, int nextBid);
int decidePayBail(Player *player);

int decideBuildHouse(Player *player, Property *property);
int decideBuildHotel(Player *player, Property *property);

BankAction decideBankAction(Player *player);
int decideLoanAmount(Player *player, Property board[]);
int decidePartialRepaymentAmount(Player *player);
int decideLoanIncreaseAmount(Player *player, Property board[]);

int decideInsuranceProperty(Player *player, Property board[]);
InsurancePolicyType decideInsurancePolicy(Player *player, Property *property);

#endif