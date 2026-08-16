#include <stdio.h>
#include <string.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"
#include "finance.h"
#include "events.h"

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
                handlePropertyPurchase(players, playerIndex, board, pos, turnOrder, game);
            }

            else if(square->owner == playerIndex){

                printf("%s landed on their own property.\n",
                    player->name);

                if(decidePropertyRenovation(player, square) == 1){

                    renovateProperty(players, playerIndex, board, pos, game);
                }
            }

            else {
                payRent(players, playerIndex, board, pos, diceTotal, game);
            }
            break;
     
        case SQUARE_EVENT:
            printf("%s Landed on an EVENT SQUARE: %s, draw a card\n", 
                player->name,
                square->name
            );
            handleNationalEventCard(players, playerIndex, board, game);
            break;
    
        case SQUARE_TAX:

            printf("%s landed on Tax: %s.\n",
                player->name,
                square->name);

            payTax(players, playerIndex, square, board, game);

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
        
        case SQUARE_COMMUNITY_FUND:

            printf("%s landed on Community Development Fund.\n",
                player->name);

            payCommunityFundTax(players, playerIndex, board, game);

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

    game->currentInflationRate = 0;

    game->rentInflationIndex = 100;

    game->boomGroup = GROUP_NONE;
    game->declineGroup = GROUP_NONE;

    game->boomRoundsRemaining = 0;
    game->declineRoundsRemaining = 0;

    game->activeEconomicEvent = ECONOMIC_EVENT_NONE;
    game->economicEventRoundsRemaining = 0;

    game->activeRegulation = REGULATION_NONE;
    game->regulationRoundsRemaining = 0;

    game->activeRegionalEvent = REGIONAL_EVENT_NONE;
    game->regionalEventRoundsRemaining = 0;

    for(int i = 0; i < 8; i++){
        game->lastMarketAffectedRound[i] = -30;
    }

    initNationalEventDeck(game);
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

    //why before 'is in jail?' - jailed players should still be able to maintain property.
    handleMaintenancePhase(players, playerIndex, board);

    if(player->isInJail == 1){

        int jailResult = handleJailTurn(players, playerIndex, board, turnOrder, game);

        if(jailResult == 0){
            return;
        }

        if(jailResult == 2){
            return;
        }

        //if the player completes a lap imidiately after getting out of jail the loan needs to get updated.
        if(player->lapCount > lapsBefore){

            updateLoanAfterRound(player);

            if(player->loan.isActive == 1 && player->loan.roundsRemaining <= 0){

                handleLoanDefault(players, playerIndex, board, turnOrder,game);
            }
        }

        if(player->isInJail == 0 && player->isBankrupt == 0){

            handleConstructionPhase(players, playerIndex, board, game);
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

             handleLoanDefault(players, playerIndex, board, turnOrder, game);
        }
    }

    //if player gets send to jail during this turn
    if(player->isInJail == 1){
        return;
    } 


    handleConstructionPhase(players, playerIndex, board, game);
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
        
        updateInflation(board, game);

        if(game->completedRounds % 10 == 0){
            triggerRandomDisaster(players, board);
        }
                
        updateEconomicEvent(game);

        if(game->completedRounds % 15 == 0){
            startEconomicEvent(game);
        }

        updateGovernmentRegulation(game);

        if(game->completedRounds % 20 == 0){
            startGovernmentRegulation(game);
        }

        updateMarketConditions(game);

        if(game->completedRounds % 10 == 0){
            startMarketReview(game);
        }

        updateRegionalEvent(game);

        if(game->completedRounds % 15 == 0){
            startRegionalEvent(game);
        }

        checkDevelopmentDeadlines(players, board, game);

        updateInsuranceAfterRound(players, board, game);
        checkAutomaticRepairs(players, board);

        updatePropertyAges(board);
        updatePropertyDepreciation(board);
        updateBuildingConditions(board);
        
        updateNationalCardEffects(players);
        updateNationalEventProperties(board);
        displayCurrentMarketConditions(game);
    }
}


