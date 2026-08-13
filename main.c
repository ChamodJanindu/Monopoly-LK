#include <stdio.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "finance.h"
#include "game.h"

int main(){

    Property board[BOARD_SIZE];
    Player players[NUM_PLAYERS];
    GameState game;

    initBoard(board);

    initPlayer(&players[0], "Player 1", STRATEGY_AGGRESSIVE);
    initPlayer(&players[1], "Player 2", STRATEGY_CONSERVATIVE);
    initPlayer(&players[2], "Player 3", STRATEGY_RISK_TAKER);
    initPlayer(&players[3], "Player 4", STRATEGY_OPPORTUNISTIC);

    initGameState(&game);

    printf("\n===== TEST 1: BASIC INSURANCE PURCHASE =====\n");

    assignPropertyToPlayer(players, 0, board, 1);

    board[1].marketValue = 10000;

    purchaseInsurance(players, 0, board, 1, POLICY_BASIC,
                      INSURANCE_PROVIDER_SRI_LANKA, &game);

    printf("Active: %d\n", board[1].insurance.isActive);
    printf("Policy: %d\n", board[1].insurance.policyType);
    printf("Expiry: %d\n", board[1].insurance.expiryRound);
    printf("Cash: %d\n", players[0].cash);


    printf("\n===== TEST 2: FIRE CLAIM WITH BASIC =====\n");

    board[1].numHouses = 1;
    board[1].isDamaged = 1;
    board[1].damageType = DISASTER_FIRE;
    board[1].repairCost = 5000;

    printf("Covered: %d\n",
           isDisasterCovered(&board[1]));

    printf("Compensation: %d\n",
           calculateInsuranceCompensation(&board[1]));

    processInsuranceClaim(players, &board[1]);

    printf("Player cash after claim: %d\n",
           players[0].cash);


    printf("\n===== TEST 3: BASIC DOES NOT COVER RIOT =====\n");

    board[1].damageType = DISASTER_RIOT;

    printf("Covered: %d\n",
           isDisasterCovered(&board[1]));

    printf("Compensation: %d\n",
           calculateInsuranceCompensation(&board[1]));


    printf("\n===== TEST 4: COMPREHENSIVE COVERS RIOT =====\n");

    purchaseInsurance(players, 0, board, 1, POLICY_COMPREHENSIVE,
                      INSURANCE_PROVIDER_CEYLINCO, &game);

    board[1].damageType = DISASTER_RIOT;
    board[1].repairCost = 5000;

    printf("Covered: %d\n",
           isDisasterCovered(&board[1]));

    printf("Compensation: %d\n",
           calculateInsuranceCompensation(&board[1]));


    printf("\n===== TEST 5: BUSINESS INTERRUPTION WITHOUT HOTEL =====\n");

    board[1].hasHotel = 0;

    printf("Can insure: %d\n",
           canInsureProperty(board, 0, 1,
                             POLICY_BUSINESS_INTERRUPTION));


    printf("\n===== TEST 6: BUSINESS INTERRUPTION WITH HOTEL =====\n");

    board[1].numHouses = 0;
    board[1].hasHotel = 1;

    printf("Can insure: %d\n",
           canInsureProperty(board, 0, 1,
                             POLICY_BUSINESS_INTERRUPTION));


    printf("\n===== TEST 7: DAMAGED PROPERTY RENT =====\n");

    board[1].isDamaged = 1;
    board[1].damageType = DISASTER_FIRE;
    board[1].repairCost = 1000;

    int rent = calculateRent(board, 1, 8);

    printf("Rent while damaged: %d\n",
           rent);


    printf("\n===== TEST 8: AUTOMATIC REPAIR =====\n");

    players[0].cash = 5000;

    checkAutomaticRepairs(players, board);

    printf("Damaged after repair: %d\n",
           board[1].isDamaged);

    printf("Repair cost after repair: %d\n",
           board[1].repairCost);

    printf("Player cash: %d\n",
           players[0].cash);


    printf("\n===== TEST 9: INSURANCE REMINDER =====\n");

    game.completedRounds = board[1].insurance.expiryRound - 3;

    updateInsuranceAfterRound(players, board, &game);


    printf("\n===== TEST 10: INSURANCE EXPIRY =====\n");

    game.completedRounds = board[1].insurance.expiryRound;

    updateInsuranceAfterRound(players, board, &game);

    printf("Active after expiry: %d\n",
           board[1].insurance.isActive);

    printf("Policy after expiry: %d\n",
           board[1].insurance.policyType);

           printf("\n===== TEST 11: BUSINESS INTERRUPTION COMPENSATION =====\n");

       board[1].hasHotel = 1;
       board[1].isDamaged = 1;
       board[1].damageType = DISASTER_FIRE;
       board[1].repairCost = 5000;
       board[1].baseRent = 500;

       board[1].insurance.isActive = 1;
       board[1].insurance.policyType = POLICY_BUSINESS_INTERRUPTION;
       board[1].insurance.provider = INSURANCE_PROVIDER_SRI_LANKA;
       board[1].insurance.expiryRound = game.completedRounds + 20;

       printf("Covered: %d\n",
              isDisasterCovered(&board[1]));

       printf("Compensation: %d\n",
              calculateInsuranceCompensation(&board[1]));

              printf("\n===== TEST 12: FORECLOSURE CANCELS INSURANCE =====\n");

       Player testPlayers[NUM_PLAYERS];

       initPlayer(&testPlayers[0], "Loan Player", STRATEGY_AGGRESSIVE);
       initPlayer(&testPlayers[1], "Player 2", STRATEGY_CONSERVATIVE);
       initPlayer(&testPlayers[2], "Player 3", STRATEGY_RISK_TAKER);
       initPlayer(&testPlayers[3], "Player 4", STRATEGY_OPPORTUNISTIC);

       Property testBoard[BOARD_SIZE];

       initBoard(testBoard);

       assignPropertyToPlayer(testPlayers, 0, testBoard, 1);

       testBoard[1].insurance.isActive = 1;
       testBoard[1].insurance.policyType = POLICY_BASIC;
       testBoard[1].insurance.provider = INSURANCE_PROVIDER_SRI_LANKA;
       testBoard[1].insurance.expiryRound = 20;

       testBoard[1].isLoanLocked = 1;

       testPlayers[0].loan.isActive = 1;
       testPlayers[0].loan.amount = 5000;
       testPlayers[0].loan.principalAmount = 5000;
       testPlayers[0].loan.interestRate = 8;
       testPlayers[0].loan.roundsRemaining = 0;
       testPlayers[0].loan.collateralIndices[0] = 1;
       testPlayers[0].loan.numCollateral = 1;

       int testTurnOrder[NUM_PLAYERS] = {0, 1, 2, 3};

       printf("Insurance before foreclosure: %d\n",
              testBoard[1].insurance.isActive);

       handleLoanDefault(testPlayers, 0, testBoard, testTurnOrder);

       printf("Insurance after foreclosure: %d\n",
              testBoard[1].insurance.isActive);

       printf("Policy after foreclosure: %d\n",
              testBoard[1].insurance.policyType);

       printf("Owner after foreclosure/auction: %d\n",
              testBoard[1].owner);

    printf("\n===== INSURANCE TEST COMPLETE =====\n");

    return 0;
}

