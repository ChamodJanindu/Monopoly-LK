#include <stdio.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "finance.h"
#include "game.h"

int main(void){

    Property board[BOARD_SIZE];
    Player players[NUM_PLAYERS];
    GameState game;

    int turnOrder[NUM_PLAYERS] = {0, 1, 2, 3};

    initBoard(board);

    initPlayer(&players[0], "Aggressive", STRATEGY_AGGRESSIVE);
    initPlayer(&players[1], "Conservative", STRATEGY_CONSERVATIVE);
    initPlayer(&players[2], "Risk Taker", STRATEGY_RISK_TAKER);
    initPlayer(&players[3], "Opportunistic", STRATEGY_OPPORTUNISTIC);

    initGameState(&game);

    /*
     * Give Aggressive some assets so a secured loan
     * can actually be created.
     */

    assignPropertyToPlayer(players, 0, board, 1);
    assignPropertyToPlayer(players, 0, board, 3);
    assignPropertyToPlayer(players, 0, board, 5);

    printf("=============================================\n");
    printf("BANK INTEGRATION TEST\n");
    printf("=============================================\n");

    printf("\nCurrent Bank interest rate: %d%%\n",
           game.currentLoanInterestRate);

    printf("Starting cash: LKR %d\n",
           players[0].cash);

    printf("Loan active: %d\n",
           players[0].loan.isActive);

    /*
     * TEST 1
     * First Bank visit.
     *
     * Since there is no active loan,
     * decideBankAction() should choose TAKE_LOAN.
     */

    printf("\n=============================================\n");
    printf("TEST 1 - FIRST BANK VISIT\n");
    printf("=============================================\n");

    players[0].position = 38;

    handleLanding(
        players,
        0,
        board,
        0,
        turnOrder,
        &game
    );

    printf("\nAFTER FIRST BANK VISIT\n");

    printf("Loan active: %d\n",
           players[0].loan.isActive);

    printf("Principal: LKR %d\n",
           players[0].loan.principalAmount);

    printf("Outstanding loan: LKR %d\n",
           players[0].loan.amount);

    printf("Loan interest rate: %d%%\n",
           players[0].loan.interestRate);

    printf("Rounds remaining: %d\n",
           players[0].loan.roundsRemaining);

    printf("Cash: LKR %d\n",
           players[0].cash);

    /*
     * TEST 2
     * Change the Bank's current rate.
     *
     * The existing loan rate must NOT change.
     */

    printf("\n=============================================\n");
    printf("TEST 2 - CHANGE CURRENT BANK RATE\n");
    printf("=============================================\n");

    game.currentLoanInterestRate = 12;

    printf("Current Bank rate changed to: %d%%\n",
           game.currentLoanInterestRate);

    printf("Existing Aggressive loan rate: %d%%\n",
           players[0].loan.interestRate);

    /*
     * TEST 3
     * Second Bank visit.
     *
     * Player already has a loan, so our temporary
     * decision function chooses partial repayment.
     */

    printf("\n=============================================\n");
    printf("TEST 3 - SECOND BANK VISIT\n");
    printf("=============================================\n");

    int loanBefore =
        players[0].loan.amount;

    int principalBefore =
        players[0].loan.principalAmount;

    int cashBefore =
        players[0].cash;

    handleLanding(
        players,
        0,
        board,
        0,
        turnOrder,
        &game
    );

    printf("\nAFTER SECOND BANK VISIT\n");

    printf("Outstanding before: LKR %d\n",
           loanBefore);

    printf("Outstanding after: LKR %d\n",
           players[0].loan.amount);

    printf("Principal before: LKR %d\n",
           principalBefore);

    printf("Principal after: LKR %d\n",
           players[0].loan.principalAmount);

    printf("Cash before: LKR %d\n",
           cashBefore);

    printf("Cash after: LKR %d\n",
           players[0].cash);

    printf("Loan still active: %d\n",
           players[0].loan.isActive);

    printf("Loan interest rate still: %d%%\n",
           players[0].loan.interestRate);

    printf("\n=============================================\n");
    printf("TEST FINISHED\n");
    printf("=============================================\n");

    return 0;
}