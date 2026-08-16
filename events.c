#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "events.h"
#include "finance.h"

int isSouthernCoastalProperty(Property *property){

    if(strcmp(property->name, "Galle Fort") == 0 ||
       strcmp(property->name, "Unawatuna") == 0 ||
       strcmp(property->name, "Hikkaduwa") == 0){

        return 1;
    }

    return 0;
}

int isCoastalProperty(Property *property){

    if(strcmp(property->name, "Bambalapitiya") == 0 ||
       strcmp(property->name, "Wellawatte") == 0 ||
       strcmp(property->name, "Mount Lavinia") == 0 ||
       strcmp(property->name, "Negombo") == 0 ||
       strcmp(property->name, "Galle Fort") == 0 ||
       strcmp(property->name, "Unawatuna") == 0 ||
       strcmp(property->name, "Hikkaduwa") == 0 ||
       strcmp(property->name, "Trincomalee") == 0 ||
       strcmp(property->name, "Galle Face") == 0){

        return 1;
    }

    return 0;
}

int isCommercialProperty(Property *property){

    if(strcmp(property->name, "Pettah") == 0 ||
       strcmp(property->name, "Maradhana") == 0 ||
       strcmp(property->name, "Bambalapitiya") == 0 ||
       strcmp(property->name, "Nugegoda") == 0 ||
       strcmp(property->name, "Maharagama") == 0 ||
       strcmp(property->name, "Kandy City") == 0 ||
       strcmp(property->name, "Galle Fort") == 0 ||
       strcmp(property->name, "Jaffna Town") == 0 ||
       strcmp(property->name, "Galle Face") == 0){

        return 1;
    }

    return 0;
}



int generateInflationRate(void){

    int rates[] = {-3, 0, 2, 5, 8, 12};

    int randomIndex = rand() % 6;

    return rates[randomIndex];
}

int applyInflationToValue(int value, int inflationRate){

    return value * (100 + inflationRate) / 100;
}

void updateInflation(Property board[], GameState *game){

    if(game->completedRounds % 10 != 0){
        return;
    }

    int inflationRate = generateInflationRate();

    game->currentInflationRate = inflationRate;

    game->rentInflationIndex = game->rentInflationIndex * (100 + inflationRate) / 100;

    for(int i = 0; i < BOARD_SIZE; i++){

        Property *property = &board[i];

        if(property->type == SQUARE_PROPERTY ||
           property->type == SQUARE_RAILWAY ||
           property->type == SQUARE_UTILITY){

            property->purchasePrice =
                applyInflationToValue(property->purchasePrice, inflationRate);

            property->marketValue =
                applyInflationToValue(property->marketValue, inflationRate);

            property->mortgageValue =
                applyInflationToValue(property->mortgageValue, inflationRate);
        }

        if(property->type == SQUARE_PROPERTY){

            property->baseRent =
                applyInflationToValue(property->baseRent, inflationRate);

            property->houseCost =
                applyInflationToValue(property->houseCost, inflationRate);

            property->hotelCost =
                applyInflationToValue(property->hotelCost, inflationRate);

            if(property->repairCost > 0){

                property->repairCost =
                    applyInflationToValue(property->repairCost, inflationRate);
            }
        }
    }

    game->currentLoanInterestRate =
        applyInflationToValue(game->currentLoanInterestRate, inflationRate);

    printf("\nInflation review at Round %d\n",
           game->completedRounds);

    printf("New inflation rate: %+d%%\n",
           inflationRate);

    printf("Current loan interest rate: %d%%\n",
           game->currentLoanInterestRate);
}



EconomicEventType generateEconomicEvent(void){

    return (EconomicEventType)((rand() % 8) + 1);
}

const char *getEconomicEventName(EconomicEventType event){

    switch(event){

        case ECONOMIC_EVENT_TOURISM_BOOM:
            return "Tourism Boom";

        case ECONOMIC_EVENT_FUEL_CRISIS:
            return "Fuel Crisis";

        case ECONOMIC_EVENT_HEAVY_MONSOON:
            return "Heavy Monsoon";

        case ECONOMIC_EVENT_RECESSION:
            return "Economic Recession";

        case ECONOMIC_EVENT_STOCK_MARKET_BOOM:
            return "Stock Market Boom";

        case ECONOMIC_EVENT_HOUSING_PROGRAMME:
            return "Government Housing Programme";

        case ECONOMIC_EVENT_FOREIGN_INVESTMENT:
            return "Foreign Investment";

        case ECONOMIC_EVENT_POLITICAL_UNREST:
            return "Political Unrest";

        case ECONOMIC_EVENT_NONE:
            return "None";
    }

    return "Unknown";
}

void startEconomicEvent(GameState *game){

    game->activeEconomicEvent = generateEconomicEvent();

    game->economicEventRoundsRemaining = 15;

    printf("\n=============================================\n");
    printf("NATIONAL ECONOMIC EVENT\n");
    printf("=============================================\n");

    printf("%s has begun.\n",
           getEconomicEventName(game->activeEconomicEvent));

    printf("Duration: %d rounds\n",
           game->economicEventRoundsRemaining);
}

