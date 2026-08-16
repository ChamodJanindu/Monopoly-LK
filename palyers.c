#include "types.h"
#include "players.h"
#include"board.h"
#include "finance.h"
#include "events.h"
#include "game.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initPlayer(Player *player, const char *name, PlayerStrategy strategy){
    
    memset(player, 0, sizeof(*player));

    snprintf(player->name, sizeof(player->name), "%s", name);

    player->strategy = strategy;
    player->cash = STARTING_CASH;
    player->position = 0;

    player->isInJail = 0;
    player->jailTurnsRemaining = 0;
    player->isBankrupt = 0;

    player->numOwnedProperties = 0;

    player->passedGoThisRound = 0;
    player->lapCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        player->ownedProperties[i] = -1;
        player->loan.collateralIndices[i] = -1;
    }

    player->loan.isActive = 0;
    player->loan.amount = 0;
    player->loan.principalAmount = 0;
    player->loan.interestRate = 0;
    player->loan.roundsRemaining = 0;
    player->loan.numCollateral = 0;

    for(int i = 0; i < 20; i++){
        player->nationalCardRounds[i] = 0;
    }

    player->revaluationGroup = GROUP_NONE;
}

int addPropertyToPlayer(Player *player, int squareIndex){

    if(player->numOwnedProperties >= BOARD_SIZE){
        printf("ERROR: %s cannot store more properties.\n", player->name);
        return 0;
    }

    player->ownedProperties[player->numOwnedProperties] = squareIndex;
    player->numOwnedProperties++;

    return 1;
}

void movePlayer(Player *player, int diceTotal){

    int oldPosition = player->position;
    int rawNewPosition = oldPosition + diceTotal;
    int passedGo = (rawNewPosition >= BOARD_SIZE); 
        
    player->position = rawNewPosition % BOARD_SIZE;

    printf("%s moved to position %d\n", player->name, player->position);

    if(passedGo == 1){
        player->cash += GO_MONEY;
        player->lapCount++;

        printf("%s passed or landed on GO.\n", player->name);
        printf("Collected LKR %d.\n", GO_MONEY);
        printf("Current balance: LKR %d.\n", player->cash);
        printf("Personal laps completed: %d\n",
               player->lapCount);
    }

}


int wouldCompleteGroup(Player *player, Property *property, Property board[]){

    if(property->type != SQUARE_PROPERTY){
        return 0;
    }

    int totalInGroup = 0;
    int ownedInGroup = 0;

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY &&
           board[i].group == property->group){

            totalInGroup++;

            for(int j = 0; j < player->numOwnedProperties; j++){

                if(player->ownedProperties[j] == i){
                    ownedInGroup++;
                    break;
                }
            }
        }
    }

    if(ownedInGroup + 1 == totalInGroup){
        return 1;
    }
    else{
        return 0;
    }
}
int decidePurchase(Player *player, Property *property, Property board[], int purchasePrice, GameState *game){

    if(player->cash < purchasePrice){
        return 0;
    }

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:{

            int cashAfterPurchase = player->cash - purchasePrice;

            if(wouldCompleteGroup(player, property, board) == 1){
                return 1;
            }

            //at least needs to be able to play 1 future rent, just take the current property rent as the base rent amount.
            int futureRentReserve = property->baseRent;

            if(cashAfterPurchase >= futureRentReserve){
                return 1;
            }

            return 0;
        }

        case STRATEGY_CONSERVATIVE:{

            if(game->activeEconomicEvent == ECONOMIC_EVENT_RECESSION){
                return 0;
            }

            int cashAfterPurchase = player->cash - purchasePrice;

            if(cashAfterPurchase >= player->cash / 2){
                return 1;
            }

            return 0;
        }

        case STRATEGY_RISK_TAKER:

            return 1;

        case STRATEGY_OPPORTUNISTIC:{

            if(property->type == SQUARE_RAILWAY ||
               property->type == SQUARE_UTILITY){

                if(purchasePrice <= property->marketValue){
                    return 1;
                }

                return 0;
            }

            int expectedGain =
                property->marketValue - purchasePrice;

            if(game->boomGroup == property->group){
                expectedGain += property->marketValue * 20 / 100;
            }

            if(game->declineGroup == property->group){
                expectedGain -= property->marketValue * 15 / 100;
            }

            if(expectedGain > property->houseCost){
                return 1;
            }

            return 0;
        }

        default:
            return 0;
    }
}

