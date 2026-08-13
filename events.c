#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "events.h"


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