void updateEconomicEvent(GameState *game){

    if(game->activeEconomicEvent == ECONOMIC_EVENT_NONE){
        return;
    }

    if(game->economicEventRoundsRemaining > 0){
        game->economicEventRoundsRemaining--;
    }

    if(game->economicEventRoundsRemaining == 0){

        printf("%s has ended.\n",
               getEconomicEventName(game->activeEconomicEvent));

        game->activeEconomicEvent =
            ECONOMIC_EVENT_NONE;
    }
}


int applyEconomicRentModifier(Property *property, int rent, GameState *game){

    if(game->activeEconomicEvent == ECONOMIC_EVENT_TOURISM_BOOM &&
       property->type == SQUARE_PROPERTY &&
       property->hasHotel == 1){

        rent = rent * 2;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_FUEL_CRISIS &&
       property->type == SQUARE_RAILWAY){

        rent = rent * 2;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_RECESSION){

        rent = rent * 90 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_POLITICAL_UNREST &&
       property->type == SQUARE_PROPERTY &&
       property->hasHotel == 1){

        rent = rent * 50 / 100;
    }

    if(game->activeRegulation == REGULATION_RAILWAY_MODERNIZATION &&
       property->type == SQUARE_RAILWAY){

        rent = rent * 125 / 100;
    }

    if(game->activeRegulation == REGULATION_ELECTRICITY_TARIFF &&
       property->type == SQUARE_UTILITY){

        rent = rent * 120 / 100;
    }

    return rent;
}
int applyEconomicValueModifier(Property *property, int value, GameState *game){

    if(game->activeEconomicEvent == ECONOMIC_EVENT_TOURISM_BOOM &&
       isSouthernCoastalProperty(property) == 1){

        value = value * 115 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_HEAVY_MONSOON &&
       isCoastalProperty(property) == 1){

        value = value * 90 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_RECESSION){

        value = value * 85 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_STOCK_MARKET_BOOM){

        value = value * 110 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_FOREIGN_INVESTMENT &&
       isCommercialProperty(property) == 1){

        value = value * 120 / 100;
    }

    return value;
}


int calculateCurrentHouseCost(Player *player, Property *property, GameState *game){

    int cost = property->houseCost;

    if(game->activeEconomicEvent == ECONOMIC_EVENT_FUEL_CRISIS){
        cost = cost * 120 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_HOUSING_PROGRAMME){
        cost = cost * 75 / 100;
    }

    if(game->activeRegulation == REGULATION_HOUSING_SUBSIDY){
        cost = cost * 70 / 100;
    }
    
    cost = applyMarketConstructionModifier(property, cost, game);
    cost = applyNationalCardHouseCostModifier(player, cost);

    return cost;
}
int calculateCurrentHotelCost(Player *player, Property *property, GameState *game){

    int cost = property->hotelCost;

    if(game->activeEconomicEvent == ECONOMIC_EVENT_FUEL_CRISIS){
        cost = cost * 120 / 100;
    }
    
    cost = applyMarketConstructionModifier(property, cost, game);
    cost = applyNationalCardHotelCostModifier(player, cost);

    return cost;
}




GovernmentRegulationType generateGovernmentRegulation(void){

    return (GovernmentRegulationType)((rand() % 8) + 1);
}

const char *getGovernmentRegulationName(GovernmentRegulationType regulation){

    switch(regulation){

        case REGULATION_PROPERTY_TAX:
            return "Increase Property Tax";

        case REGULATION_REDUCE_LOAN_INTEREST:
            return "Reduce Loan Interest";

        case REGULATION_HOUSING_SUBSIDY:
            return "Housing Subsidy";

        case REGULATION_LUXURY_PROPERTY_TAX:
            return "Luxury Property Tax";

        case REGULATION_RAILWAY_MODERNIZATION:
            return "Railway Modernization";

        case REGULATION_ELECTRICITY_TARIFF:
            return "Electricity Tariff Revision";

        case REGULATION_INSURANCE:
            return "Insurance Regulation";

        case REGULATION_ANTI_SPECULATION:
            return "Anti-Speculation Act";

        case REGULATION_NONE:
            return "None";
    }

    return "Unknown";
}

void startGovernmentRegulation(GameState *game){

    game->activeRegulation =
        generateGovernmentRegulation();

    game->regulationRoundsRemaining = 20;

    printf("\n=============================================\n");
    printf("GOVERNMENT REGULATION\n");
    printf("=============================================\n");

    printf("%s is now active.\n",
           getGovernmentRegulationName(game->activeRegulation));

    printf("Duration: %d rounds\n",
           game->regulationRoundsRemaining);
}

void updateGovernmentRegulation(GameState *game){

    if(game->activeRegulation == REGULATION_NONE){
        return;
    }

    if(game->regulationRoundsRemaining > 0){
        game->regulationRoundsRemaining--;
    }

    if(game->regulationRoundsRemaining == 0){

        printf("%s has ended.\n",
               getGovernmentRegulationName(game->activeRegulation));

        game->activeRegulation = REGULATION_NONE;
    }
}