int decideAuctionBid(Player *player, Property *property, int nextBid, int currentMarketValue){

    if(player->isBankrupt == 1){
        return 0;
    }

    if(player->cash < nextBid){
        return 0;
    }

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:{

            int maximumBid =
                currentMarketValue * 120 / 100;

            if(nextBid <= maximumBid){
                return 1;
            }

            return 0;
        }

        case STRATEGY_CONSERVATIVE:

            if(nextBid < currentMarketValue){
                return 1;
            }

            return 0;

        case STRATEGY_RISK_TAKER:

            return 1;

        case STRATEGY_OPPORTUNISTIC:{

            int maximumBid =
                currentMarketValue * 80 / 100;

            if(nextBid <= maximumBid){
                return 1;
            }

            return 0;
        }

        default:
            return 0;
    }
}

int decidePayBail(Player *player){

    if(player->cash < JAIL_BAIL){
        return 0;
    }

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            return 1;

        case STRATEGY_CONSERVATIVE:

            if(player->cash - JAIL_BAIL >= 2000){
                return 1;
            }

            return 0;

        case STRATEGY_RISK_TAKER:

            return 0;

        case STRATEGY_OPPORTUNISTIC:

            if(player->numOwnedProperties >= 3){
                return 1;
            }

            return 0;

        default:

            return 0;
    }
}


int decideBuildHouse(Player *player, Property *property, int constructionCost, GameState *game){

    if(player->cash < constructionCost){
        return 0;
    }

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            return 1;

        case STRATEGY_CONSERVATIVE:{

            int cashAfterBuilding =
                player->cash - constructionCost;
            
            //my implementation to only build while keeping half the cash
            if(cashAfterBuilding >= player->cash / 2){
                return 1;
            }

            return 0;
        }

        case STRATEGY_RISK_TAKER:

            return 1;

        case STRATEGY_OPPORTUNISTIC:

            //give priority to housing subsity above others
            if(game->activeRegulation == REGULATION_HOUSING_SUBSIDY){
                return 1;
            }

            if(game->currentInflationRate > 0){
                return 0;
            }

            return 1;

        default:

            return 0;
    }
}
int decideBuildHotel(Player *player, Property *property, int constructionCost, GameState *game){

    if(player->cash < constructionCost){
        return 0;
    }

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            return 1;

        case STRATEGY_CONSERVATIVE:{

            if(player->loan.isActive == 1){
                return 0;
            }

            int cashAfterBuilding =
                player->cash - constructionCost;

            if(cashAfterBuilding >= player->cash / 2){
                return 1;
            }

            return 0;
        }

        case STRATEGY_RISK_TAKER:

            return 1;

        case STRATEGY_OPPORTUNISTIC:

            if(game->currentInflationRate > 0){
                return 0;
            }

            return 1;

        default:

            return 0;
    }
}


