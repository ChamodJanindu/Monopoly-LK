#include <stdio.h>
#include <string.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"
#include "finance.h"

void handleLanding(Player players[], int playerIndex, Property board[], int diceTotal, int turnOrder[], GameState *game){

    Player *player = &players[playerIndex];
    int pos = player->position;
    Property *square = &board[pos];

    switch (square->type)
    {
        case SQUARE_START:
            printf("%s Landed on Go\n", player->name);
            break;
    
        case SQUARE_PROPERTY:
        case SQUARE_RAILWAY:
        case SQUARE_UTILITY:
            
            printf("%s Landed on property: %s, (Price: %d, Owner: %d)\n",
                player->name,
                square->name,
                square->purchasePrice,
                square->owner 
            );       
            
            if(square->owner == -1){
                handlePropertyPurchase(players, playerIndex, board, pos, turnOrder);
            }
            else if(square->owner == playerIndex){
                printf("Already owns this, nothing happens\n");
            }
            else {
                payRent(players, playerIndex, board, pos, diceTotal);
            }
            break;
     
        case SQUARE_EVENT:
            printf("%s Landed on an EVENT SQUARE: %s, draw a card\n", 
                player->name,
                square->name
            );
            break;
    
        case SQUARE_TAX:

            printf("%s landed on Tax: %s.\n",
                player->name,
                square->name);

            payTax(player, square);

            break;

        case SQUARE_JAIL:
            printf("%s landed on Jail / Just Visiting.\n", player->name);
            break;

        case SQUARE_INSURANCE:
            printf("%s landed on Insurance office: %s.\n",
            player->name,
            square->name);

            handleInsurance(players, playerIndex, board, pos, game);
            break;

        case SQUARE_FREE_PARKING:
            printf("%s landed on Free Parking. Nothing happens.\n", player->name);
            break;

        case SQUARE_GO_TO_JAIL:
            printf("%s landed on Go To Jail\n",
                   player->name);
            sendPlayerToJail(player);
            break;

        case SQUARE_BANK:
            
            handleBank(players, playerIndex, board, game);
            break;

        default:
            printf("%s landed on an unrecognized square type at position %d!\n",
                   player->name, pos);
            break;
    }
}

void determineTurnOrder(Player players[], int turnOrder[]){

    typedef struct {
        int playerIndex;
        int total;
    } Rolls;

    Rolls rolls[NUM_PLAYERS];

    for(int i = 0; i < NUM_PLAYERS; i++){
        DiceRoll roll = rollDice();
        int total = roll.die1 + roll.die2;

        rolls[i].playerIndex = i;
        rolls[i].total = total;

        printf("%s rolled %d\n", players[i].name, total);
    }

    int slot = 0;

    while (slot < NUM_PLAYERS) {

        // find the highest remaining total
        int bestTotal = -1;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (rolls[i].total > bestTotal) {
                bestTotal = rolls[i].total;
            }
        }

        // collect every remaining entry that shares this best total
        int groupPositions[NUM_PLAYERS];
        int groupCount = 0;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (rolls[i].total == bestTotal) {
                groupPositions[groupCount] = i;
                groupCount++;
            }
        }

        if (groupCount == 1) {
            // no tie - place directly, same as the original logic
            int pos = groupPositions[0];
            turnOrder[slot] = rolls[pos].playerIndex;
            rolls[pos].total = -1;
            slot++;

        } else {
            // tie among groupCount players
            printf("Tie detected among %d players at total %d - re-rolling...\n", groupCount, bestTotal);

            int allDistinct = 0;
            while (!allDistinct) {

                for (int g = 0; g < groupCount; g++) {
                    int pos = groupPositions[g];
                    DiceRoll roll = rollDice();
                    int total = roll.die1 + roll.die2;
                    rolls[pos].total = total;
                    printf("%s re-rolled %d\n", players[rolls[pos].playerIndex].name, total);
                }

                allDistinct = 1;
                for (int a = 0; a < groupCount && allDistinct; a++) {
                    for (int b = a + 1; b < groupCount; b++) {
                        if (rolls[groupPositions[a]].total == rolls[groupPositions[b]].total) {
                            allDistinct = 0;
                            break;
                        }
                    }
                }
            }

            // sort just this resolved group and splice into turnOrder
            for (int s = 0; s < groupCount; s++) {
                int bestPos = -1;
                int bestGroupTotal = -1;

                for (int g = 0; g < groupCount; g++) {
                    int pos = groupPositions[g];
                    if (rolls[pos].total > bestGroupTotal) {
                        bestGroupTotal = rolls[pos].total;
                        bestPos = pos;
                    }
                }

                turnOrder[slot] = rolls[bestPos].playerIndex;
                rolls[bestPos].total = -1;
                slot++;
            }
        }
    }

    printf("Turn order (first to last): ");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf("%s ", players[turnOrder[i]].name);
    }
    printf("\n");
}  
    