int calculateCurrentLoanInterestRate(GameState *game){

    int interestRate = game->currentLoanInterestRate;

    if(game->activeEconomicEvent == ECONOMIC_EVENT_RECESSION){
        interestRate = interestRate * 115 / 100;
    }

    if(game->activeEconomicEvent == ECONOMIC_EVENT_STOCK_MARKET_BOOM){
        interestRate = interestRate * 90 / 100;
    }

    if(game->activeRegulation == REGULATION_REDUCE_LOAN_INTEREST){

        interestRate -= 2;

        if(interestRate < 0){
            interestRate = 0;
        }
    }

    return interestRate;
}

int calculateLuxuryPropertyTax(Player players[], Property *property, GameState *game){

    if(game->activeRegulation != REGULATION_LUXURY_PROPERTY_TAX){
        return 0;
    }

    if(property->type != SQUARE_PROPERTY){
        return 0;
    }

    if(property->hasHotel == 0){
        return 0;
    }

    int propertyValue =
        calculateCurrentPropertyValue(players, property, game);

    return propertyValue * 25 / 100;
}

int countUndevelopedProperties(Player *player, Property board[]){

    int count = 0;

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];
        Property *property = &board[squareIndex];

        if(property->type == SQUARE_PROPERTY &&
           property->numHouses == 0 &&
           property->hasHotel == 0){

            count++;
        }
    }

    return count;
}

int requiresImmediateDevelopment(Player *player, Property board[], GameState *game){

    if(game->activeRegulation != REGULATION_ANTI_SPECULATION){
        return 0;
    }

    int undevelopedCount =
        countUndevelopedProperties(player, board);

    if(undevelopedCount >= 3){
        return 1;
    }

    return 0;
}

void checkDevelopmentDeadlines(Player players[], Property board[], GameState *game){

    for(int i = 0; i < NUM_PLAYERS; i++){

        if(players[i].isBankrupt == 1){
            continue;
        }

        for(int j = 0; j < players[i].numOwnedProperties; j++){

            int squareIndex =
                players[i].ownedProperties[j];

            Property *property =
                &board[squareIndex];

            if(property->developmentDeadlineRound == -1){
                continue;
            }

            if(property->numHouses > 0 ||
               property->hasHotel == 1){

                property->developmentDeadlineRound = -1;
                continue;
            }

            if(game->completedRounds >=
               property->developmentDeadlineRound){

                printf("ANTI-SPECULATION VIOLATION: %s failed to develop %s by Round %d.\n",
                       players[i].name,
                       property->name,
                       property->developmentDeadlineRound);

                property->developmentDeadlineRound = -1;
            }
        }
    }
}



static PropertyGroup marketGroups[] = {
    GROUP_BROWN,
    GROUP_LIGHT_BLUE,
    GROUP_PINK,
    GROUP_ORANGE,
    GROUP_RED,
    GROUP_YELLOW,
    GROUP_GREEN,
    GROUP_DARK_BLUE
};

int getMarketGroupIndex(PropertyGroup group){

    for(int i = 0; i < 8; i++){

        if(marketGroups[i] == group){
            return i;
        }
    }

    return -1;
}

const char *getPropertyGroupName(PropertyGroup group){

    switch(group){

        case GROUP_BROWN:
            return "Brown";

        case GROUP_LIGHT_BLUE:
            return "Light Blue";

        case GROUP_PINK:
            return "Pink";

        case GROUP_ORANGE:
            return "Orange";

        case GROUP_RED:
            return "Red";

        case GROUP_YELLOW:
            return "Yellow";

        case GROUP_GREEN:
            return "Green";

        case GROUP_DARK_BLUE:
            return "Dark Blue";

        case GROUP_NONE:
            return "None";
    }

    return "Unknown";
}

int canSelectMarketGroup(GameState *game, PropertyGroup group){

    int index = getMarketGroupIndex(group);

    if(index == -1){
        return 0;
    }

    if(game->completedRounds -
       game->lastMarketAffectedRound[index] < 30){

        return 0;
    }

    return 1;
}

PropertyGroup selectMarketGroup(GameState *game, PropertyGroup excludedGroup){

    PropertyGroup availableGroups[8];
    int availableCount = 0;

    for(int i = 0; i < 8; i++){

        PropertyGroup group = marketGroups[i];

        if(group == excludedGroup){
            continue;
        }

        if(canSelectMarketGroup(game, group) == 1){

            availableGroups[availableCount] = group;
            availableCount++;
        }
    }

    if(availableCount == 0){
        return GROUP_NONE;
    }

    int randomIndex = rand() % availableCount;

    return availableGroups[randomIndex];
}

