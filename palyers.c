#include "types.h"
#include "players.h"
#include"board.h"

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
    player->loan.interestRate = 0;
    player->loan.roundsRemaining = 0;
    player->loan.numCollateral = 0;
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

int decidePurchase(Player *player, Property *property){

    if(player->cash >= property->purchasePrice){
            return 1;
    }
    return 0;
}

int decideAuctionBid(Player *player, Property *property, int nextBid){

    if(player->isBankrupt == 1){
        return 0;
    }
    if(player->cash < nextBid){
        return 0;
    }
    
    if(nextBid > property->marketValue){
        return 0;
    }

    return 1;
}

int decidePayBail(Player *player){

    if(player == NULL){
        return 0;
    }

    // Temporary general decision.
    // Strategy-specific behaviour will be implemented later.
    return 0;
}
