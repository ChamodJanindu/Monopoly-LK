#include <stdio.h>
#include <string.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"

void handleLanding(Player *player, Property board[]){

    int pos = player->position;
    Property *square = &board[pos];
   
    const char *groupNames[] = {
    "Brown", "Light Blue", "Pink", "Orange",
    "Red", "Yellow", "Green", "Dark Blue", "None"

};

    switch (square->type)
    {
        case SQUARE_START:
            printf("%s Landed on Go\n", player->name);
            break;
    
        case SQUARE_PROPERTY:
            printf("%s Landed on property: %s, (Gruop: %s, Price: %d, Owner: %d)\n",
                player->name,
                square->name,
                groupNames[square->group],
                square->purchasePrice,
                square->owner        
            );
            break;
     
        case SQUARE_EVENT:
            printf("%s Landed on an EVENT SQUARE: %s, draw a card\n", 
                player->name,
                square->name
            );
            break;
    
        case SQUARE_TAX:
            printf("%s landed on Tax: %s (would pay %d - not yet implemented)\n",
                   player->name, square->name, square->taxAmount);
            break;

        case SQUARE_RAILWAY:
            printf("%s landed on Railway: %s (Owner=%d - rent calc not yet implemented)\n",
                   player->name, square->name, square->owner);
            break;

        case SQUARE_UTILITY:
            printf("%s landed on Utility: %s (Owner=%d - rent calc not yet implemented)\n",
                   player->name, square->name, square->owner);
            break;

        case SQUARE_JAIL:
            printf("%s landed on Jail / Just Visiting.\n", player->name);
            break;

        case SQUARE_INSURANCE:
            printf("%s landed on Insurance office: %s (not yet implemented)\n",
                   player->name, square->name);
            break;

        case SQUARE_FREE_PARKING:
            printf("%s landed on Free Parking. Nothing happens.\n", player->name);
            break;

        case SQUARE_GO_TO_JAIL:
            printf("%s landed on Go To Jail (send-to-jail logic not yet implemented)\n",
                   player->name);
            break;

        case SQUARE_BANK:
            printf("%s landed on Bank of Ceylon: %s (loan logic not yet implemented)\n",
                   player->name, square->name);
            break;

        default:
            printf("%s landed on an unrecognized square type at position %d!\n",
                   player->name, pos);
            break;
    }
}
    

void handleLandingTest(int testPostions[], int numTests, Player *player, Property board[]){

    printf("---Handle landing tests---\n");

    for(int i = 0; i<numTests; i++){
        player->position = testPostions[i];
        handleLanding(player,board);
    }

}

void testing001(Player *player, Property board[]){

    printf("--movePlayer + handleLanding + rollDice Testing--\n");

    for(int i = 0; i < 20; i++){
        DiceRoll roll = rollDice();
        int total = roll.die1 + roll.die2;
        
        movePlayer(player, total);
        handleLanding(player, board);
    }

}