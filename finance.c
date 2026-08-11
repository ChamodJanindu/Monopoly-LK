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

void payTax(Player *player, Property *taxSquare){

    int taxAmount = taxSquare->taxAmount;

    if(player->cash >= taxAmount){

        player->cash -= taxAmount;

        printf("%s paid %s of LKR %d.\n",
               player->name,
               taxSquare->name,
               taxAmount);

        printf("Remaining cash: LKR %d\n", player->cash);
    }

    else{

        int availableCash = player->cash;
        int unpaidAmount = taxAmount - availableCash;

        player->cash = 0;

        printf("%s could not fully pay %s.\n",
                player->name,
                taxSquare->name);

        printf("Paid available cash: LKR %d\n", availableCash);
        printf("Unpaid amount: LKR %d\n", unpaidAmount);
        printf("Debt recovery not yet implemented.\n");
    }
}




int calculateTotalMortgageValue(Player *player, Property board[]){

    int totalMortgageValue = 0;

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];

        Property *property = &board[squareIndex];

        if(property->type != SQUARE_PROPERTY &&
           property->type != SQUARE_RAILWAY &&
           property->type != SQUARE_UTILITY){

            continue;
        }

        if(property->isMortgaged == 1){
            continue;
        }

        if(property->isLoanLocked == 1){
            continue;
        }

        totalMortgageValue += property->mortgageValue;
    }

    return totalMortgageValue;
}

int calculateMaximumLoan(Player *player, Property board[]){

    int totalMortgageValue = calculateTotalMortgageValue(player, board);

    int maximumLoan = totalMortgageValue * 75 / 100;

    return maximumLoan;
}

int lockLoanCollateral(Player *player, Property board[], int loanAmount){

    int totalMortgageValue = 0;

    player->loan.numCollateral = 0;

    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];

        Property *property = &board[squareIndex];

        if(property->type != SQUARE_PROPERTY &&
           property->type != SQUARE_RAILWAY &&
           property->type != SQUARE_UTILITY){

            continue;
        }

        if(property->isMortgaged == 1){
            continue;
        }

        if(property->isLoanLocked == 1){
            continue;
        }

        player->loan.collateralIndices[player->loan.numCollateral] = squareIndex;
        player->loan.numCollateral++;

        property->isLoanLocked = 1;

        totalMortgageValue += property->mortgageValue;

        int supportedLoan = totalMortgageValue * 75 / 100;

        if(supportedLoan >= loanAmount){
            return 1;
        }
    }

    return 0;
}

void unlockLoanCollateral(Player *player, Property board[]){

    for(int i = 0; i < player->loan.numCollateral; i++){

        int squareIndex = player->loan.collateralIndices[i];

        board[squareIndex].isLoanLocked = 0;

        player->loan.collateralIndices[i] = -1;
    }

    player->loan.numCollateral = 0;
}

int createLoan(Player *player, Property board[], int loanAmount, int interestRate){

    if(player->loan.isActive == 1){

        printf("%s already has an active loan.\n",
               player->name);

        return 0;
    }

    if(loanAmount <= 0){
        return 0;
    }

    int maximumLoan = calculateMaximumLoan(player, board);

    if(loanAmount > maximumLoan){

        printf("%s cannot borrow LKR %d.\n",
               player->name,
               loanAmount);

        printf("Maximum available loan: LKR %d\n",
               maximumLoan);

        return 0;
    }
 
    if(lockLoanCollateral(player, board, loanAmount) == 0){

        unlockLoanCollateral(player, board);

        printf("Unable to secure enough collateral for the loan.\n");

        return 0;
    }


    player->loan.isActive = 1;
    player->loan.amount = loanAmount;
    player->loan.principalAmount = loanAmount;
    player->loan.interestRate = interestRate;
    player->loan.roundsRemaining = LOAN_DURATION_ROUNDS;

    player->cash += loanAmount;

    printf("%s obtained a secured loan.\n",
           player->name);

    printf("Loan Amount: LKR %d\n",
           loanAmount);

    printf("Interest Rate: %d%%\n",
           interestRate);

    printf("Duration: %d rounds\n",
           LOAN_DURATION_ROUNDS);

    printf("Current cash: LKR %d\n",
           player->cash);

    return 1;
}

void updateLoanAfterRound(Player *player){

    if(player->loan.isActive == 0){
        return;
    }

    int interest = player->loan.amount * player->loan.interestRate / 100;

    player->loan.amount += interest;

    player->loan.roundsRemaining--;

    printf("%s loan interest added: LKR %d\n",
           player->name,
           interest);

    printf("Outstanding loan: LKR %d\n",
           player->loan.amount);

    printf("Loan rounds remaining: %d\n",
           player->loan.roundsRemaining);
}