void handlePropertyPurchase(Player players[], int playerIndex, Property board[], int squareIndex, int turnOrder[], GameState *game){

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];

    int purchasePrice = calculateCurrentPurchasePrice(property, game);
    
    int developmentRequired = requiresImmediateDevelopment(player, board, game);


    if(decidePurchase(player, property, board, purchasePrice, game) == 1 && player->cash >= purchasePrice){
        
        if(assignPropertyToPlayer(players, playerIndex, board, squareIndex) == 0){
            printf("Purchase failed for %s.\n", property->name);
            return;
        }

        player->cash -= purchasePrice;

        if(developmentRequired == 1){

            property->developmentDeadlineRound =
                game->completedRounds + 5;

            printf("%s must develop %s by Round %d due to the Anti-Speculation Act.\n",
                player->name,
                property->name,
                property->developmentDeadlineRound);
        }
            
        printf("%s bought %s for %d. remaining cash %d\n",
                player->name, property->name, purchasePrice, player->cash);
    }
    
    else {
        printf("%s declined to buy %s (cash: %d, price: %d)\n", player->name, property->name, player->cash, property->purchasePrice);
        
        handleAuction(players, board, squareIndex, turnOrder, game);
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

void handleAuction(Player players[], Property board[], int squareIndex, int turnOrder[], GameState *game){

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

    //to apply all types of eventvaluemodifiers
    int currentValue = calculateAuctionMarketValue(property, game);
    int openingBid = currentValue / 2;
    openingBid = applyMarketAuctionModifier(property, openingBid, game);

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
    printf("Current market value: LKR %d\n", currentValue);
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
            int wantsToBid = decideAuctionBid(&players[playerIndex], property, nextBid, currentValue);
            
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

int getDevelopmentLevel(Property *property){

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

int buildHouse(Player players[], int playerIndex, Property board[], int squareIndex, GameState *game){

    if(canBuildHouse(board, playerIndex, squareIndex) == 0){
        return 0;
    }

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];

    int constructionCost = calculateCurrentHouseCost(player, property, game);
    
    if(player->cash < constructionCost){

        printf("%s does not have enough cash to build a house on %s.\n",
            player->name,
            property->name);
        
        return 0;
    }

    player->cash -= constructionCost;

    int newHouseIndex = property->numHouses;

    property->houses[newHouseIndex].conditionRating = 100;
    property->houses[newHouseIndex].roundsNeglected = 0;
    property->houses[newHouseIndex].isStructurallyDamaged = 0;

    property->numHouses++;

    //event(anti speculation act) - to reset the development round deadline after building a property
    property->developmentDeadlineRound = -1;

    printf("%s built a house on %s for LKR %d.\n",
       player->name,
       property->name,
       constructionCost);

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

int buildHotel(Player players[], int playerIndex, Property board[], int squareIndex, GameState *game){

    if(canBuildHotel(board, playerIndex, squareIndex) == 0){
            return 0;
        }

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];
    
    int constructionCost = calculateCurrentHotelCost(player, property, game);
    
    if(player->cash < constructionCost){

        printf("%s does not have enough cash to build a hotel on %s.\n",
            player->name,
            property->name);

        return 0;
    }

    player->cash -= constructionCost;

    property->numHouses = 0;
    property->hasHotel = 1;

    //event(anti speculation act) - to reset the development round deadline after building a property
    property->developmentDeadlineRound = -1;

    //when houses are replaced by a hotel the condition rating should go back to full cause its literally a new structure.
    property->hotel.conditionRating = 100;
    property->hotel.roundsNeglected = 0;
    property->hotel.isStructurallyDamaged = 0;

    for(int i = 0; i < 4; i++){

        property->houses[i].conditionRating = 100;
        property->houses[i].roundsNeglected = 0;
        property->houses[i].isStructurallyDamaged = 0;
    }   

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

void handleConstructionPhase(Player players[], int playerIndex, Property board[], GameState *game){

    Player *player = &players[playerIndex];

    if(canConstructWithNationalCards(player) == 0){

        printf("%s cannot construct buildings because of the Labour Strike.\n",
            player->name);

        return;
    }

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];
        Property *property =  &board[squareIndex];
        
        if(canBuildHotel(board, playerIndex, squareIndex) == 1){

            int constructionCost =
                calculateCurrentHotelCost(player, property, game);

            if(decideBuildHotel(player, property, constructionCost, game) == 1){

                buildHotel(players, playerIndex, board, squareIndex, game);

                continue;
            }
        }
    
        if(canBuildHouse(board, playerIndex, squareIndex) == 1){

            int constructionCost =
                calculateCurrentHouseCost(player, property, game);

            if(decideBuildHouse(player, property, constructionCost, game) == 1){

                buildHouse(players, playerIndex, board, squareIndex, game);
            }
        }
    }
}

void handleMaintenancePhase(Player players[], int playerIndex, Property board[]){

    Player *player = &players[playerIndex];

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];
        Property *property = &board[squareIndex];

        if(property->type != SQUARE_PROPERTY){
            continue;
        }

        if(property->hasHotel == 1){

            BuildingCondition *hotel = &property->hotel;

            if(hotel->isStructurallyDamaged == 1){

                renovateHotelStructuralDamage(player, property);
            }
            else if(decideMaintainBuilding(player, hotel) == 1){

                maintainHotel(player, property);
            }

            continue;
        }

        for(int j = 0; j < property->numHouses; j++){

            BuildingCondition *house = &property->houses[j];

            if(house->isStructurallyDamaged == 1){

                renovateHouseStructuralDamage(player, property, j);
            }
            else if(decideMaintainBuilding(player, house) == 1){

                maintainHouse(player, property, j);
            }
        }
    }
}