int hasDevelopmentOpportunity(Player *player, Property board[]){

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];
        Property *property = &board[squareIndex];

        if(property->type != SQUARE_PROPERTY){
            continue;
        }

        int totalInGroup = 0;
        int ownedInGroup = 0;

        for(int j = 0; j < BOARD_SIZE; j++){

            if(board[j].type == SQUARE_PROPERTY &&
               board[j].group == property->group){

                totalInGroup++;

                if(board[j].owner == property->owner){
                    ownedInGroup++;
                }
            }
        }

        if(totalInGroup == ownedInGroup &&
           property->hasHotel == 0){

            return 1;
        }
    }

    return 0;
}
BankAction decideBankAction(Player *player, Property board[], GameState *game){

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            if(player->loan.isActive == 0){

                if(hasDevelopmentOpportunity(player, board) == 1){
                    return BANK_ACTION_TAKE_LOAN;
                }

                return BANK_ACTION_NONE;
            }

            if(player->cash > player->loan.amount * 2){
                return BANK_ACTION_REPAY_FULL;
            }

            if(player->loan.roundsRemaining <= 5){
                return BANK_ACTION_EXTEND;
            }

            return BANK_ACTION_NONE;

        case STRATEGY_CONSERVATIVE:

            if(player->loan.isActive == 1){

                if(player->cash >= player->loan.amount){
                    return BANK_ACTION_REPAY_FULL;
                }

                if(player->loan.roundsRemaining <= 5){
                    return BANK_ACTION_EXTEND;
                }

                return BANK_ACTION_NONE;
            }

            if(player->cash < GO_MONEY){
                return BANK_ACTION_TAKE_LOAN;
            }

            return BANK_ACTION_NONE;

        case STRATEGY_RISK_TAKER:

            if(player->loan.isActive == 0){
                return BANK_ACTION_TAKE_LOAN;
            }

            if(decideLoanIncreaseAmount(player, board, game) > 0){
                return BANK_ACTION_INCREASE;
            }

            if(player->loan.roundsRemaining <= 5){
                return BANK_ACTION_EXTEND;
            }

            return BANK_ACTION_NONE;

        case STRATEGY_OPPORTUNISTIC:{

            int maximumLoan =
                calculateMaximumLoan(player, board, game);

            int interestRate =
                calculateCurrentLoanInterestRate(game);

            int borrowingCost =
                maximumLoan * interestRate / 100;

            int projectedReturn = 0;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(property->type == SQUARE_PROPERTY &&
                   property->hasHotel == 0){

                    projectedReturn += property->baseRent;
                }
            }

            if(player->loan.isActive == 0){

                if(projectedReturn > borrowingCost){
                    return BANK_ACTION_TAKE_LOAN;
                }

                return BANK_ACTION_NONE;
            }

            if(player->cash >= player->loan.amount * 2){
                return BANK_ACTION_REPAY_FULL;
            }

            if(player->loan.roundsRemaining <= 5){
                return BANK_ACTION_EXTEND;
            }

            return BANK_ACTION_NONE;
        }

        default:

            return BANK_ACTION_NONE;
    }
}
int decideLoanAmount(Player *player, Property board[], GameState *game){

    int maximumLoan =
        calculateMaximumLoan(player, board, game);

    if(maximumLoan <= 0){
        return 0;
    }

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            return maximumLoan;

        case STRATEGY_CONSERVATIVE:

            return maximumLoan / 2;

        case STRATEGY_RISK_TAKER:

            return maximumLoan;

        case STRATEGY_OPPORTUNISTIC:

            return maximumLoan / 2;

        default:

            return 0;
    }
}
int decidePartialRepaymentAmount(Player *player){

    if(player->loan.isActive == 0){
        return 0;
    }

    int repaymentAmount =
        player->loan.amount / 2;

    if(repaymentAmount > player->cash){
        repaymentAmount = player->cash;
    }

    if(repaymentAmount >= player->loan.amount){
        return 0;
    }

    return repaymentAmount;
}
int decideLoanIncreaseAmount(Player *player, Property board[], GameState *game){

    int maximumLoan =
        calculateMaximumLoan(player, board, game);

    int availableIncrease =
        maximumLoan - player->loan.principalAmount;

    if(availableIncrease <= 0){
        return 0;
    }

    return availableIncrease;
}



int isAvailableForInsurance(Property *property){

    if(property->type != SQUARE_PROPERTY){
        return 0;
    }

    if(property->insurance.isActive == 1){
        return 0;
    }

    return 1;
}