void startMarketReview(GameState *game){

    PropertyGroup newBoom =
        selectMarketGroup(game, GROUP_NONE);

    if(newBoom == GROUP_NONE){
        printf("No eligible group available for Market Boom.\n");
        return;
    }

    PropertyGroup newDecline =
        selectMarketGroup(game, newBoom);

    if(newDecline == GROUP_NONE){
        printf("No eligible group available for Market Decline.\n");
        return;
    }

    game->boomGroup = newBoom;
    game->declineGroup = newDecline;

    game->boomRoundsRemaining = 10;
    game->declineRoundsRemaining = 10;

    int boomIndex =
        getMarketGroupIndex(newBoom);

    int declineIndex =
        getMarketGroupIndex(newDecline);

    game->lastMarketAffectedRound[boomIndex] =
        game->completedRounds;

    game->lastMarketAffectedRound[declineIndex] =
        game->completedRounds;

    printf("\n=============================================\n");
    printf("PROPERTY MARKET REVIEW\n");
    printf("=============================================\n");

    printf("Market Boom: %s Group\n",
           getPropertyGroupName(game->boomGroup));

    printf("Market Decline: %s Group\n",
           getPropertyGroupName(game->declineGroup));
}

void updateMarketConditions(GameState *game){

    if(game->boomRoundsRemaining > 0){

        game->boomRoundsRemaining--;

        if(game->boomRoundsRemaining == 0){

            printf("%s Group Market Boom has ended.\n",
                   getPropertyGroupName(game->boomGroup));

            game->boomGroup = GROUP_NONE;
        }
    }

    if(game->declineRoundsRemaining > 0){

        game->declineRoundsRemaining--;

        if(game->declineRoundsRemaining == 0){

            printf("%s Group Market Decline has ended.\n",
                   getPropertyGroupName(game->declineGroup));

            game->declineGroup = GROUP_NONE;
        }
    }
}


int applyMarketPurchasePriceModifier(Property *property, int value, GameState *game){

    if(property->group == GROUP_NONE){
        return value;
    }

    if(property->group == game->boomGroup){
        value = value * 115 / 100;
    }

    return value;
}
int applyMarketMortgageModifier(Property *property, int value, GameState *game){

    if(property->group == GROUP_NONE){
        return value;
    }
    if(property->group == game->boomGroup){
        value = value * 115 / 100;
    }

    if(property->group == game->declineGroup){
        value = value * 90 / 100;
    }

    return value;
}
int applyMarketRentModifier(Property *property, int rent, GameState *game){

    if(property->group == GROUP_NONE){
        return rent;
    }

    if(property->group == game->boomGroup){
        rent = rent * 125 / 100;
    }

    if(property->group == game->declineGroup){
        rent = rent * 80 / 100;
    }

    return rent;
}
int applyMarketConstructionModifier(Property *property, int cost, GameState *game){

    if(property->group == GROUP_NONE){
        return cost;   
    }
    if(property->group == game->boomGroup){
        cost = cost * 110 / 100;
    }

    return cost;
}
int applyMarketPropertyValueModifier(Property *property, int value, GameState *game){
    if(property->group == GROUP_NONE){
        return value;
    }

    if(property->group == game->boomGroup){
        value = value * 120 / 100;
    }

    if(property->group == game->declineGroup){
        value = value * 85 / 100;
    }

    return value;
}
int applyMarketAuctionModifier(Property *property, int value, GameState *game){
    if(property->group == GROUP_NONE){
        return value;
    }
    if(property->group == game->declineGroup){
        value = value * 75 / 100;
    }

    return value;
}


int calculateCurrentPurchasePrice(Property *property, GameState *game){

    int price = property->purchasePrice;

    price = applyMarketPurchasePriceModifier(property, price, game);

    return price;
}

int calculateCurrentMortgageValue(Property *property, GameState *game){

    int value = property->mortgageValue;

    value = applyMarketMortgageModifier(property, value, game);

    return value;
}




RegionalEventType generateRegionalEvent(void){

    return (RegionalEventType)((rand() % 12) + 1);
}

const char *getRegionalEventName(RegionalEventType event){

    switch(event){

        case REGIONAL_EVENT_SOUTHERN_TOURISM:
            return "Southern Tourism Boom";

        case REGIONAL_EVENT_PORT_CITY:
            return "Port City Expansion";

        case REGIONAL_EVENT_IT_GROWTH:
            return "IT Industry Growth";

        case REGIONAL_EVENT_NORTHERN_DEVELOPMENT:
            return "Northern Development Programme";

        case REGIONAL_EVENT_TEA_EXPORT:
            return "Tea Export Boom";

        case REGIONAL_EVENT_AIRPORT_EXPANSION:
            return "Airport Expansion";

        case REGIONAL_EVENT_UNIVERSITY_GROWTH:
            return "University City Growth";

        case REGIONAL_EVENT_BEACH_POLLUTION:
            return "Beach Pollution";

        case REGIONAL_EVENT_FLOOD_DAMAGE:
            return "Flood Damage";

        case REGIONAL_EVENT_TRANSPORT_STRIKE:
            return "Transport Strike";

        case REGIONAL_EVENT_ELECTRICITY_TARIFF:
            return "Electricity Tariff Increase";

        case REGIONAL_EVENT_WATER_SHORTAGE:
            return "Water Shortage";

        case REGIONAL_EVENT_NONE:
            return "None";
    }

    return "Unknown";
}

void startRegionalEvent(GameState *game){

    game->activeRegionalEvent = generateRegionalEvent();
    game->regionalEventRoundsRemaining = 15;

    printf("\n=============================================\n");
    printf("REGIONAL DEVELOPMENT CARD\n");
    printf("=============================================\n");

    printf("%s\n",
           getRegionalEventName(game->activeRegionalEvent));

    printf("Active for 15 rounds.\n");
}

