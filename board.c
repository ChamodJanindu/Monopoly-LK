#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "board.h"


static void setSpecialSquare(Property board[], int index, const char *name, SquareType type){
    strcpy(board[index].name, name);
    board[index].type = type;
    board[index].group = GROUP_NONE;
}

static void setPropertySquare(Property board[], int index, const char *name, PropertyGroup group, int purchasePrice, int mortgageValue, int baseRent, int houseCost, int hotelCost){
    strcpy(board[index].name, name);
    board[index].type = SQUARE_PROPERTY;
    board[index].group = group;
    board[index].purchasePrice = purchasePrice;
    board[index].mortgageValue = mortgageValue;
    board[index].baseRent = baseRent;
    board[index].houseCost = houseCost;
    board[index].hotelCost = hotelCost;

    board[index].owner = -1;
    board[index].conditionRating = 100;
}

static void setRailwaySquare(Property board[], int index, const char *name) {
    strcpy(board[index].name, name);
    board[index].type = SQUARE_RAILWAY;
    board[index].group = GROUP_NONE;
    board[index].owner = -1;
}

static void setUtilitySquare(Property board[], int index, const char *name) {
    strcpy(board[index].name, name);
    board[index].type = SQUARE_UTILITY;
    board[index].group = GROUP_NONE;
    board[index].owner = -1;
}

static void setTaxSquare(Property board[], int index, const char *name, int taxAmount) {
    strcpy(board[index].name, name);
    board[index].type = SQUARE_TAX;
    board[index].group = GROUP_NONE;
    board[index].taxAmount = taxAmount;
}

void initBoard(Property board[]){

     memset(board, 0, sizeof(Property) * BOARD_SIZE);

    setSpecialSquare(board, 0, "GO", SQUARE_START);
    setPropertySquare(board, 1, "Pettah", GROUP_BROWN, 1500, 750, 150, 500, 2000);
    setSpecialSquare(board, 2, "Community Development", SQUARE_EVENT);
    setPropertySquare(board, 3, "Maradhana", GROUP_BROWN, 1500, 750, 150, 500, 2000);
    setTaxSquare(board, 4, "Income Tax", 200); 
    setRailwaySquare(board, 5, "Colombo Fort Railway Station");
    setPropertySquare(board, 6, "Bambalapitiya", GROUP_LIGHT_BLUE, 2500, 1250, 250, 750, 3000);
    setSpecialSquare(board, 7, "National Event Card", SQUARE_EVENT);
    setPropertySquare(board, 8, "Wellawatte", GROUP_LIGHT_BLUE, 2500, 1250, 250, 750, 3000);
    setPropertySquare(board, 9, "Mount Lavinia", GROUP_LIGHT_BLUE, 2500, 1250, 250, 750, 3000);
    setSpecialSquare(board, 10, "Jail / Just Visiting", SQUARE_JAIL);
    setPropertySquare(board, 11, "Nugegoda", GROUP_PINK, 3500, 1750, 350, 1000, 4000);
    setUtilitySquare(board, 12, "Ceylon Electricity Board (CEB)");
    setPropertySquare(board, 13, "Maharagama", GROUP_PINK, 3500, 1750, 350, 1000, 4000);
    setPropertySquare(board, 14, "Kottawa", GROUP_PINK, 3500, 1750, 350, 1000, 4000);
    setRailwaySquare(board, 15, "Kandy Railway Station");
    setPropertySquare(board, 16, "Negombo", GROUP_ORANGE, 4500, 2250, 450, 1250, 5000);
    setSpecialSquare(board, 17, "Sri Lanka Insurance", SQUARE_INSURANCE);
    setPropertySquare(board, 18, "Katunayake", GROUP_ORANGE, 4500, 2250, 450, 1250, 5000);
    setPropertySquare(board, 19, "Ja-Ela", GROUP_ORANGE, 4500, 2250, 450, 1250, 5000);
    setSpecialSquare(board, 20, "Free Parking", SQUARE_FREE_PARKING);
    setPropertySquare(board, 21, "Kandy City", GROUP_RED, 5500, 2750, 550, 1500, 6000);
    setSpecialSquare(board, 22, "National Event Card", SQUARE_EVENT);
    setPropertySquare(board, 23, "Peradeniya", GROUP_RED, 5500, 2750, 550, 1500, 6000);
    setPropertySquare(board, 24, "Katugastota", GROUP_RED, 5500, 2750, 550, 1500, 6000);
    setRailwaySquare(board, 25, "Galle Railway Station");
    setPropertySquare(board, 26, "Galle Fort", GROUP_YELLOW, 6500, 3250, 650, 2000, 8000);
    setPropertySquare(board, 27, "Unawatuna", GROUP_YELLOW, 6500, 3250, 650, 2000, 8000);
    setUtilitySquare(board, 28, "National Water Supply and Drainage Board (NWSDB)");
    setPropertySquare(board, 29, "Hikkaduwa", GROUP_YELLOW, 6500, 3250, 650, 2000, 8000);
    setSpecialSquare(board, 30, "Go To Jail", SQUARE_GO_TO_JAIL);
    setPropertySquare(board, 31, "Jaffna Town", GROUP_GREEN, 8000, 4000, 800, 2500, 10000);
    setPropertySquare(board, 32, "Nallur", GROUP_GREEN, 8000, 4000, 800, 2500, 10000);
    setSpecialSquare(board, 33, "Ceylinco Insurance", SQUARE_INSURANCE);
    setPropertySquare(board, 34, "Trincomalee", GROUP_GREEN, 8000, 4000, 800, 2500, 10000);
    setRailwaySquare(board, 35, "Jaffna Railway Station");
    setSpecialSquare(board, 36, "National Event Card", SQUARE_EVENT);
    setPropertySquare(board, 37, "Nuwara Eliya", GROUP_DARK_BLUE, 10000, 5000, 1000, 3000, 12000);
    setSpecialSquare(board, 38, "Bank of Ceylon", SQUARE_BANK);
    setPropertySquare(board, 39, "Galle Face", GROUP_DARK_BLUE, 10000, 5000, 1000, 3000, 12000);
}


void printBoard(Property board[]) {
    const char *typeNames[] = {
        "START", "PROPERTY", "EVENT", "TAX", "RAILWAY",
        "UTILITY", "JAIL", "INSURANCE", "FREE_PARKING",
        "GO_TO_JAIL", "BANK"
    };
for (int i = 0; i < BOARD_SIZE; i++) {
        printf("[%2d] %-35s type=%-12s price=%-6d rent=%-6d owner=%d\n",
               i,
               board[i].name,
               typeNames[board[i].type],
               board[i].purchasePrice,
               board[i].baseRent,
               board[i].owner);
    }
}



DiceRoll rollDice(void){
    DiceRoll roll;
    roll.die1 = (rand() % 6) + 1;
    roll.die2 = (rand() % 6) + 1;
    return roll;
}

int isDoubles(DiceRoll roll){
    return roll.die1 == roll.die2;
} 

void printDie(void){

    DiceRoll roll;
    int isDouble;

    printf("--- Print Dice Rolls---\n");
    
    for(int i = 0; i < 20; i++){
        roll = rollDice();
        isDouble = isDoubles(roll);

        int total = roll.die1 + roll.die2;
        printf("Die1 = %d\t Die2 = %d\t Total = %d\t Is doubles? = %s \n",roll.die1, roll.die2, total, isDouble ? "YES" : "NO");
    }
}