#include <stdio.h>
#include <string.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"
#include "finance.h"

void handleLanding(Player players[], Player *player, int playerIndex, Property board[], int diceTotal){

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
                handlePropertyPurchase(players, playerIndex, board, pos);
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
            printf("%s landed on Tax: %s (would pay %d - not yet implemented)\n",
                   player->name, square->name, square->taxAmount);
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
}

void handleLandingTest(Player players[], int testPostions[], int numTests, Player *player, Property board[]){

    printf("---Handle landing tests---\n");

    for(int i = 0; i < numTests; i++){
        player->position = testPostions[i];
        handleLanding(players, player, 0, board, 0);
    }

}

void testTurnRotation(Player players[], int numPlayers, Property board[], int targetCompletedRounds, int turnOrder[], GameState *game){
    
    printf("--- Turn Rotation Test: %d players, %d rounds ---\n", numPlayers, targetCompletedRounds);

    while(game->completedRounds < targetCompletedRounds){

        for (int i = 0; i < numPlayers; i++) {

            int playerIndex = turnOrder[i];
            Player *player = &players[playerIndex];
            
            if (player->isBankrupt == 1) {
                continue;
            }

            if(player->isInJail  == 1){
                printf("%s is in jail and skips movement\n",player->name);
            
                updateCompletedRounds(game, players, numPlayers);
                
                continue;
            }


            DiceRoll roll = rollDice();
            int total = roll.die1 + roll.die2;

            printf("\n%s rolled %d + %d = %d\n", player->name, roll.die1, roll.die2, total);

            movePlayer(player, total);

            handleLanding(players, player, playerIndex, board, total);   

            updateCompletedRounds(game, players, numPlayers);
            
              if (game->completedRounds >= targetCompletedRounds) {
                break;
              }
        }
    }
    
    printf("\nTurn rotation test finished.\n");
    printf("Completed global rounds: %d\n",
           game->completedRounds);
}

void handlePropertyPurchase(Player players[], int playerIndex, Property board[], int squareIndex){

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
        printf("%s declined to buy %s (cash: %d, price: %d)\n",
               player->name, property->name, player->cash, property->purchasePrice);
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
    }

    if(addPropertyToPlayer(player, squareIndex) == 0){
        return 0;
    }

    property->owner = playerIndex;

    return 1;
}


int calculateCompletedRounds(
    Player players[],
    int numPlayers,
    int currentCompletedRounds
)
{
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

void updateCompletedRounds(
    GameState *game,
    Player players[],
    int numPlayers
)

{
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