void updateRegionalEvent(GameState *game){

    if(game->activeRegionalEvent == REGIONAL_EVENT_NONE){
        return;
    }

    if(game->regionalEventRoundsRemaining > 0){
        game->regionalEventRoundsRemaining--;
    }

    if(game->regionalEventRoundsRemaining == 0){

        printf("%s has ended.\n",
               getRegionalEventName(game->activeRegionalEvent));

        game->activeRegionalEvent = REGIONAL_EVENT_NONE;
    }
}

int applyRegionalRentModifier(Property *property, int rent, GameState *game){

    if(game->activeRegionalEvent == REGIONAL_EVENT_SOUTHERN_TOURISM){

        if(strcmp(property->name, "Galle Fort") == 0 ||
           strcmp(property->name, "Unawatuna") == 0 ||
           strcmp(property->name, "Hikkaduwa") == 0){

            rent = rent * 140 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_AIRPORT_EXPANSION){

        if(strcmp(property->name, "Negombo") == 0 ||
           strcmp(property->name, "Katunayake") == 0 ||
           strcmp(property->name, "Ja-Ela") == 0){

            rent = rent * 130 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_BEACH_POLLUTION){

        if(isSouthernCoastalProperty(property) == 1){

            rent = rent * 70 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_TRANSPORT_STRIKE &&
       property->type == SQUARE_RAILWAY){

        rent = rent * 60 / 100;
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_ELECTRICITY_TARIFF &&
        property->type == SQUARE_UTILITY &&
        strcmp(property->name, "Ceylon Electricity Board (CEB)") == 0){

            rent = rent * 125 / 100;
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_WATER_SHORTAGE &&
        property->type == SQUARE_UTILITY &&
        strcmp(property->name, "National Water Supply and Drainage Board (NWSDB)") == 0){

            rent = rent * 120 / 100;
    }

    return rent;
}

int isLowLyingCoastalProperty(Property *property){

    if(strcmp(property->name, "Bambalapitiya") == 0 ||
       strcmp(property->name, "Wellawatte") == 0 ||
       strcmp(property->name, "Mount Lavinia") == 0 ||
       strcmp(property->name, "Negombo") == 0 ||
       strcmp(property->name, "Unawatuna") == 0 ||
       strcmp(property->name, "Hikkaduwa") == 0 ||
       strcmp(property->name, "Galle Face") == 0){

        return 1;
    }

    return 0;
}

int applyRegionalValueModifier(Property *property, int value, GameState *game){

    if(game->activeRegionalEvent == REGIONAL_EVENT_PORT_CITY){

        if(strcmp(property->name, "Pettah") == 0 ||
        strcmp(property->name, "Maradhana") == 0 ||
        strcmp(property->name, "Colombo Fort Railway Station") == 0){

            value = value * 125 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_IT_GROWTH){

        if(strcmp(property->name, "Maharagama") == 0 ||
           strcmp(property->name, "Nugegoda") == 0 ||
           strcmp(property->name, "Kottawa") == 0){

            value = value * 120 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_NORTHERN_DEVELOPMENT){

        if(strcmp(property->name, "Jaffna Town") == 0 ||
           strcmp(property->name, "Nallur") == 0 ||
           strcmp(property->name, "Trincomalee") == 0){

            value = value * 130 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_TEA_EXPORT &&
       strcmp(property->name, "Nuwara Eliya") == 0){

        value = value * 135 / 100;
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_UNIVERSITY_GROWTH){

        if(strcmp(property->name, "Peradeniya") == 0 ||
           strcmp(property->name, "Kandy City") == 0){

            value = value * 120 / 100;
        }
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_FLOOD_DAMAGE &&
       isLowLyingCoastalProperty(property) == 1){

        value = value * 80 / 100;
    }

    if(game->activeRegionalEvent == REGIONAL_EVENT_WATER_SHORTAGE){

        if(strcmp(property->name, "Unawatuna") == 0 ||
        strcmp(property->name, "Hikkaduwa") == 0){

            value = value * 90 / 100;
        }
    }   

    return value;
}




void initNationalEventDeck(GameState *game){

    for(int i = 0; i < 20; i++){
        game->nationalEventDeck[i] = (NationalEventCardType)i;
    }

    game->nationalEventTop = 0;

    shuffleNationalEventDeck(game);
}

void shuffleNationalEventDeck(GameState *game){

    for(int i = 19; i > 0; i--){

        //get a random index between 0 and i
        int randomIndex = rand() % (i + 1);

        //swap places of card i with random card from deck
        NationalEventCardType temp = game->nationalEventDeck[i];

        game->nationalEventDeck[i] = game->nationalEventDeck[randomIndex];

        game->nationalEventDeck[randomIndex] = temp;
    }
}

NationalEventCardType drawNationalEventCard(GameState *game){

    NationalEventCardType card =
        game->nationalEventDeck[game->nationalEventTop];

    game->nationalEventTop++;

    if(game->nationalEventTop >= 20){
        game->nationalEventTop = 0;
    }

    return card;
}

const char *getNationalEventCardName(NationalEventCardType card){

    switch(card){

        case NATIONAL_CARD_TOURISM_HYPE:
            return "Tourism Hype";

        case NATIONAL_CARD_FUEL_SHORTAGE:
            return "Fuel Shortage";

        case NATIONAL_CARD_HEAVY_FLOODS:
            return "Heavy Floods";

        case NATIONAL_CARD_POLITICAL_RALLY:
            return "Political Rally";

        case NATIONAL_CARD_STOCK_MARKET_RISE:
            return "Stock Market Rise";

        case NATIONAL_CARD_ECONOMIC_DOWNTURN:
            return "Economic Downturn";

        case NATIONAL_CARD_HOUSING_SUBSIDY:
            return "Housing Subsidy";

        case NATIONAL_CARD_INTEREST_RATE_CUT:
            return "Interest Rate Cut";

        case NATIONAL_CARD_INTEREST_RATE_INCREASE:
            return "Interest Rate Increase";

        case NATIONAL_CARD_TAX_AMNESTY:
            return "Tax Amnesty";

        case NATIONAL_CARD_POWER_FAILURE:
            return "Power Failure";

        case NATIONAL_CARD_FOREIGN_FUNDING:
            return "Foreign Funding";

        case NATIONAL_CARD_PORT_EXPANSION:
            return "Port Expansion";

        case NATIONAL_CARD_FESTIVAL_SEASON:
            return "Festival Season";

        case NATIONAL_CARD_LABOUR_STRIKE:
            return "Labour Strike";

        case NATIONAL_CARD_INSURANCE_DISCOUNT:
            return "Insurance Discount";

        case NATIONAL_CARD_PROPERTY_REVALUATION:
            return "Property Revaluation";

        case NATIONAL_CARD_CURRENCY_DEPRECIATION:
            return "Currency Depreciation";

        case NATIONAL_CARD_GOVERNMENT_GRANT:
            return "Government Grant";

        case NATIONAL_CARD_NATIONAL_DISASTER:
            return "National Disaster";

        default:
            return "Unknown Card";
    }
}



void handleNationalEventCard(Player players[], int playerIndex, Property board[], GameState *game){

    Player *player = &players[playerIndex];

    NationalEventCardType card = drawNationalEventCard(game);

    printf("\nNational Event Card\n");
    printf("-------------------\n");
    printf("%s drew: %s\n",
           player->name,
           getNationalEventCardName(card));


    if(card == NATIONAL_CARD_TAX_AMNESTY){

        for(int i = 0; i < NUM_PLAYERS; i++){

            if(players[i].isBankrupt == 0){
                players[i].cash += 2000;
            }
        }

        printf("Each active player receives LKR 2000.\n");
    }


    if(card == NATIONAL_CARD_GOVERNMENT_GRANT){

        int possiblePlayers[NUM_PLAYERS];
        int count = 0;

        for(int i = 0; i < NUM_PLAYERS; i++){

            if(players[i].isBankrupt == 0){

                possiblePlayers[count] = i;
                count++;
            }
        }

        if(count > 0){

            int selectedPlayer =
                possiblePlayers[rand() % count];

            players[selectedPlayer].cash += 5000;

            printf("%s receives a Government Grant of LKR 5000.\n",
                   players[selectedPlayer].name);
        }
    }


    if(card == NATIONAL_CARD_INTEREST_RATE_CUT){

        player->nationalCardRounds[NATIONAL_CARD_INTEREST_RATE_CUT] = 15;

        printf("%s receives -2%% loan interest for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_INTEREST_RATE_INCREASE){

        player->nationalCardRounds[NATIONAL_CARD_INTEREST_RATE_INCREASE] = 15;

        printf("%s receives +2%% loan interest for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_TOURISM_HYPE){

        player->nationalCardRounds[NATIONAL_CARD_TOURISM_HYPE] = 5;

        printf("%s's hotels earn double rent for 5 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_FUEL_SHORTAGE){

        player->nationalCardRounds[NATIONAL_CARD_FUEL_SHORTAGE] = 5;

        printf("%s's railway rent is doubled for 5 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_POWER_FAILURE){

        player->nationalCardRounds[NATIONAL_CARD_POWER_FAILURE] = 3;

        printf("%s's utility income is halved for 3 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_FESTIVAL_SEASON){

        player->nationalCardRounds[NATIONAL_CARD_FESTIVAL_SEASON] = 15;

        printf("%s's hotels receive 50%% additional rent for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_HOUSING_SUBSIDY){

        player->nationalCardRounds[NATIONAL_CARD_HOUSING_SUBSIDY] = 15;

        printf("%s receives 30%% cheaper house construction for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_LABOUR_STRIKE){

        player->nationalCardRounds[NATIONAL_CARD_LABOUR_STRIKE] = 2;

        printf("%s cannot construct buildings for 2 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_INSURANCE_DISCOUNT){

        player->nationalCardRounds[NATIONAL_CARD_INSURANCE_DISCOUNT] = 15;

        printf("%s receives 20%% cheaper insurance premiums for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_STOCK_MARKET_RISE){

    player->nationalCardRounds[NATIONAL_CARD_STOCK_MARKET_RISE] = 15;

    printf("%s's property values increase by 10%% for 15 rounds.\n",
           player->name);
    }

    if(card == NATIONAL_CARD_ECONOMIC_DOWNTURN){

        player->nationalCardRounds[NATIONAL_CARD_ECONOMIC_DOWNTURN] = 15;

        printf("%s's property values decrease by 15%% for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_FOREIGN_FUNDING){

        player->nationalCardRounds[NATIONAL_CARD_FOREIGN_FUNDING] = 15;

        printf("%s's commercial property values increase by 15%% for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_PORT_EXPANSION){

        player->nationalCardRounds[NATIONAL_CARD_PORT_EXPANSION] = 15;

        printf("%s's railway station values increase by 20%% for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_PROPERTY_REVALUATION){

        player->nationalCardRounds[NATIONAL_CARD_PROPERTY_REVALUATION] = 15;

        player->revaluationGroup =
            (PropertyGroup)(rand() % 8);

        printf("%s's %d property group appreciates by 15%% for 15 rounds.\n",
            player->name,
            player->revaluationGroup);
    }

    if(card == NATIONAL_CARD_CURRENCY_DEPRECIATION){

        player->nationalCardRounds[NATIONAL_CARD_CURRENCY_DEPRECIATION] = 15;

        printf("%s's construction costs increase by 10%% for 15 rounds.\n",
            player->name);
    }

    if(card == NATIONAL_CARD_HEAVY_FLOODS){

        int squareIndex = getRandomCoastalProperty(board, playerIndex);

        if(squareIndex != -1){

            Property *property = &board[squareIndex];

            property->isDamaged = 1;
            property->damageType = DISASTER_FLOOD;
            property->repairCost = property->marketValue * 20 / 100;

            processInsuranceClaim(players, property);

            printf("Heavy Floods damaged %s.\n",
                board[squareIndex].name);
        }
    }

    if(card == NATIONAL_CARD_NATIONAL_DISASTER){

        int squareIndex =
            getRandomDevelopedProperty(board, playerIndex);

        if(squareIndex != -1){

            Property *property = &board[squareIndex];

            DisasterType disasters[] = {
                DISASTER_FIRE,
                DISASTER_FLOOD,
                DISASTER_RIOT,
                DISASTER_BUILDING_COLLAPSE,
                DISASTER_ELECTRICAL_FAILURE
            };

            property->isDamaged = 1;
            property->damageType =
                disasters[rand() % 5];

            property->repairCost =
                property->marketValue * 20 / 100;

            printf("National Disaster damaged %s.\n",
                property->name);

            processInsuranceClaim(players, property);
        }
        else{

            printf("%s has no developed property to be damaged.\n",
                player->name);
        }
    }

    if(card == NATIONAL_CARD_POLITICAL_RALLY){

        int squareIndex = getRandomOwnedProperty(board, playerIndex);

        if(squareIndex != -1){

            board[squareIndex].nationalEventClosedRounds = 2;

            printf("Political Rally closed %s for 2 rounds.\n",
                board[squareIndex].name);
        }
    }
}

void updateNationalCardEffects(Player players[]){

    for(int i = 0; i < NUM_PLAYERS; i++){

        if(players[i].isBankrupt == 1){
            continue;
        }

        for(int j = 0; j < 20; j++){

            if(players[i].nationalCardRounds[j] > 0){
                players[i].nationalCardRounds[j]--;
            }
        }

        if(players[i].nationalCardRounds[NATIONAL_CARD_PROPERTY_REVALUATION] == 0){
            players[i].revaluationGroup = GROUP_NONE;
        }
    }
}

int applyNationalCardInterestModifier(Player *player, int rate){

    if(player->nationalCardRounds[NATIONAL_CARD_INTEREST_RATE_CUT] > 0){
        rate -= 2;
    }

    if(player->nationalCardRounds[NATIONAL_CARD_INTEREST_RATE_INCREASE] > 0){
        rate += 2;
    }

    if(rate < 0){
        rate = 0;
    }

    return rate;
}
int applyNationalCardRentModifier(Player *owner, Property *property, int rent){

    if(owner->nationalCardRounds[NATIONAL_CARD_TOURISM_HYPE] > 0 &&
       property->type == SQUARE_PROPERTY &&
       property->hasHotel == 1){

        rent = rent * 2;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_FUEL_SHORTAGE] > 0 &&
       property->type == SQUARE_RAILWAY){

        rent = rent * 2;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_POWER_FAILURE] > 0 &&
       property->type == SQUARE_UTILITY){

        rent = rent * 50 / 100;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_FESTIVAL_SEASON] > 0 &&
       property->type == SQUARE_PROPERTY &&
       property->hasHotel == 1){

        rent = rent * 150 / 100;
    }

    return rent;
}
int applyNationalCardHouseCostModifier(Player *player, int cost){

    if(player->nationalCardRounds[NATIONAL_CARD_HOUSING_SUBSIDY] > 0){
        cost = cost * 70 / 100;
    }
    
    if(player->nationalCardRounds[NATIONAL_CARD_CURRENCY_DEPRECIATION] > 0){
        cost = cost * 110 / 100;
    }
    
    return cost;
}
int applyNationalCardHotelCostModifier(Player *player, int cost){

    if(player->nationalCardRounds[NATIONAL_CARD_CURRENCY_DEPRECIATION] > 0){
        cost = cost * 110 / 100;
    }

    return cost;
}
int canConstructWithNationalCards(Player *player){

    if(player->nationalCardRounds[NATIONAL_CARD_LABOUR_STRIKE] > 0){
        return 0;
    }

    return 1;
}
int applyNationalCardInsuranceModifier(Player *player, int premium){

    if(player->nationalCardRounds[NATIONAL_CARD_INSURANCE_DISCOUNT] > 0){
        premium = premium * 80 / 100;
    }

    return premium;
}
int applyNationalCardValueModifier(Player *owner, Property *property, int value){

    if(owner == NULL){
        return value;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_STOCK_MARKET_RISE] > 0){
        value = value * 110 / 100;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_ECONOMIC_DOWNTURN] > 0){
        value = value * 85 / 100;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_FOREIGN_FUNDING] > 0 &&
       isCommercialProperty(property)){
        value = value * 115 / 100;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_PORT_EXPANSION] > 0 &&
       property->type == SQUARE_RAILWAY){
        value = value * 120 / 100;
    }

    if(owner->nationalCardRounds[NATIONAL_CARD_PROPERTY_REVALUATION] > 0 &&
       property->group == owner->revaluationGroup){
        value = value * 115 / 100;
    }

    return value;
}

int getRandomCoastalProperty(Property board[], int playerIndex){

    int possible[BOARD_SIZE];
    int count = 0;

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY &&
           board[i].owner == playerIndex &&
           isCoastalProperty(&board[i])){

            possible[count] = i;
            count++;
        }
    }

    if(count == 0){
        return -1;
    }

    return possible[rand() % count];
}
int getRandomDevelopedProperty(Property board[], int playerIndex){

    int possible[BOARD_SIZE];
    int count = 0;

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY &&
           board[i].owner == playerIndex &&
           (board[i].numHouses > 0 || board[i].hasHotel == 1)){

            possible[count] = i;
            count++;
        }
    }

    if(count == 0){
        return -1;
    }

    return possible[rand() % count];
}
int getRandomOwnedProperty(Property board[], int playerIndex){

    int possible[BOARD_SIZE];
    int count = 0;

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY &&
           board[i].owner == playerIndex){

            possible[count] = i;
            count++;
        }
    }

    if(count == 0){
        return -1;
    }

    return possible[rand() % count];
}

void updateNationalEventProperties(Property board[]){

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].nationalEventClosedRounds > 0){
            board[i].nationalEventClosedRounds--;
        }
    }
}

void displayCurrentMarketConditions(GameState *game){

    printf("\n=============================================\n");
    printf("CURRENT ECONOMIC CONDITIONS\n");
    printf("=============================================\n");

    printf("Inflation Rate: %+d%%\n",
           game->currentInflationRate);

    printf("Current Loan Interest Rate: %d%%\n",
           game->currentLoanInterestRate);

    if(game->boomGroup != GROUP_NONE){

        printf("Market Boom: %s Group (%d rounds remaining)\n",
               getPropertyGroupName(game->boomGroup),
               game->boomRoundsRemaining);
    }
    else{
        printf("Market Boom: None\n");
    }

    if(game->declineGroup != GROUP_NONE){

        printf("Market Decline: %s Group (%d rounds remaining)\n",
               getPropertyGroupName(game->declineGroup),
               game->declineRoundsRemaining);
    }
    else{
        printf("Market Decline: None\n");
    }

    if(game->activeRegionalEvent != REGIONAL_EVENT_NONE){

        printf("Regional Development: %s (%d rounds remaining)\n",
               getRegionalEventName(game->activeRegionalEvent),
               game->regionalEventRoundsRemaining);
    }
    else{
        printf("Regional Development: None\n");
    }

    if(game->activeEconomicEvent != ECONOMIC_EVENT_NONE){

        printf("National Economic Event: %s (%d rounds remaining)\n",
               getEconomicEventName(game->activeEconomicEvent),
               game->economicEventRoundsRemaining);
    }
    else{
        printf("National Economic Event: None\n");
    }

    if(game->activeRegulation != REGULATION_NONE){

        printf("Government Regulation: %s (%d rounds remaining)\n",
               getGovernmentRegulationName(game->activeRegulation),
               game->regulationRoundsRemaining);
    }
    else{
        printf("Government Regulation: None\n");
    }

    printf("=============================================\n");
}

//for auction you need to apply all the peroperty value modifiers before taking the peroperty price
int calculateAuctionMarketValue(Property *property, GameState *game){

    int value = property->marketValue;

    value = applyEconomicValueModifier(property, value, game);
    value = applyMarketPropertyValueModifier(property, value, game);
    value = applyRegionalValueModifier(property, value, game);

    return value;
}