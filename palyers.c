#include "types.h"
#include <stdio.h>
#include <string.h>


void initPlayer(Player *player, const char *name, PlayerStrategy strategy){
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';

    player->strategy = strategy;
    player->cash = STARTING_CASH;
    player->position = 0;
    player->isInJail = 0;
    player->jailTurnsRemaining = 0;
    player->isBankrupt = 0;
    player->numOwnedProperties = 0;

    for(int i = 0; i < BOARD_SIZE; i++){
        player->ownedProperties[i] = 0;
    }

}

void movePlayer(Player *player, int diceTotal){

    int rawNewPosition = player->position + diceTotal;
    int passedGo = (rawNewPosition >= 40); 
    int newPosition = rawNewPosition % 40;

    player->position = newPosition;

    printf("%s moved to position %d\n", player->name, player->position);

    if(passedGo == 1){
        player->cash = player->cash + GO_MONEY;
        printf("Player cash = %d", player->cash);
    }

}