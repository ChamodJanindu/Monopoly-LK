#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "finance.h"
#include "game.h"


int passed = 0;
int failed = 0;


void checkInt(const char *name, int actual, int expected){

    if(actual == expected){

        printf("[PASS] %-55s actual=%d expected=%d\n",
               name, actual, expected);

        passed++;
    }
    else{

        printf("[FAIL] %-55s actual=%d expected=%d\n",
               name, actual, expected);

        failed++;
    }
}


void resetTestPlayer(Player *player){

    memset(player, 0, sizeof(*player));

    strcpy(player->name, "Test Player");

    player->cash = 100000;
    player->numOwnedProperties = 0;

    for(int i = 0; i < BOARD_SIZE; i++){
        player->ownedProperties[i] = -1;
        player->loan.collateralIndices[i] = -1;
    }
}


void resetBuilding(Property *property){

    property->numHouses = 0;
    property->hasHotel = 0;

    for(int i = 0; i < 4; i++){

        property->houses[i].conditionRating = 100;
        property->houses[i].roundsNeglected = 0;
        property->houses[i].isStructurallyDamaged = 0;
    }

    property->hotel.conditionRating = 100;
    property->hotel.roundsNeglected = 0;
    property->hotel.isStructurallyDamaged = 0;
}


int expectedRentPercent(int condition){

    if(condition >= 90){
        return 100;
    }

    if(condition >= 75){
        return 90;
    }

    if(condition >= 50){
        return 75;
    }

    if(condition >= 25){
        return 50;
    }

    return 0;
}


