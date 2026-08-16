#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

int isSouthernCoastalProperty(Property *property);
int isCoastalProperty(Property *property);
int isCommercialProperty(Property *property);

int generateInflationRate(void);
int applyInflationToValue(int value, int inflationRate);
void updateInflation(Property board[], GameState *game);

EconomicEventType generateEconomicEvent(void);
const char *getEconomicEventName(EconomicEventType event);
void startEconomicEvent(GameState *game);
void updateEconomicEvent(GameState *game);

int applyEconomicRentModifier(Property *property, int rent, GameState *game);
int applyEconomicValueModifier(Property *property, int value, GameState *game);
int calculateCurrentHouseCost(Player *player, Property *property, GameState *game);
int calculateCurrentHotelCost(Player *player, Property *property, GameState *game);

//need to modify interest rate based on economic events
int calculateCurrentLoanInterestRate(GameState *game);

GovernmentRegulationType generateGovernmentRegulation(void);
const char *getGovernmentRegulationName(GovernmentRegulationType regulation);
void startGovernmentRegulation(GameState *game);
void updateGovernmentRegulation(GameState *game);

int calculateLuxuryPropertyTax(Player players[], Property *property, GameState *game);
int countUndevelopedProperties(Player *player, Property board[]);

int requiresImmediateDevelopment(Player *player, Property board[], GameState *game);
void checkDevelopmentDeadlines(Player players[], Property board[], GameState *game);

int getMarketGroupIndex(PropertyGroup group);
const char *getPropertyGroupName(PropertyGroup group);
int canSelectMarketGroup(GameState *game, PropertyGroup group);
PropertyGroup selectMarketGroup(GameState *game, PropertyGroup excludedGroup);
void startMarketReview(GameState *game);
void updateMarketConditions(GameState *game);

int applyMarketPurchasePriceModifier(Property *property, int value, GameState *game);
int applyMarketMortgageModifier(Property *property, int value, GameState *game);
int applyMarketRentModifier(Property *property, int rent, GameState *game);
int applyMarketConstructionModifier(Property *property, int cost, GameState *game);
int applyMarketPropertyValueModifier(Property *property, int value, GameState *game);
int applyMarketAuctionModifier(Property *property, int value, GameState *game);

int calculateCurrentMortgageValue(Property *property, GameState *game);

int calculateCurrentPurchasePrice(Property *property, GameState *game);

RegionalEventType generateRegionalEvent(void);
const char *getRegionalEventName(RegionalEventType event);
void startRegionalEvent(GameState *game);
void updateRegionalEvent(GameState *game);
int applyRegionalRentModifier(Property *property, int rent, GameState *game);
int isLowLyingCoastalProperty(Property *property);
int applyRegionalValueModifier(Property *property, int value, GameState *game);


void initNationalEventDeck(GameState *game);
void shuffleNationalEventDeck(GameState *game);
NationalEventCardType drawNationalEventCard(GameState *game);
const char *getNationalEventCardName(NationalEventCardType card);
void handleNationalEventCard(Player players[], int playerIndex, Property board[], GameState *game);
void updateNationalCardEffects(Player players[]);
int applyNationalCardInterestModifier(Player *player, int rate);
int applyNationalCardRentModifier(Player *owner, Property *property, int rent);
int applyNationalCardHouseCostModifier(Player *player, int cost);
int applyNationalCardHotelCostModifier(Player *player, int cost);
int canConstructWithNationalCards(Player *player);
int applyNationalCardInsuranceModifier(Player *player, int premium);
int applyNationalCardValueModifier(Player *owner, Property *property, int value);
int getRandomCoastalProperty(Property board[], int playerIndex);
int getRandomDevelopedProperty(Property board[], int playerIndex);
int getRandomOwnedProperty(Property board[], int playerIndex);
void updateNationalEventProperties(Property board[]);


void displayCurrentMarketConditions(GameState *game);

int calculateAuctionMarketValue(Property *property, GameState *game);

#endif