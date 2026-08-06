#include <stdio.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"
#include "finance.h"

int calculateRent(Property board[], int squareIndex, int diceTotal){
    //this function will only be called by owned by other section in handleLanding function. therfore no need for a isowned? check.
    
    Property *property = &board[squareIndex];
    int multiplier = 1;
    int rent = 0;

    //if the property is mortgaged no need for a rent calculation
    if(property->isMortgaged == 0){
        
        //rent calculation for properties
        if(property->type == SQUARE_PROPERTY) {
        
            if(property->hasHotel == 1){
                multiplier = 10;
            }
            else{
                switch (property->numHouses)
                {
                case 0:
                    multiplier = 1;
                    break;
                case 1:
                    multiplier = 2;
                    break;
                case 2:
                    multiplier = 3;
                    break;
                case 3:
                    multiplier = 5;
                    break;
                case 4:
                    multiplier = 7;
                    break;
                default:
                    printf("WARNING: invalid numHouses value %d for %s\n", property->numHouses, property->name);
                    multiplier = 1;
                    break;      
                }

            }

            rent = property->baseRent * multiplier;
        
            if (property->conditionRating >= 90) {
                rent = (rent * 100) / 100;
            } 
            else if (property->conditionRating >= 75) {
                rent = (rent * 90) / 100;
            } 
            else if (property->conditionRating >= 50) {
                rent = (rent * 75) / 100;
            } 
            else if (property->conditionRating >= 25) {
                rent = (rent * 50) / 100;
            } 
            else {
                rent = 0;
            }
        }

        //rent calculation for railwayStations
        else if(property->type == SQUARE_RAILWAY){

            int railwayCount = 0;

            for(int i = 0; i < BOARD_SIZE; i++){
                if(board[i].type == SQUARE_RAILWAY && board[i].owner == property->owner){
                    railwayCount++;
                }
            }

            switch (railwayCount)
            {
            case 1:
                rent = 250;
                break;
            case 2:
                rent = 500;
                break;
            case 3:
                rent = 1000;
                break;
            case 4:
                rent = 2000;
                break;
             default:
                    printf("WARNING: invalid railwayCount value %d\n", railwayCount);
                    rent = 0;
                    break;      
            }
            
        }
        
        //rent calculation for utility
        else if(property->type == SQUARE_UTILITY){

            int utilityCount = 0;

            for(int i = 0; i < BOARD_SIZE; i++){
                if(board[i].type == SQUARE_UTILITY && board[i].owner == property->owner){
                    utilityCount++;
                }
            }
            
            if(utilityCount == 1){
                rent = diceTotal * 4;
            }
            else if(utilityCount == 2){
                rent = diceTotal * 10;
            }
            else{
                printf("WARNING: invalid utilityCount value: %d\n", utilityCount);
            }
        }
    }
    return rent;
}

void payRent(Player players[], int playerIndex, Property board[], int squareIndex, int diceTotal){

    Property *property = &board[squareIndex];
    Player *player = &players[playerIndex];
    Player *owner = &players[property->owner];

    int rent = calculateRent(board, squareIndex, diceTotal);

    if(rent <= player->cash){
        player->cash -= rent;
        owner->cash += rent;
        printf("%s landed on property owned by %s, rent amount %d was paid to the owner\n", player->name, owner->name, rent);
    }
    else{
        int availableCash = player->cash;
        player->cash = 0;
        owner->cash += availableCash;
        
        printf("%s landed on property owned by %s, not enough money to pay the rent amount %d, transferred remaining %d instead\n",
               player->name, owner->name, rent, availableCash);
        printf("This is partial/incomplete banking logic\n");
    }
}