void initGameState(GameState *game){
    game->completedRounds = 0;
    game->gameOver = 0;

    game->currentLoanInterestRate = 8;
}

void testTurnRotation(Player players[], int numPlayers, Property board[], int targetCompletedRounds, int turnOrder[], GameState *game){

    printf("--- Turn Rotation Test: %d players, %d rounds ---\n",
           numPlayers,
           targetCompletedRounds);

    while(game->completedRounds < targetCompletedRounds){

        for(int i = 0; i < numPlayers; i++){

            int playerIndex = turnOrder[i];

            playTurn(players, playerIndex, board, turnOrder, game);

            updateCompletedRounds(game, players, numPlayers, board);

            if(game->completedRounds >= targetCompletedRounds){
                break;
            }
        }
    }
}



void playTurn(Player players[], int playerIndex, Property board[], int turnOrder[], GameState *game){

    Player *player = &players[playerIndex];

    int lapsBefore = player->lapCount;

    printf("\n---------------------------------------------\n");
    printf("%s's TURN\n", player->name);
    printf("---------------------------------------------\n");

    if(player->isBankrupt == 1){

        printf("%s is bankrupt and cannot take a turn.\n",
               player->name);

        return;
    }

    if(player->isInJail == 1){

        int jailResult = handleJailTurn(players, playerIndex, board, turnOrder, game);

        if(jailResult == 0){
            return;
        }

        if(jailResult == 2){
            return;
        }

        if(player->lapCount > lapsBefore){

            updateLoanAfterRound(player);

            if(player->loan.isActive == 1 && player->loan.roundsRemaining <= 0){

                handleLoanDefault(players, playerIndex, board, turnOrder);
            }
        }

        if(player->isInJail == 0 && player->isBankrupt == 0){

            handleConstructionPhase(players, playerIndex, board);
        }

        return;
        
    }

    DiceRoll roll = rollDice();

    int total = roll.die1 + roll.die2;

    printf("%s rolled %d + %d = %d\n",
           player->name,
           roll.die1,
           roll.die2,
           total);

    movePlayer(player, total);

    handleLanding(players, playerIndex, board, total, turnOrder, game);

    //Player based things that needs to get updated based on individual rounds
    if(player->lapCount > lapsBefore){
        
        updateLoanAfterRound(player);

        if(player->loan.isActive == 1 && player->loan.roundsRemaining <= 0){

             handleLoanDefault(players, playerIndex, board, turnOrder);
        }
    }

    //if player gets send to jail during this turn
    if(player->isInJail == 1){
        return;
    } 


    handleConstructionPhase(players, playerIndex, board);
}

void updateCompletedRounds(GameState *game, Player players[], int numPlayers, Property board[]){
    int newCompletedRounds = calculateCompletedRounds(
        players,
        numPlayers,
        game->completedRounds
    );

    while (game->completedRounds < newCompletedRounds) {

        game->completedRounds++;

        printf("\n=============================================\n");
        printf("GLOBAL ROUND %d COMPLETED\n",
               game->completedRounds);
        printf("=============================================\n");
        

        updateInsuranceAfterRound(players, board, game);
        checkAutomaticRepairs(players, board);
        /*
         * Later, end-of-round systems will run here:
         *
         * applyLoanInterest();
         * updateInsuranceExpiry();
         * updatePropertyAge();
         * reduceBuildingCondition();
         * processInflation();
         * processEconomicEvents();
         * processGovernmentRegulations();
         * printRoundSummary();
         */
    }
}


