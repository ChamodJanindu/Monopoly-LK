#include <stdio.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "game.h"
#include "finance.h"

int calculateRent(Property board[], int squareIndex, int diceTotal){

    // This function is only called when the property is owned by another player.
    Property *property = &board[squareIndex];

    if(property->isDamaged == 1){
        return 0;
    }

    int multiplier = 1;
    int rent = 0;

    if(property->isMortgaged == 0){

        // Normal properties
        if(property->type == SQUARE_PROPERTY){

            if(property->hasHotel == 1){
                multiplier = 10;
            }
            else{

                switch(property->numHouses){

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
                        printf("WARNING: invalid numHouses value %d for %s\n",
                               property->numHouses,
                               property->name);
                        multiplier = 1;
                        break;
                }
            }

            rent = property->baseRent * multiplier;

            if(property->numHouses > 0 || property->hasHotel == 1){

                int averageCondition =
                    calculateAverageBuildingCondition(property);

                if(averageCondition >= 90){
                    rent = rent;
                }
                else if(averageCondition >= 75){
                    rent = rent * 90 / 100;
                }
                else if(averageCondition >= 50){
                    rent = rent * 75 / 100;
                }
                else if(averageCondition >= 25){
                    rent = rent * 50 / 100;
                }
                else{
                    rent = 0;
                }
            }

            if(hasStructuralDamage(property) == 1){
                rent = rent * 75 / 100;
            }
        }

        // Railway stations
        else if(property->type == SQUARE_RAILWAY){

            int railwayCount = 0;

            for(int i = 0; i < BOARD_SIZE; i++){

                if(board[i].type == SQUARE_RAILWAY &&
                   board[i].owner == property->owner){

                    railwayCount++;
                }
            }

            switch(railwayCount){

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
                    printf("WARNING: invalid railwayCount value %d\n",
                           railwayCount);
                    rent = 0;
                    break;
            }
        }

        // Utilities
        else if(property->type == SQUARE_UTILITY){

            int utilityCount = 0;

            for(int i = 0; i < BOARD_SIZE; i++){

                if(board[i].type == SQUARE_UTILITY &&
                   board[i].owner == property->owner){

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
                printf("WARNING: invalid utilityCount value: %d\n",
                       utilityCount);
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
        
        for(int j = 0; j < 4; j++){

            property->houses[j].conditionRating = 100;
            property->houses[j].roundsNeglected = 0;
            property->houses[j].isStructurallyDamaged = 0;
        }

        property->hotel.conditionRating = 100;
        property->hotel.roundsNeglected = 0;
        property->hotel.isStructurallyDamaged = 0;

        property->isDamaged = 0;
        property->damageType = DISASTER_NONE;
        property->repairCost = 0;
    
        //Cancel insurance
        cancelInsurance(property);

        
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




int canInsureProperty(Property board[], int playerIndex, int squareIndex, InsurancePolicyType policyType){

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

    if(policyType == POLICY_NONE){
        return 0;
    }

    if(policyType == POLICY_BUSINESS_INTERRUPTION && property->hasHotel == 0){
        return 0;
    }

    return 1;
}

int calculateInsurancePremium(Property *property, InsurancePolicyType policyType){

    if(property == NULL){
        return 0;
    }

    int premium = 0;

    switch(policyType){

        case POLICY_BASIC:
            premium = property->marketValue * 5 / 100;
            break;

        case POLICY_COMPREHENSIVE:
            premium = property->marketValue * 10 / 100;
            break;

        case POLICY_BUSINESS_INTERRUPTION:
            premium = property->marketValue * 15 / 100;
            break;

        case POLICY_NONE:
            return 0;
    }

    return premium;
}

int purchaseInsurance(Player players[], int playerIndex, Property board[], int squareIndex, InsurancePolicyType policyType, InsuranceProvider provider, GameState *game){

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];

    if(canInsureProperty(board, playerIndex, squareIndex, policyType) == 0){

        printf("%s cannot be insured with this policy.\n",
               property->name);

        return 0;
    }

    if(provider != INSURANCE_PROVIDER_SRI_LANKA &&
       provider != INSURANCE_PROVIDER_CEYLINCO){

        return 0;
    }

    int premium = calculateInsurancePremium(property, policyType);

    if(player->cash < premium){

        printf("%s does not have enough cash to insure %s.\n",
               player->name,
               property->name);

        return 0;
    }

    player->cash -= premium;

    property->insurance.isActive = 1;
    property->insurance.policyType = policyType;
    property->insurance.provider = provider;
    property->insurance.expiryRound = game->completedRounds + INSURANCE_DURATION_ROUNDS;

    printf("%s purchased insurance for %s.\n",
           player->name,
           property->name);

    printf("Premium: LKR %d\n", premium);
    printf("Expiry round: %d\n", property->insurance.expiryRound);
    printf("Remaining cash: LKR %d\n", player->cash);

    return 1;
}

void handleInsurance(Player players[], int playerIndex, Property board[], int insuranceSquareIndex, GameState *game){

    Player *player = &players[playerIndex];

    InsuranceProvider provider;

    if(insuranceSquareIndex == SRI_LANKA_INSURANCE_SQUARE){

        provider = INSURANCE_PROVIDER_SRI_LANKA;
    }
    else if(insuranceSquareIndex == CEYLINCO_INSURANCE_SQUARE){

        provider = INSURANCE_PROVIDER_CEYLINCO;
    }
    else{

        printf("Invalid insurance square.\n");
        return;
    }

    //to chose the property base on player strategy
    int propertyIndex = decideInsuranceProperty(player, board);

    if(propertyIndex == -1){

        printf("%s has no property available for insurance.\n",
               player->name);

        return;
    }

    InsurancePolicyType policyType = decideInsurancePolicy(player, &board[propertyIndex]);

        if(canInsureProperty(board, playerIndex, propertyIndex, policyType) == 0){

        printf("%s cannot be insured with the selected policy.\n",
               board[propertyIndex].name);

        return;
    }

    purchaseInsurance(players, playerIndex, board, propertyIndex, policyType, provider, game);
}

void updateInsuranceAfterRound(Player players[], Property board[], GameState *game){

    for(int i = 0; i < BOARD_SIZE; i++){

        Property *property = &board[i];

        if(property->insurance.isActive == 0){
            continue;
        }

        int roundsRemaining = property->insurance.expiryRound - game->completedRounds;

        if(roundsRemaining == 3){

            printf("Insurance on %s expires in 3 rounds.\n",
                   property->name);

            if(property->owner >= 0 && property->owner < NUM_PLAYERS){

                printf("Owner: %s\n",
                       players[property->owner].name);
            }
        }

        if(roundsRemaining <= 0){

            printf("Insurance on %s has expired.\n",
                   property->name);

            cancelInsurance(property);
        }
    }
}

int isDisasterCovered(Property *property){

    if(property->insurance.isActive == 0){
        return 0;
    }

    switch(property->insurance.policyType){

        case POLICY_BASIC:

            if(property->damageType == DISASTER_FIRE ||
               property->damageType == DISASTER_FLOOD){

                return 1;
            }

            break;

        case POLICY_COMPREHENSIVE:

            if(property->damageType == DISASTER_FIRE ||
               property->damageType == DISASTER_FLOOD ||
               property->damageType == DISASTER_RIOT ||
               property->damageType == DISASTER_VANDALISM){

                return 1;
            }

            break;

        case POLICY_BUSINESS_INTERRUPTION:

            if(property->hasHotel == 1){
                return 1;
            }

            break;

        case POLICY_NONE:
            return 0;
    }

    return 0;
}

int calculateInsuranceCompensation(Property *property){

    if(isDisasterCovered(property) == 0){
        return 0;
    }

    if(property->insurance.policyType == POLICY_BASIC){

        return property->repairCost * 80 / 100;
    }

    if(property->insurance.policyType == POLICY_COMPREHENSIVE){

        return property->repairCost;
    }

    if(property->insurance.policyType == POLICY_BUSINESS_INTERRUPTION){

        int lostRent = property->baseRent * 5;

        return property->repairCost + lostRent;
    }

    return 0;
}

int processInsuranceClaim(Player players[], Property *property){

    if(property->owner < 0 || property->owner >= NUM_PLAYERS){
        return 0;
    }

    if(isDisasterCovered(property) == 0){
        return 0;
    }

    int compensation = calculateInsuranceCompensation(property);

    Player *owner = &players[property->owner];

    owner->cash += compensation;

    printf("%s received an insurance claim for %s.\n",
           owner->name,
           property->name);

    printf("Compensation: LKR %d\n",
           compensation);

    printf("Current cash: LKR %d\n",
           owner->cash);

    return 1;
}

int repairDamagedProperty(Player players[], Property *property){

    if(property->isDamaged == 0){
        return 0;
    }

    if(property->owner < 0 || property->owner >= NUM_PLAYERS){
        return 0;
    }

    Player *owner = &players[property->owner];

    if(owner->cash < property->repairCost){
        return 0;
    }

    owner->cash -= property->repairCost;

    printf("%s repaired %s for LKR %d.\n",
           owner->name,
           property->name,
           property->repairCost);

    property->isDamaged = 0;
    property->damageType = DISASTER_NONE;
    property->repairCost = 0;

    printf("%s can now collect rent again.\n",
           property->name);

    printf("Remaining cash: LKR %d\n",
           owner->cash);

    return 1;
}

void checkAutomaticRepairs(Player players[], Property board[]){

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].isDamaged == 1){

            repairDamagedProperty(players, &board[i]);
        }
    }
}

void cancelInsurance(Property *property){

    property->insurance.isActive = 0;
    property->insurance.policyType = POLICY_NONE;
    property->insurance.provider = INSURANCE_PROVIDER_NONE;
    property->insurance.expiryRound = 0;
}



void updatePropertyAges(Property board[]){

    for(int i = 0; i < BOARD_SIZE; i++){

        if(board[i].type == SQUARE_PROPERTY &&
           board[i].owner != -1){

            board[i].age++;
        }
    }
}

void updatePropertyDepreciation(Property board[]){

    for(int i = 0; i < BOARD_SIZE; i++){

        Property *property = &board[i];

        if(property->type != SQUARE_PROPERTY){
            continue;
        }

        if(property->owner == -1){
            continue;
        }

        if(property->age <= 50){
            continue;
        }

        if(property->age % 5 != 0){
            continue;
        }

        if(property->depreciationPercent >= 30){
            continue;
        }

        property->depreciationPercent++;

        printf("%s depreciation increased to %d%%.\n",
               property->name,
               property->depreciationPercent);
    }
}

int calculateDepreciatedValue(Property *property){

    if(property->depreciationPercent <= 0){
        return property->marketValue;
    }

    return property->marketValue * (100 - property->depreciationPercent) / 100;
}

int canRenovateProperty(Player players[], int playerIndex, Property board[], int squareIndex){

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

    if(property->depreciationPercent <= 0){
        return 0;
    }

    int renovationCost = calculateRenovationCost(property);

    if(players[playerIndex].cash < renovationCost){
        return 0;
    }

    return 1;
}

int calculateRenovationCost(Property *property){

    int currentValue = calculateCurrentPropertyValue(property);

    return currentValue * 10 / 100;
}

int renovateProperty(Player players[], int playerIndex, Property board[], int squareIndex){

    if(canRenovateProperty(players, playerIndex, board, squareIndex) == 0){
        return 0;
    }

    Player *player = &players[playerIndex];
    Property *property = &board[squareIndex];

    int renovationCost = calculateRenovationCost(property);
    int oldDepreciation = property->depreciationPercent;

    player->cash -= renovationCost;

    property->depreciationPercent = 0;
    property->age = 0;

    printf("%s renovated %s.\n",
           player->name,
           property->name);

    printf("Previous depreciation: %d%%\n",
           oldDepreciation);

    printf("Renovation cost: LKR %d\n",
           renovationCost);

    printf("Property age reset to 0.\n");

    printf("Current property value: LKR %d\n",
           calculateDepreciatedValue(property));

    return 1;
}


void updateBuildingConditions(Property board[]){

    for(int i = 0; i < BOARD_SIZE; i++){

        Property *property = &board[i];

        if(property->type != SQUARE_PROPERTY){
            continue;
        }


        if(property->hasHotel == 1){

            BuildingCondition *hotel = &property->hotel;

            if(hotel->conditionRating > 0){
                hotel->conditionRating -= 2;
            }

            if(hotel->conditionRating < 0){
                hotel->conditionRating = 0;
            }

            hotel->roundsNeglected++;

            if(hotel->roundsNeglected > 20 &&
               hotel->isStructurallyDamaged == 0){

                hotel->isStructurallyDamaged = 1;

                printf("Hotel on %s has suffered structural damage.\n",
                       property->name);
            }

            continue;
        }


        for(int j = 0; j < property->numHouses; j++){

            BuildingCondition *house = &property->houses[j];

            if(house->conditionRating > 0){
                house->conditionRating -= 2;
            }

            if(house->conditionRating < 0){
                house->conditionRating = 0;
            }

            house->roundsNeglected++;

            if(house->roundsNeglected > 20 &&
               house->isStructurallyDamaged == 0){

                house->isStructurallyDamaged = 1;

                printf("House %d on %s has suffered structural damage.\n",
                       j + 1,
                       property->name);
            }
        }
    }
}

int calculateAverageBuildingCondition(Property *property){

    if(property->hasHotel == 1){
        return property->hotel.conditionRating;
    }

    if(property->numHouses == 0){
        return 100;
    }

    int totalCondition = 0;

    for(int i = 0; i < property->numHouses; i++){
        totalCondition += property->houses[i].conditionRating;
    }

    return totalCondition / property->numHouses;
}

int calculateHouseMaintenanceCost(Property *property, int houseIndex){

    if(houseIndex < 0 || houseIndex >= property->numHouses){
        return 0;
    }

    int maintenanceCost = property->houseCost * 5 / 100;

    if(property->houses[houseIndex].isStructurallyDamaged == 1){
        maintenanceCost += maintenanceCost * 50 / 100;
    }

    return maintenanceCost;
}

int calculateHotelMaintenanceCost(Property *property){

    if(property->hasHotel == 0){
        return 0;
    }

    int maintenanceCost = property->hotelCost * 8 / 100;

    if(property->hotel.isStructurallyDamaged == 1){
        maintenanceCost += maintenanceCost * 50 / 100;
    }

    return maintenanceCost;
}

int maintainHouse(Player *player, Property *property, int houseIndex){

    if(houseIndex < 0 || houseIndex >= property->numHouses){
        return 0;
    }

    BuildingCondition *house = &property->houses[houseIndex];

    int maintenanceCost =
        calculateHouseMaintenanceCost(property, houseIndex);

    if(player->cash < maintenanceCost){

        printf("%s does not have enough cash to maintain House %d on %s.\n",
               player->name,
               houseIndex + 1,
               property->name);

        return 0;
    }

    player->cash -= maintenanceCost;

    house->conditionRating = 100;
    house->roundsNeglected = 0;

    printf("%s maintained House %d on %s for LKR %d.\n",
           player->name,
           houseIndex + 1,
           property->name,
           maintenanceCost);

    return 1;
}

int maintainHotel(Player *player, Property *property){

    if(property->hasHotel == 0){
        return 0;
    }

    int maintenanceCost =
        calculateHotelMaintenanceCost(property);

    if(player->cash < maintenanceCost){

        printf("%s does not have enough cash to maintain the hotel on %s.\n",
               player->name,
               property->name);

        return 0;
    }

    player->cash -= maintenanceCost;

    property->hotel.conditionRating = 100;
    property->hotel.roundsNeglected = 0;

    printf("%s maintained the hotel on %s for LKR %d.\n",
           player->name,
           property->name,
           maintenanceCost);

    return 1;
}


int hasStructuralDamage(Property *property){

    if(property->hasHotel == 1){
        return property->hotel.isStructurallyDamaged;
    }

    for(int i = 0; i < property->numHouses; i++){

        if(property->houses[i].isStructurallyDamaged == 1){
            return 1;
        }
    }

    return 0;
}

int calculateHouseStructuralRenovationCost(Property *property, int houseIndex){

    if(houseIndex < 0 || houseIndex >= property->numHouses){
        return 0;
    }

    return property->houseCost * 25 / 100;
}

int calculateHotelStructuralRenovationCost(Property *property){

    if(property->hasHotel == 0){
        return 0;
    }

    return property->hotelCost * 25 / 100;
}

int renovateHouseStructuralDamage(Player *player, Property *property, int houseIndex){

    if(houseIndex < 0 || houseIndex >= property->numHouses){
        return 0;
    }

    BuildingCondition *house = &property->houses[houseIndex];

    if(house->isStructurallyDamaged == 0){
        return 0;
    }

    int renovationCost =
        calculateHouseStructuralRenovationCost(property, houseIndex);

    if(player->cash < renovationCost){

        printf("%s does not have enough cash to renovate House %d on %s.\n",
               player->name,
               houseIndex + 1,
               property->name);

        return 0;
    }

    player->cash -= renovationCost;

    house->isStructurallyDamaged = 0;
    house->conditionRating = 100;
    house->roundsNeglected = 0;

    printf("%s repaired structural damage on House %d of %s for LKR %d.\n",
           player->name,
           houseIndex + 1,
           property->name,
           renovationCost);

    return 1;
}

int renovateHotelStructuralDamage(Player *player, Property *property){

    if(property->hasHotel == 0){
        return 0;
    }

    if(property->hotel.isStructurallyDamaged == 0){
        return 0;
    }

    int renovationCost =
        calculateHotelStructuralRenovationCost(property);

    if(player->cash < renovationCost){

        printf("%s does not have enough cash to renovate the hotel on %s.\n",
               player->name,
               property->name);

        return 0;
    }

    player->cash -= renovationCost;

    property->hotel.isStructurallyDamaged = 0;
    property->hotel.conditionRating = 100;
    property->hotel.roundsNeglected = 0;

    printf("%s repaired structural damage on the hotel of %s for LKR %d.\n",
           player->name,
           property->name,
           renovationCost);

    return 1;
}

//to calculate the property value after applying both age depriciation & structural damage depritiation.
int calculateCurrentPropertyValue(Property *property){

    int currentValue = calculateDepreciatedValue(property);

    if(hasStructuralDamage(property) == 1){
        currentValue = currentValue * 85 / 100;
    }

    return currentValue;
}