int main(void){

    Property board[BOARD_SIZE];
    Player players[NUM_PLAYERS];

    printf("\n");
    printf("============================================================\n");
    printf("        MONOPOLY-LK PHASE 7 FULL TEST\n");
    printf("        PER-BUILDING CONDITION MODEL\n");
    printf("============================================================\n\n");


    /*
     * ============================================================
     * TEST 1
     * PROPERTY AGE
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 1 - PROPERTY AGE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    board[1].owner = 0;
    board[1].age = 0;

    updatePropertyAges(board);

    checkInt("Owned property age increases", board[1].age, 1);

    board[3].owner = -1;
    board[3].age = 0;

    updatePropertyAges(board);

    checkInt("Unowned property age does not increase", board[3].age, 0);



    /*
     * ============================================================
     * TEST 2
     * PROPERTY DEPRECIATION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 2 - PROPERTY DEPRECIATION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    board[1].owner = 0;
    board[1].age = 55;
    board[1].depreciationPercent = 0;

    updatePropertyDepreciation(board);

    checkInt("Age 55 gives 1 percent depreciation",
             board[1].depreciationPercent, 1);


    board[1].age = 60;

    updatePropertyDepreciation(board);

    checkInt("Age 60 gives 2 percent total depreciation",
             board[1].depreciationPercent, 2);


    board[1].age = 61;

    updatePropertyDepreciation(board);

    checkInt("Non-five-year point does not depreciate",
             board[1].depreciationPercent, 2);


    board[1].age = 200;
    board[1].depreciationPercent = 29;

    updatePropertyDepreciation(board);

    checkInt("Depreciation reaches maximum 30 percent",
             board[1].depreciationPercent, 30);


    board[1].age = 205;

    updatePropertyDepreciation(board);

    checkInt("Depreciation cannot exceed 30 percent",
             board[1].depreciationPercent, 30);



    /*
     * ============================================================
     * TEST 3
     * INDIVIDUAL HOUSE CONDITIONS
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 3 - INDIVIDUAL HOUSE CONDITION UPDATE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    Property *property = &board[1];

    property->numHouses = 3;

    property->houses[0].conditionRating = 100;
    property->houses[1].conditionRating = 80;
    property->houses[2].conditionRating = 1;

    property->houses[0].roundsNeglected = 0;
    property->houses[1].roundsNeglected = 5;
    property->houses[2].roundsNeglected = 10;

    updateBuildingConditions(board);

    checkInt("House 1 condition 100 -> 98",
             property->houses[0].conditionRating, 98);

    checkInt("House 2 condition 80 -> 78",
             property->houses[1].conditionRating, 78);

    checkInt("House 3 condition cannot go below 0",
             property->houses[2].conditionRating, 0);

    checkInt("House 1 neglect increases",
             property->houses[0].roundsNeglected, 1);

    checkInt("House 2 neglect increases independently",
             property->houses[1].roundsNeglected, 6);

    checkInt("House 3 neglect increases independently",
             property->houses[2].roundsNeglected, 11);



    /*
     * ============================================================
     * TEST 4
     * INDIVIDUAL HOTEL CONDITION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 4 - HOTEL CONDITION UPDATE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->hasHotel = 1;
    property->numHouses = 0;

    property->hotel.conditionRating = 70;
    property->hotel.roundsNeglected = 7;

    updateBuildingConditions(board);

    checkInt("Hotel condition 70 -> 68",
             property->hotel.conditionRating, 68);

    checkInt("Hotel neglect 7 -> 8",
             property->hotel.roundsNeglected, 8);



    /*
     * ============================================================
     * TEST 5
     * STRUCTURAL DAMAGE BOUNDARY
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 5 - STRUCTURAL DAMAGE BOUNDARY\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->numHouses = 2;

    property->houses[0].conditionRating = 80;
    property->houses[0].roundsNeglected = 19;
    property->houses[0].isStructurallyDamaged = 0;

    property->houses[1].conditionRating = 80;
    property->houses[1].roundsNeglected = 20;
    property->houses[1].isStructurallyDamaged = 0;

    updateBuildingConditions(board);

    checkInt("20 rounds neglected is not structural damage",
             property->houses[0].isStructurallyDamaged, 0);

    checkInt("More than 20 rounds causes structural damage",
             property->houses[1].isStructurallyDamaged, 1);



    /*
     * ============================================================
     * TEST 6
     * AVERAGE BUILDING CONDITION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 6 - AVERAGE BUILDING CONDITION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->numHouses = 3;

    property->houses[0].conditionRating = 100;
    property->houses[1].conditionRating = 80;
    property->houses[2].conditionRating = 60;

    checkInt("Average of 100, 80, 60",
             calculateAverageBuildingCondition(property), 80);


    property->numHouses = 4;

    property->houses[0].conditionRating = 91;
    property->houses[1].conditionRating = 73;
    property->houses[2].conditionRating = 66;
    property->houses[3].conditionRating = 50;

    checkInt("Average uses integer calculation",
             calculateAverageBuildingCondition(property),
             (91 + 73 + 66 + 50) / 4);


    property->numHouses = 0;
    property->hasHotel = 1;
    property->hotel.conditionRating = 67;

    checkInt("Hotel condition is its own average",
             calculateAverageBuildingCondition(property), 67);


    property->hasHotel = 0;

    checkInt("Property with no buildings returns condition 100",
             calculateAverageBuildingCondition(property), 100);



    /*
     * ============================================================
     * TEST 7
     * RENT USING AVERAGE HOUSE CONDITION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 7 - RENT USING AVERAGE CONDITION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->owner = 0;
    property->numHouses = 3;

    property->houses[0].conditionRating = 100;
    property->houses[1].conditionRating = 80;
    property->houses[2].conditionRating = 60;

    /*
     * Pettah base rent = 100
     * 3 houses multiplier = 5
     *
     * Base developed rent = 500
     * Average condition = 80
     * 80 gives 90% rent
     *
     * Expected = 450
     */

    checkInt("3 houses average 80 gives 90 percent rent",
             calculateRent(board, 1, 7), 450);



    /*
     * ============================================================
     * TEST 8
     * ALL RENT CONDITION BOUNDARIES
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 8 - RENT CONDITION BOUNDARIES\n");
    printf("------------------------------------------------------------\n");

    int testConditions[] = {
        100, 90, 89, 75, 74, 50, 49, 25, 24, 0
    };

    int expectedPercents[] = {
        100, 100, 90, 90, 75, 75, 50, 50, 0, 0
    };

    for(int i = 0; i < 10; i++){

        initBoard(board);

        property = &board[1];

        property->owner = 0;
        property->numHouses = 1;

        property->houses[0].conditionRating = testConditions[i];

        int baseDevelopedRent = property->baseRent * 2;

        int expected =
            baseDevelopedRent * expectedPercents[i] / 100;

        int actual =
            calculateRent(board, 1, 6);

        char testName[100];

        snprintf(testName, sizeof(testName),
                 "Condition %d rent boundary",
                 testConditions[i]);

        checkInt(testName, actual, expected);
    }



    /*
     * ============================================================
     * TEST 9
     * STRUCTURAL DAMAGE RENT PENALTY
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 9 - STRUCTURAL DAMAGE RENT PENALTY\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->owner = 0;
    property->numHouses = 3;

    property->houses[0].conditionRating = 100;
    property->houses[1].conditionRating = 80;
    property->houses[2].conditionRating = 60;

    property->houses[0].isStructurallyDamaged = 1;

    /*
     * Average condition rent = 450
     * Structural penalty = 75%
     *
     * 450 * 75 / 100 = 337
     */

    checkInt("Any structural damage reduces property rent by 25 percent",
             calculateRent(board, 1, 6), 337);


    property->houses[0].isStructurallyDamaged = 0;
    property->houses[2].isStructurallyDamaged = 1;

    checkInt("Structural damage on another house has same property penalty",
             calculateRent(board, 1, 6), 337);



    /*
     * ============================================================
     * TEST 10
     * HAS STRUCTURAL DAMAGE
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 10 - STRUCTURAL DAMAGE SEARCH\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->numHouses = 4;

    checkInt("No damaged houses",
             hasStructuralDamage(property), 0);

    property->houses[3].isStructurallyDamaged = 1;

    checkInt("Damage on House 4 is detected",
             hasStructuralDamage(property), 1);


    property->numHouses = 0;
    property->hasHotel = 1;

    property->hotel.isStructurallyDamaged = 0;

    checkInt("Undamaged hotel",
             hasStructuralDamage(property), 0);

    property->hotel.isStructurallyDamaged = 1;

    checkInt("Damaged hotel detected",
             hasStructuralDamage(property), 1);



    /*
     * ============================================================
     * TEST 11
     * HOUSE MAINTENANCE COST
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 11 - HOUSE MAINTENANCE COST\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->numHouses = 2;

    property->houses[0].isStructurallyDamaged = 0;
    property->houses[1].isStructurallyDamaged = 1;

    /*
     * Pettah house cost = 500
     * 5% = 25
     *
     * structural:
     * 25 + 50% = 25 + 12 = 37
     */

    checkInt("Normal house maintenance cost",
             calculateHouseMaintenanceCost(property, 0), 25);

    checkInt("Structural house maintenance has 50 percent increase",
             calculateHouseMaintenanceCost(property, 1), 37);

    checkInt("Invalid house index rejected",
             calculateHouseMaintenanceCost(property, 5), 0);



    /*
     * ============================================================
     * TEST 12
     * HOTEL MAINTENANCE COST
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 12 - HOTEL MAINTENANCE COST\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->hasHotel = 1;

    property->hotel.isStructurallyDamaged = 0;

    /*
     * Pettah hotel cost = 2000
     * 8% = 160
     */

    checkInt("Normal hotel maintenance cost",
             calculateHotelMaintenanceCost(property), 160);


    property->hotel.isStructurallyDamaged = 1;

    checkInt("Structural hotel maintenance has 50 percent increase",
             calculateHotelMaintenanceCost(property), 240);



    /*
     * ============================================================
     * TEST 13
     * MAINTAIN ONE HOUSE ONLY
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 13 - INDIVIDUAL HOUSE MAINTENANCE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->numHouses = 3;

    property->houses[0].conditionRating = 60;
    property->houses[0].roundsNeglected = 10;

    property->houses[1].conditionRating = 70;
    property->houses[1].roundsNeglected = 15;

    property->houses[2].conditionRating = 85;
    property->houses[2].roundsNeglected = 8;

    int oldCash = players[0].cash;

    maintainHouse(&players[0], property, 1);

    checkInt("Selected House 2 restored to 100",
             property->houses[1].conditionRating, 100);

    checkInt("Selected House 2 neglect reset",
             property->houses[1].roundsNeglected, 0);

    checkInt("House 1 remains unchanged",
             property->houses[0].conditionRating, 60);

    checkInt("House 3 remains unchanged",
             property->houses[2].conditionRating, 85);

    checkInt("Only one house maintenance cost deducted",
             players[0].cash, oldCash - 25);



    /*
     * ============================================================
     * TEST 14
     * NORMAL MAINTENANCE DOES NOT REMOVE STRUCTURAL DAMAGE
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 14 - MAINTENANCE VS STRUCTURAL DAMAGE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->numHouses = 1;

    property->houses[0].conditionRating = 40;
    property->houses[0].roundsNeglected = 25;
    property->houses[0].isStructurallyDamaged = 1;

    maintainHouse(&players[0], property, 0);

    checkInt("Normal maintenance restores condition",
             property->houses[0].conditionRating, 100);

    checkInt("Normal maintenance resets neglect",
             property->houses[0].roundsNeglected, 0);

    checkInt("Normal maintenance does NOT clear structural damage",
             property->houses[0].isStructurallyDamaged, 1);



    /*
     * ============================================================
     * TEST 15
     * HOUSE STRUCTURAL RENOVATION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 15 - HOUSE STRUCTURAL RENOVATION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->numHouses = 1;

    property->houses[0].conditionRating = 20;
    property->houses[0].roundsNeglected = 30;
    property->houses[0].isStructurallyDamaged = 1;

    checkInt("House structural renovation cost is 25 percent",
             calculateHouseStructuralRenovationCost(property, 0), 125);

    oldCash = players[0].cash;

    renovateHouseStructuralDamage(&players[0], property, 0);

    checkInt("House structural damage removed",
             property->houses[0].isStructurallyDamaged, 0);

    checkInt("House condition restored after renovation",
             property->houses[0].conditionRating, 100);

    checkInt("House neglect reset after renovation",
             property->houses[0].roundsNeglected, 0);

    checkInt("House structural renovation cost deducted",
             players[0].cash, oldCash - 125);



    /*
     * ============================================================
     * TEST 16
     * HOTEL STRUCTURAL RENOVATION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 16 - HOTEL STRUCTURAL RENOVATION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->hasHotel = 1;
    property->numHouses = 0;

    property->hotel.conditionRating = 10;
    property->hotel.roundsNeglected = 30;
    property->hotel.isStructurallyDamaged = 1;

    checkInt("Hotel structural renovation cost is 25 percent",
             calculateHotelStructuralRenovationCost(property), 500);

    oldCash = players[0].cash;

    renovateHotelStructuralDamage(&players[0], property);

    checkInt("Hotel structural damage removed",
             property->hotel.isStructurallyDamaged, 0);

    checkInt("Hotel condition restored",
             property->hotel.conditionRating, 100);

    checkInt("Hotel neglect reset",
             property->hotel.roundsNeglected, 0);

    checkInt("Hotel renovation cost deducted",
             players[0].cash, oldCash - 500);



    /*
     * ============================================================
     * TEST 17
     * CURRENT PROPERTY VALUE
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 17 - PROPERTY VALUE WITH BOTH TYPES OF DAMAGE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->depreciationPercent = 10;
    property->numHouses = 1;

    property->houses[0].isStructurallyDamaged = 0;

    /*
     * Pettah market value = 1500
     * 10% depreciation -> 1350
     */

    checkInt("10 percent age depreciation",
             calculateCurrentPropertyValue(property), 1350);


    property->houses[0].isStructurallyDamaged = 1;

    /*
     * 1350 * 85 / 100 = 1147
     */

    checkInt("Age depreciation plus structural 15 percent value loss",
             calculateCurrentPropertyValue(property), 1147);



    /*
     * ============================================================
     * TEST 18
     * PROPERTY AGE RENOVATION
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 18 - PROPERTY AGE RENOVATION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->owner = 0;
    property->age = 100;
    property->depreciationPercent = 10;

    /*
     * Current value = 1350
     * renovation = 10% = 135
     */

    checkInt("Property renovation cost",
             calculateRenovationCost(property), 135);

    oldCash = players[0].cash;

    renovateProperty(players, 0, board, 1);

    checkInt("Property age resets to 0",
             property->age, 0);

    checkInt("Property depreciation resets to 0",
             property->depreciationPercent, 0);

    checkInt("Property renovation cost deducted",
             players[0].cash, oldCash - 135);



    /*
     * ============================================================
     * TEST 19
     * NEW HOUSE STARTS FRESH
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 19 - NEW HOUSE INITIALIZATION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    /*
     * Brown group consists of Pettah and Maradhana.
     * Give both to player 0 so building is legal.
     */

    board[1].owner = 0;
    board[3].owner = 0;

    buildHouse(players, 0, board, 1);

    checkInt("First house successfully added",
             board[1].numHouses, 1);

    checkInt("New house starts at condition 100",
             board[1].houses[0].conditionRating, 100);

    checkInt("New house starts with 0 neglect",
             board[1].houses[0].roundsNeglected, 0);

    checkInt("New house starts without structural damage",
             board[1].houses[0].isStructurallyDamaged, 0);


    /*
     * Change first house condition before building another.
     */

    board[1].houses[0].conditionRating = 62;

    /*
     * Equal construction rule requires Maradhana to receive
     * its first house before Pettah gets its second.
     */

    buildHouse(players, 0, board, 3);
    buildHouse(players, 0, board, 1);

    checkInt("Old house condition remains unchanged",
             board[1].houses[0].conditionRating, 62);

    checkInt("Second new house starts at 100",
             board[1].houses[1].conditionRating, 100);



    /*
     * ============================================================
     * TEST 20
     * FOUR HOUSES -> HOTEL
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 20 - HOTEL CONVERSION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    board[1].owner = 0;
    board[3].owner = 0;

    /*
     * Build evenly until both Brown properties have four houses.
     */

    for(int level = 0; level < 4; level++){

        buildHouse(players, 0, board, 1);
        buildHouse(players, 0, board, 3);
    }

    checkInt("Pettah reaches four houses",
             board[1].numHouses, 4);

    checkInt("Maradhana reaches four houses",
             board[3].numHouses, 4);


    /*
     * Give old houses strange states.
     * Hotel construction should clear all of them.
     */

    board[1].houses[0].conditionRating = 20;
    board[1].houses[0].roundsNeglected = 30;
    board[1].houses[0].isStructurallyDamaged = 1;

    buildHotel(players, 0, board, 1);

    checkInt("Houses removed after hotel construction",
             board[1].numHouses, 0);

    checkInt("Hotel flag becomes 1",
             board[1].hasHotel, 1);

    checkInt("New hotel starts at condition 100",
             board[1].hotel.conditionRating, 100);

    checkInt("New hotel starts at 0 neglect",
             board[1].hotel.roundsNeglected, 0);

    checkInt("New hotel has no structural damage",
             board[1].hotel.isStructurallyDamaged, 0);

    checkInt("Old house state cleared",
             board[1].houses[0].conditionRating, 100);

    checkInt("Old house structural state cleared",
             board[1].houses[0].isStructurallyDamaged, 0);



    /*
     * ============================================================
     * TEST 21
     * BEGINNING-OF-TURN MAINTENANCE PHASE
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 21 - AUTOMATIC TEMPORARY MAINTENANCE PHASE\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->owner = 0;
    property->numHouses = 3;

    players[0].ownedProperties[0] = 1;
    players[0].numOwnedProperties = 1;

    /*
     * House 1: below 90 -> normal maintenance
     * House 2: structurally damaged -> structural renovation
     * House 3: above 90 -> nothing
     */

    property->houses[0].conditionRating = 80;
    property->houses[0].roundsNeglected = 10;
    property->houses[0].isStructurallyDamaged = 0;

    property->houses[1].conditionRating = 40;
    property->houses[1].roundsNeglected = 25;
    property->houses[1].isStructurallyDamaged = 1;

    property->houses[2].conditionRating = 95;
    property->houses[2].roundsNeglected = 2;
    property->houses[2].isStructurallyDamaged = 0;

    oldCash = players[0].cash;

    handleMaintenancePhase(players, 0, board);

    checkInt("Maintenance phase repairs House 1",
             property->houses[0].conditionRating, 100);

    checkInt("Maintenance phase structurally renovates House 2",
             property->houses[1].conditionRating, 100);

    checkInt("House 2 structural damage removed",
             property->houses[1].isStructurallyDamaged, 0);

    checkInt("House 3 is left alone",
             property->houses[2].conditionRating, 95);

    /*
     * House 1 maintenance = 25
     * House 2 structural renovation = 125
     * Total = 150
     */

    checkInt("Correct total maintenance phase cost",
             players[0].cash, oldCash - 150);



    /*
     * ============================================================
     * TEST 22
     * INSUFFICIENT CASH
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 22 - INSUFFICIENT CASH\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);
    resetTestPlayer(&players[0]);

    property = &board[1];

    property->numHouses = 1;

    property->houses[0].conditionRating = 50;
    property->houses[0].roundsNeglected = 10;

    players[0].cash = 10;

    checkInt("Maintenance fails when player cannot afford it",
             maintainHouse(&players[0], property, 0), 0);

    checkInt("Condition stays unchanged after failed maintenance",
             property->houses[0].conditionRating, 50);

    checkInt("Cash stays unchanged after failed maintenance",
             players[0].cash, 10);



    /*
     * ============================================================
     * TEST 23
     * DISASTER DAMAGE STILL BLOCKS RENT
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 23 - DISASTER DAMAGE VS BUILDING CONDITION\n");
    printf("------------------------------------------------------------\n");

    initBoard(board);

    property = &board[1];

    property->owner = 0;
    property->numHouses = 1;
    property->houses[0].conditionRating = 100;

    property->isDamaged = 1;

    checkInt("Disaster-damaged property collects no rent",
             calculateRent(board, 1, 6), 0);



    /*
     * ============================================================
     * TEST 24
     * RANDOM STRESS TEST
     * ============================================================
     */

    printf("\n------------------------------------------------------------\n");
    printf("TEST 24 - 100 RANDOM PER-BUILDING SCENARIOS\n");
    printf("------------------------------------------------------------\n");

    unsigned int seed = (unsigned int)time(NULL);

    srand(seed);

    printf("Random seed: %u\n\n", seed);

    int randomFailures = 0;

    for(int test = 1; test <= 100; test++){

        initBoard(board);

        property = &board[1];

        property->owner = 0;
        property->numHouses = (rand() % 4) + 1;
        property->hasHotel = 0;

        int totalCondition = 0;
        int structuralDamage = 0;

        for(int i = 0; i < property->numHouses; i++){

            int condition = rand() % 101;

            property->houses[i].conditionRating = condition;
            property->houses[i].roundsNeglected = rand() % 21;
            property->houses[i].isStructurallyDamaged = rand() % 2;

            totalCondition += condition;

            if(property->houses[i].isStructurallyDamaged == 1){
                structuralDamage = 1;
            }
        }

        int expectedAverage =
            totalCondition / property->numHouses;

        int actualAverage =
            calculateAverageBuildingCondition(property);

        if(actualAverage != expectedAverage){

            printf("[RANDOM FAIL %d] Average actual=%d expected=%d\n",
                   test,
                   actualAverage,
                   expectedAverage);

            randomFailures++;
            continue;
        }


        int multiplier = 1;

        switch(property->numHouses){

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
        }

        int expectedRent =
            property->baseRent * multiplier;

        int percent =
            expectedRentPercent(expectedAverage);

        expectedRent =
            expectedRent * percent / 100;

        if(structuralDamage == 1){
            expectedRent = expectedRent * 75 / 100;
        }

        int actualRent =
            calculateRent(board, 1, 7);

        if(actualRent != expectedRent){

            printf("[RANDOM FAIL %d] Rent actual=%d expected=%d avg=%d houses=%d structural=%d\n",
                   test,
                   actualRent,
                   expectedRent,
                   expectedAverage,
                   property->numHouses,
                   structuralDamage);

            randomFailures++;
        }
    }

    if(randomFailures == 0){

        printf("[PASS] All 100 random scenarios passed\n");
        passed++;
    }
    else{

        printf("[FAIL] Random scenarios failed: %d\n",
               randomFailures);

        failed++;
    }



    /*
     * ============================================================
     * FINAL RESULT
     * ============================================================
     */

    printf("\n");
    printf("============================================================\n");
    printf("                 PHASE 7 TEST SUMMARY\n");
    printf("============================================================\n");

    printf("Passed checks : %d\n", passed);
    printf("Failed checks : %d\n", failed);

    printf("============================================================\n");

    if(failed == 0){

        printf("PHASE 7 TEST RESULT: ALL TESTS PASSED\n");
        printf("============================================================\n");

        return 0;
    }

    printf("PHASE 7 TEST RESULT: SOME TESTS FAILED\n");
    printf("============================================================\n");

    return 1;
}