int repayLoanPart(Player *player, int repaymentAmount){

    if(player->loan.isActive == 0){

        printf("%s does not have an active loan.\n",
               player->name);

        return 0;
    }

    if(repaymentAmount <= 0){
        return 0;
    }

    if(repaymentAmount >= player->loan.amount){

        printf("Repayment amount must be less than the outstanding loan.\n");

        return 0;
    }

    if(player->cash < repaymentAmount){

        printf("%s does not have enough cash to repay LKR %d.\n",
               player->name,
               repaymentAmount);

        return 0;
    }

    player->cash -= repaymentAmount;
    player->loan.amount -= repaymentAmount;

    if(player->loan.amount < player->loan.principalAmount){

        player->loan.principalAmount = player->loan.amount;
    }   

    printf("%s repaid LKR %d.\n",
           player->name,
           repaymentAmount);

    printf("Outstanding loan: LKR %d\n",
           player->loan.amount);

    printf("Current cash: LKR %d\n",
           player->cash);

    return 1;
}

int repayLoanFull(Player *player, Property board[]){

    if(player->loan.isActive == 0){

        printf("%s does not have an active loan.\n",
               player->name);

        return 0;
    }

    if(player->cash < player->loan.amount){

        printf("%s does not have enough cash to fully repay the loan.\n",
               player->name);

        return 0;
    }

    int repaymentAmount = player->loan.amount;

    player->cash -= repaymentAmount;

    unlockLoanCollateral(player, board);

    player->loan.isActive = 0;
    player->loan.amount = 0;
    player->loan.principalAmount = 0;
    player->loan.interestRate = 0;
    player->loan.roundsRemaining = 0;


    printf("%s fully repaid the loan.\n",
           player->name);

    printf("Amount repaid: LKR %d\n",
           repaymentAmount);

    printf("Current cash: LKR %d\n",
           player->cash);

    return 1;
}

int extendLoan(Player *player){

    if(player->loan.isActive == 0){

        printf("%s does not have an active loan.\n",
               player->name);

        return 0;
    }

    if(player->loan.roundsRemaining <= 0){

        printf("%s's loan has already reached maturity.\n",
               player->name);

        return 0;
    }

    player->loan.roundsRemaining = LOAN_DURATION_ROUNDS;

    printf("%s extended the loan period.\n",
           player->name);

    printf("Loan duration reset to %d rounds.\n",
           LOAN_DURATION_ROUNDS);

    return 1;
}

int addLoanCollateral(Player *player, Property board[], int requiredLoanAmount){

    int totalMortgageValue = 0;

    int originalNumCollateral = player->loan.numCollateral;

    // Check how much the already pledged collateral supports
    for(int i = 0; i < player->loan.numCollateral; i++){

        int squareIndex = player->loan.collateralIndices[i];

        totalMortgageValue += board[squareIndex].mortgageValue;
    }

    int supportedLoan = totalMortgageValue * 75 / 100;

    if(supportedLoan >= requiredLoanAmount){
        return 1;
    }

    // Add new eligible collateral until enough support exists
    for(int i = 0; i < player->numOwnedProperties; i++){

        int squareIndex = player->ownedProperties[i];

        Property *property = &board[squareIndex];

        if(property->type != SQUARE_PROPERTY &&
           property->type != SQUARE_RAILWAY &&
           property->type != SQUARE_UTILITY){

            continue;
        }

        if(property->isMortgaged == 1){
            continue;
        }

        if(property->isLoanLocked == 1){
            continue;
        }

        player->loan.collateralIndices[player->loan.numCollateral] = squareIndex;
        player->loan.numCollateral++;

        property->isLoanLocked = 1;

        totalMortgageValue += property->mortgageValue;

        supportedLoan = totalMortgageValue * 75 / 100;

        if(supportedLoan >= requiredLoanAmount){
            return 1;
        }
    }

    // Roll back only the newly added collateral
    for(int i = originalNumCollateral; i < player->loan.numCollateral; i++){

        int squareIndex = player->loan.collateralIndices[i];

        board[squareIndex].isLoanLocked = 0;

        player->loan.collateralIndices[i] = -1;
    }

    player->loan.numCollateral = originalNumCollateral;

    return 0;
}