void handlePropertyPurchase(Player players[], int playerIndex, Property board[], int squareIndex, int turnOrder[]){

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];


    if(decidePurchase(player, property) == 1 && player->cash >= property->purchasePrice){
        
        if(assignPropertyToPlayer(players, playerIndex, board, squareIndex) == 0){
            printf("Purchase failed for %s.\n", property->name);
            return;
        }

        player->cash -= property->purchasePrice;
    
        printf("%s bought %s for %d. remaining cash %d\n",
                player->name, property->name, property->purchasePrice, player->cash);
    }
    else {
        printf("%s declined to buy %s (cash: %d, price: %d)\n", player->name, property->name, player->cash, property->purchasePrice);
        
        handleAuction(players, board, squareIndex, turnOrder);
    }

}

int assignPropertyToPlayer(Player players[], int playerIndex, Property board[], int squareIndex){

    if (playerIndex < 0 || playerIndex >= NUM_PLAYERS) {
        printf("ERROR: Invalid player index %d.\n", playerIndex);

        return 0;
    }

    if (squareIndex < 0 || squareIndex >= BOARD_SIZE) {
        printf("ERROR: Invalid square index %d.\n", squareIndex);

        return 0;
    }

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];

    if (property->owner != -1) {
        printf("ERROR: %s already has an owner.\n", property->name);

        return 0;
    }

    if(addPropertyToPlayer(player, squareIndex) == 0){
        return 0;
    }

    property->owner = playerIndex;

    return 1;
}

int calculateCompletedRounds(Player players[], int numPlayers, int currentCompletedRounds){
    int minimumLapCount = -1;

    for (int i = 0; i < numPlayers; i++) {

        /* Bankrupt players no longer affect round completion. */
        if (players[i].isBankrupt == 1) {
            continue;
        }

        /* Jailed players are temporarily excluded. */
        if (players[i].isInJail == 1) {
            continue;
        }

        if (minimumLapCount == -1 ||
            players[i].lapCount < minimumLapCount) {

            minimumLapCount = players[i].lapCount;
        }
    }

    /*
     * If every remaining player is jailed,
     * the global round remains unchanged.
     */
    if (minimumLapCount == -1) {
        return currentCompletedRounds;
    }

    return minimumLapCount;
}

void handleAuction(Player players[], Property board[], int squareIndex, int turnOrder[]){

    if(squareIndex < 0 || squareIndex >= BOARD_SIZE){
        printf("ERROR: Invalid auction square index %d.\n", squareIndex);
        return;
    } 

    Property *property = &board[squareIndex];

    if(property->isLoanLocked == 1){

    printf("%s is Loan Locked and cannot be auctioned.\n",
           property->name);

    return;
    }

    if(property->type != SQUARE_PROPERTY && property->type != SQUARE_RAILWAY && property->type != SQUARE_UTILITY){
        printf("ERROR: %s cannot be auctioned.\n", property->name);
        return;
    }

    if(property->owner != -1){
        printf("ERROR: %s already has an owner.\n", property->name);
        return;
    }

    int openingBid = property->marketValue / 2;
    int currentBid = openingBid - AUCTION_INCREMENT;
    int highestBidder = -1;

    int withdrawn[NUM_PLAYERS] = {0};
    int activeBidders = 0;

    for(int i = 0; i < NUM_PLAYERS; i++){

        if(players[i].isBankrupt == 1){
            withdrawn[i] = 1;
        }
        else{
            activeBidders++;
        }
    }

    if(activeBidders == 0){
        printf("No solvent players are available for the auction.\n");
        return;
    }

    printf("\nAuction started for %s.\n", property->name);
    printf("Market value: LKR %d\n", property->marketValue);
    printf("Opening bid: LKR %d\n", openingBid);
    printf("Active bidders: %d\n", activeBidders);

    printf("Current bid before first bid: LKR %d\n", currentBid);
    printf("Highest bidder index: %d\n", highestBidder);

    int auctionFinished = 0;

    while(auctionFinished == 0){

        for(int i = 0; i < NUM_PLAYERS; i++){

            int playerIndex = turnOrder[i];

            if(withdrawn[playerIndex] == 1){
                continue;
            }
            
            if(playerIndex == highestBidder){
                continue;
            }
            
            int nextBid = currentBid + AUCTION_INCREMENT;
            int wantsToBid = decideAuctionBid(&players[playerIndex], property, nextBid);
            
            if(wantsToBid == 1){
                currentBid = nextBid;
                highestBidder = playerIndex;
            
                printf("%s bids LKR %d.\n", players[playerIndex].name, currentBid);
            }
             else{
                withdrawn[playerIndex] = 1;
                activeBidders--;

                printf("%s withdraws from the auction.\n", players[playerIndex].name);
            }
            
            if(activeBidders == 0){
                auctionFinished = 1;
                break;
            }

            if(activeBidders == 1 && highestBidder != -1){
                auctionFinished = 1;
                break;
            }
        }

    }

    if(highestBidder == -1){
        printf("No bids were placed for %s.\n", property->name);
        printf("%s remains owned by the bank.\n", property->name);
        return;
    }

    if(players[highestBidder].cash < currentBid){
        printf("ERROR: %s cannot afford the winning bid of LKR %d.\n", players[highestBidder].name, currentBid);
        return;  
    }

    if(assignPropertyToPlayer(players, highestBidder, board, squareIndex) == 0){
        printf("Auction ownership transfer failed! for %s. \n", property->name);
        return;
    }
    
    players[highestBidder].cash -= currentBid;
    
    printf("\n%s wins the auction for %s.\n", players[highestBidder].name, property->name);
    printf("Winning bid: LKR %d\n", currentBid);
    printf("%s remaining cash: LKR %d\n", players[highestBidder].name, players[highestBidder].cash);
}