int decideInsuranceProperty(Player *player, Property board[], GameState *game){

    int selectedProperty = -1;

    switch(player->strategy){

        //insurance any properties with 3 or more development level
        case STRATEGY_AGGRESSIVE:{

            int highestDevelopment = -1;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(isAvailableForInsurance(property) == 0){
                    continue;
                }

                int developmentLevel = getDevelopmentLevel(property);

                if(developmentLevel < 3){
                    continue;
                }

                if(developmentLevel > highestDevelopment){

                    highestDevelopment = developmentLevel;
                    selectedProperty = squareIndex;
                }
            }

            return selectedProperty;
        }

        //any property starting from hightest value
        case STRATEGY_CONSERVATIVE:{

            int highestValue = -1;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(isAvailableForInsurance(property) == 0){
                    continue;
                }

                if(property->marketValue > highestValue){

                    highestValue = property->marketValue;
                    selectedProperty = squareIndex;
                }
            }

            return selectedProperty;
        }

        //needs to update TODO
        case STRATEGY_RISK_TAKER:

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(isAvailableForInsurance(property) == 0){
                    continue;
                }
                
                //this needs to be fixed to find out if the proeprty has been damaged in the past
                if(property->isDamaged == 1){
                    return squareIndex;
                }
            }

            return -1;
        
        case STRATEGY_OPPORTUNISTIC:{

            int highestRent = -1;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(isAvailableForInsurance(property) == 0){
                    continue;
                }

                if(getDevelopmentLevel(property) == 0){
                    continue;
                }

                if(property->baseRent > highestRent){

                    highestRent = property->baseRent;
                    selectedProperty = squareIndex;
                }
            }

            return selectedProperty;
        }

        default:

            return -1;
    }
}
InsurancePolicyType decideInsurancePolicy(Player *player, Property *property){

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            if(property->hasHotel == 1){
                return POLICY_COMPREHENSIVE;
            }

            if(property->numHouses > 0){
                return POLICY_BASIC;
            }

            return POLICY_NONE;

        case STRATEGY_CONSERVATIVE:

            return POLICY_COMPREHENSIVE;

        case STRATEGY_RISK_TAKER:

            return POLICY_BASIC;

        case STRATEGY_OPPORTUNISTIC:

            return POLICY_COMPREHENSIVE;

        default:

            return POLICY_NONE;
    }
}

int decideMaintainBuilding(Player *player, BuildingCondition *building){

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            if(building->conditionRating < 75){
                return 1;
            }

            return 0;

        case STRATEGY_CONSERVATIVE:

            if(building->conditionRating < 90){
                return 1;
            }

            return 0;

        case STRATEGY_RISK_TAKER:

            if(building->conditionRating < 25){
                return 1;
            }

            return 0;

        case STRATEGY_OPPORTUNISTIC:

            if(building->conditionRating < 75){
                return 1;
            }

            return 0;

        default:

            return 0;
    }
}

int decidePropertyRenovation(Player *player, Property *property){

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            if(property->depreciationPercent >= 20){
                return 1;
            }

            return 0;

        case STRATEGY_CONSERVATIVE:

            if(property->depreciationPercent > 10){
                return 1;
            }

            return 0;

        case STRATEGY_RISK_TAKER:

            return 0;

        case STRATEGY_OPPORTUNISTIC:

            if(property->depreciationPercent > 15){
                return 1;
            }

            return 0;

        default:

            return 0;
    }
}

int decidePropertyToLiquidate(Player *player, Property board[], GameState *game){

    int selectedProperty = -1;

    switch(player->strategy){

        case STRATEGY_AGGRESSIVE:

            return -1;

        case STRATEGY_CONSERVATIVE:{

            int lowestValue = -1;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(property->isLoanLocked == 1){
                    continue;
                }

                if(property->numHouses > 0 ||
                   property->hasHotel == 1){
                    continue;
                }

                if(lowestValue == -1 ||
                   property->marketValue < lowestValue){

                    lowestValue = property->marketValue;
                    selectedProperty = squareIndex;
                }
            }

            return selectedProperty;
        }

        case STRATEGY_RISK_TAKER:{

            int lowestValue = -1;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(property->isLoanLocked == 1){
                    continue;
                }

                if(lowestValue == -1 ||
                   property->marketValue < lowestValue){

                    lowestValue = property->marketValue;
                    selectedProperty = squareIndex;
                }
            }

            return selectedProperty;
        }

        case STRATEGY_OPPORTUNISTIC:{

            int lowestValue = -1;

            for(int i = 0; i < player->numOwnedProperties; i++){

                int squareIndex = player->ownedProperties[i];
                Property *property = &board[squareIndex];

                if(property->isLoanLocked == 1){
                    continue;
                }

                int currentValue =
                    calculateCurrentPropertyValue(player, property, game);

                if(currentValue < property->marketValue){

                    if(lowestValue == -1 ||
                       currentValue < lowestValue){

                        lowestValue = currentValue;
                        selectedProperty = squareIndex;
                    }
                }
            }

            return selectedProperty;
        }

        default:

            return -1;
    }
}