int increaseLoan(Player *player, Property board[], int additionalAmount){

    if(player->loan.isActive == 0){

        printf("%s does not have an active loan.\n",
               player->name);

        return 0;
    }

    if(additionalAmount <= 0){
        return 0;
    }

    int newPrincipalAmount = player->loan.principalAmount + additionalAmount;

    if(addLoanCollateral(player, board, newPrincipalAmount) == 0){

        printf("%s does not have enough collateral to increase the loan.\n",
               player->name);

        return 0;
    }

    player->loan.principalAmount += additionalAmount;
    player->loan.amount += additionalAmount;
    player->cash += additionalAmount;

    printf("%s increased the loan amount.\n",
           player->name);

    printf("Additional loan: LKR %d\n",
           additionalAmount);

    printf("Outstanding loan: LKR %d\n",
           player->loan.amount);

    printf("Current cash: LKR %d\n",
           player->cash);

    return 1;
}

static void removeOwnedProperty(Player *player, int squareIndex){

    for(int i = 0; i < player->numOwnedProperties; i++){
        
        if(player->ownedProperties[i] == squareIndex){

            for(int j = i; j < player->numOwnedProperties - 1; j++){
                
                player->ownedProperties[j] = player->ownedProperties[j + 1];
            }
        
            player->numOwnedProperties--;
            player->ownedProperties[player->numOwnedProperties] = -1;

            return;
        }
    }
}

void handleLoanDefault(Player players[], int playerIndex, Property board[], int turnOrder[]){

    Player *player = &players[playerIndex];

    if(player->loan.isActive == 0){
        return;
    }

    printf("\n%s has defaulted on the loan.\n",
           player->name);

    int collateralCount = player->loan.numCollateral;
    int foreclosedProperties[BOARD_SIZE];

    for(int i = 0; i < collateralCount; i++){

        int squareIndex = player->loan.collateralIndices[i];

        foreclosedProperties[i] = squareIndex;


        Property *property = &board[squareIndex];

        printf("Foreclosed: %s\n",
               property->name);
        
       //Remove buildings
        property->numHouses = 0;
        property->hasHotel = 0;

    
        //Cancel insurance
        property->insurance.isActive = 0;
        property->insurance.policyType = POLICY_NONE;
        property->insurance.provider = 0;
        property->insurance.expiryRound = 0;

        
        //Property returns to the Bank
        property->owner = -1;
        property->isLoanLocked = 0;

        //Remove from player's owned property list
        removeOwnedProperty(player, squareIndex);
        player->loan.collateralIndices[i] = -1;
    }

    
    //Clear the loan.
    player->loan.isActive = 0;
    player->loan.amount = 0;
    player->loan.principalAmount = 0;
    player->loan.interestRate = 0;
    player->loan.roundsRemaining = 0;
    player->loan.numCollateral = 0;

    printf("Collateral has been foreclosed.\n");
    printf("Outstanding debt cleared.\n");

    
    //Check whether player has any assets remaining.

    if(player->numOwnedProperties == 0 &&
       player->cash <= 0){

        player->isBankrupt = 1;

        printf("%s has been declared bankrupt.\n",
               player->name);
    }
    
    //Auction all the forclosed Properties
    for(int i = 0; i < collateralCount; i++){

        int squareIndex =
            foreclosedProperties[i];

        handleAuction(players, board, squareIndex, turnOrder);
    }
}   

void handleBank(Player players[], int playerIndex, Property board[], GameState *game){

    Player *player = &players[playerIndex];

    printf("%s landed on Bank of Ceylon.\n",
           player->name);

    printf("Current loan interest rate: %d%%\n",
           game->currentLoanInterestRate);

    BankAction action = decideBankAction(player);

    switch(action){

        case BANK_ACTION_TAKE_LOAN:{

            int loanAmount = decideLoanAmount(player, board);

            if(loanAmount <= 0){

                printf("%s does not have enough collateral for a loan.\n",
                    player->name);

                break;
            }

            createLoan(player, board, loanAmount, game->currentLoanInterestRate);

            break;
            }

        case BANK_ACTION_REPAY_PART:{

            int repaymentAmount =
                    decidePartialRepaymentAmount(player);

                if(repaymentAmount <= 0){

                    printf("%s cannot make a partial repayment.\n",
                        player->name);

                    break;
                }

                repayLoanPart(
                    player,
                    repaymentAmount
                );

                break;
            }

        case BANK_ACTION_REPAY_FULL:

            repayLoanFull(player, board);

            break;

        case BANK_ACTION_EXTEND:

            extendLoan(player);

            break;

        case BANK_ACTION_INCREASE:{

            int additionalAmount =
                decideLoanIncreaseAmount(
                    player,
                    board
                );

            if(additionalAmount <= 0){

                printf("%s cannot increase the loan.\n",
                    player->name);

                break;
            }

            increaseLoan(
                player,
                board,
                additionalAmount
            );

            break;
        }

        case BANK_ACTION_NONE:

            printf("%s decided not to perform a Bank transaction.\n",
                   player->name);

            break;
    }
}