void sendPlayerToJail(Player *player){

    player->position = 10;
    player->isInJail = 1;
    player->jailTurnsRemaining = 3;

    printf("%s was sent to Jail.\n", player->name);
    printf("%s is now at position %d.\n", player->name, player->position);
}


int handleJailTurn(Player players[], int playerIndex, Property board[], int turnOrder[], GameState *game){

    Player *player = &players[playerIndex];
    
    printf("%s is currently in Jail.\n", player->name);
    printf("Jail turns remaining: %d\n", player->jailTurnsRemaining);

    if(decidePayBail(player) ==  1 && player->cash >= JAIL_BAIL){

        player->cash -= JAIL_BAIL;
        player->isInJail = 0;
        player->jailTurnsRemaining = 0;

        printf("%s paid LKR %d bail and left Jail.\n",
               player->name,
               JAIL_BAIL);

        printf("Remaining cash: LKR %d\n", player->cash);

        DiceRoll roll = rollDice();
        int total = roll.die1 + roll.die2;

        printf("%s rolled %d + %d = %d\n",
               player->name,
               roll.die1,
               roll.die2,
               total);

        movePlayer(player, total);

        handleLanding(players, playerIndex, board, total, turnOrder, game);

        return 1;
    }

    DiceRoll roll = rollDice();
    int total = roll.die1 + roll.die2;

    if(isDoubles(roll)){

        player->isInJail = 0;
        player->jailTurnsRemaining = 0;

        printf("%s rolled doubles and left Jail.\n",
               player->name);


        movePlayer(player, total);

        handleLanding(players, playerIndex, board, total, turnOrder, game);

        return 1;
    }

    player->jailTurnsRemaining--;

    if(player->jailTurnsRemaining <= 0){

        player->isInJail = 0;
        player->jailTurnsRemaining = 0;

        printf("%s has completed three turns in Jail and is released.\n",
            player->name);

        printf("%s will move normally on their next turn.\n",
            player->name);

        return 2;
    }


        printf("%s did not roll doubles and remains in Jail.\n",
           player->name);

        printf("Jail turns remaining: %d\n",
           player->jailTurnsRemaining);

        return 0;
}


