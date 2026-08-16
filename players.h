#ifndef PLAYERS_H
#define PLAYERS_H 

#include "types.h"

void initPlayer(Player *player, const char *name, PlayerStrategy strategy);
void movePlayer(Player *player, int diceTotal);

int decidePurchase(Player *player, Property *property, Property board[], int purchasePrice, GameState *game);
int addPropertyToPlayer(Player *player, int squareIndex);

int decideAuctionBid(Player *player, Property *property, int nextBid, int currentMarketValue);
int decidePayBail(Player *player);

int decideBuildHouse(Player *player, Property *property, int constructionCost, GameState *game);
int decideBuildHotel(Player *player, Property *property, int constructionCost, GameState *game);

BankAction decideBankAction(Player *player, Property board[], GameState *game);
int decideLoanAmount(Player *player, Property board[], GameState *game);
int decidePartialRepaymentAmount(Player *player);
int decideLoanIncreaseAmount(Player *player, Property board[], GameState *game);

int decideInsuranceProperty(Player *player, Property board[], GameState *game);
InsurancePolicyType decideInsurancePolicy(Player *player, Property *property);

int decideMaintainBuilding(Player *player, BuildingCondition *building);
int decidePropertyRenovation(Player *player, Property *property);

int decidePropertyToLiquidate(Player *player, Property board[], GameState *game);

#endif