int ownsCompleteGroup(Property board[], int playerIndex, PropertyGroup group){

    int groupPropertyCount = 0;
    int ownedPropertyCount = 0;

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY && board[i].group == group){

            groupPropertyCount++;

            if(board[i].owner == playerIndex){

                ownedPropertyCount++;
            }
        }
    }

    if(groupPropertyCount > 0 && ownedPropertyCount == groupPropertyCount){
        return 1;
    }

    return 0;
}

static int getDevelopmentLevel(Property *property){

    if(property->hasHotel == 1){
        return 5;
    }

    return property->numHouses;
}

int canBuildHouse(Property board[], int playerIndex, int squareIndex){

    if(squareIndex < 0 || squareIndex >= BOARD_SIZE){
        return 0;
    }

    Property *property = &board[squareIndex];

    if(property->type != SQUARE_PROPERTY){
        return 0;
    }

    if(property->owner != playerIndex){
        return 0;
    }

    if(ownsCompleteGroup(board, playerIndex, property->group) == 0){
        return 0;
    }

    if(property->hasHotel == 1){
        return 0;
    }

    if(property->numHouses >=4){
        return 0;
    }

    int propertyLevel = getDevelopmentLevel(property);

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY && 
            board[i].group == property->group){
                
                int otherPropertyLevel = getDevelopmentLevel(&board[i]);
                
                if(otherPropertyLevel < propertyLevel){
                    return 0;
                }
            }
    }

    return 1;
}

int buildHouse(Player players[], int playerIndex, Property board[], int squareIndex){

    if(canBuildHouse(board, playerIndex, squareIndex) == 0){
        return 0;
    }

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];

    if(player->cash < property->houseCost){

        printf("%s does not have enough cash to build a house on %s.\n",
            player->name,
            property->name);
        
        return 0;
    }

    player->cash -= property->houseCost;
    property->numHouses++;

    printf("%s built a house on %s for LKR %d.\n",
        player->name,
        property->name,
        property->houseCost);

    printf("Houses on %s: %d\n",
           property->name,
           property->numHouses);

    printf("Remaining cash: LKR %d\n",
           player->cash);

    return 1;

}

int canBuildHotel(Property board[], int playerIndex, int squareIndex){

    if(squareIndex < 0 || squareIndex >= BOARD_SIZE){
        return 0;
    }

    Property *property = &board[squareIndex];

    if(property->type != SQUARE_PROPERTY){
        return 0;
    }

    if(property->owner != playerIndex){
        return 0;
    }

    if(ownsCompleteGroup(board, playerIndex, property->group) == 0){
        return 0;
    }

    if(property->hasHotel == 1){
        return 0;
    }

    if(property->numHouses != 4){
        return 0;
    }

    int propertyLevel = getDevelopmentLevel(property);

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY &&
           board[i].group == property->group){

            int otherPropertyLevel = getDevelopmentLevel(&board[i]);

            if(otherPropertyLevel < propertyLevel){
                return 0;
            }
        }
    }

    return 1;
}


int buildHotel(Player players[], int playerIndex, Property board[], int squareIndex){

    if(canBuildHotel(board, playerIndex, squareIndex) == 0){
            return 0;
        }

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];
    
    if(player->cash < property->hotelCost){

        printf("%s does not have enough cash to build a hotel on %s.\n",
               player->name,
               property->name);

        return 0;
    }

    player->cash -= property->hotelCost;

    property->numHouses = 0;
    property->hasHotel = 1;

    printf("%s built a hotel on %s for LKR %d.\n",
           player->name,
           property->name,
           property->hotelCost);

    printf("Hotel on %s: YES\n",
           property->name);

    printf("Remaining cash: LKR %d\n",
           player->cash);

    return 1;

}

void handleConstructionPhase(Player players[], int playerIndex, Property board[]){

    Player *player = &players[playerIndex];

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];
        Property *property =  &board[squareIndex];
        
        if(canBuildHotel(board, playerIndex, squareIndex) == 1){

            if(decideBuildHotel(player, property) == 1){

                buildHotel(players, playerIndex, board, squareIndex);

                continue;
            }
        }
    
          if(canBuildHouse(board, playerIndex, squareIndex) == 1){

            if(decideBuildHouse(player, property) == 1){
                
                buildHouse(players, playerIndex, board, squareIndex);
            }
        }
    }
}

