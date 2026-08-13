#ifndef TYPES_H
#define TYPES_H

#define BOARD_SIZE 40
#define NUM_PLAYERS 4

#define MAX_PROPERTY_NAME_LENGTH 64
#define MAX_PLAYER_NAME_LENGTH 20

#define STARTING_CASH 30000
#define GO_MONEY 2000
#define MAX_ROUNDS 500
#define JAIL_BAIL 300
#define AUCTION_INCREMENT 250
#define LOAN_DURATION_ROUNDS 20
#define INSURANCE_DURATION_ROUNDS 20

#define SRI_LANKA_INSURANCE_SQUARE 17
#define CEYLINCO_INSURANCE_SQUARE 33

typedef enum{
    SQUARE_START,
    SQUARE_PROPERTY, 
    SQUARE_EVENT, 
    SQUARE_TAX,
    SQUARE_RAILWAY, 
    SQUARE_UTILITY, 
    SQUARE_JAIL, 
    SQUARE_INSURANCE,
    SQUARE_FREE_PARKING, 
    SQUARE_GO_TO_JAIL, 
    SQUARE_BANK
}SquareType;

typedef enum{
    GROUP_BROWN, 
    GROUP_LIGHT_BLUE, 
    GROUP_PINK, 
    GROUP_ORANGE,
    GROUP_RED, 
    GROUP_YELLOW, 
    GROUP_GREEN, 
    GROUP_DARK_BLUE,
    GROUP_NONE
}PropertyGroup;

typedef enum {
    STRATEGY_AGGRESSIVE, 
    STRATEGY_CONSERVATIVE,
    STRATEGY_RISK_TAKER, 
    STRATEGY_OPPORTUNISTIC
} PlayerStrategy;

typedef enum {
    BANK_ACTION_NONE,
    BANK_ACTION_TAKE_LOAN,
    BANK_ACTION_REPAY_PART,
    BANK_ACTION_REPAY_FULL,
    BANK_ACTION_EXTEND,
    BANK_ACTION_INCREASE
} BankAction;

typedef enum {
    POLICY_NONE, 
    POLICY_BASIC, 
    POLICY_COMPREHENSIVE, 
    POLICY_BUSINESS_INTERRUPTION
} InsurancePolicyType;

typedef enum {
    INSURANCE_PROVIDER_NONE,
    INSURANCE_PROVIDER_SRI_LANKA,
    INSURANCE_PROVIDER_CEYLINCO
} InsuranceProvider;

typedef enum {
    DISASTER_NONE,
    DISASTER_FIRE,
    DISASTER_FLOOD,
    DISASTER_RIOT,
    DISASTER_BUILDING_COLLAPSE,
    DISASTER_ELECTRICAL_FAILURE,
    DISASTER_VANDALISM
} DisasterType;

typedef struct{
    int isActive;
    InsurancePolicyType policyType;
    InsuranceProvider provider;
    int expiryRound;
} Insurance;

typedef struct {
    int isActive;
    int amount;
    int principalAmount;
    int interestRate;     
    int roundsRemaining;
    int collateralIndices[BOARD_SIZE]; 
    int numCollateral;
} Loan;

typedef struct{
    int conditionRating;
    int roundsNeglected;
    int isStructurallyDamaged;
} BuildingCondition;

typedef struct {
    char name[MAX_PROPERTY_NAME_LENGTH];

    SquareType type;
    PropertyGroup group;

    int purchasePrice;
    int marketValue;
    int mortgageValue;
    int baseRent;
    int houseCost;
    int hotelCost;

    int taxAmount;

    int owner;
    int isMortgaged;
    int isLoanLocked;

    int numHouses;
    int hasHotel;

    int age;

    BuildingCondition houses[4];
    BuildingCondition hotel;
    
    int depreciationPercent;
    int structuralDamageValueLoss;

    int isDamaged;
    DisasterType damageType;
    int repairCost;

    Insurance insurance;
} Property;

typedef struct {
    char name[MAX_PLAYER_NAME_LENGTH];

    PlayerStrategy strategy;

    int cash;
    int position;
    int isInJail;
    int jailTurnsRemaining;
    int isBankrupt;

    int ownedProperties[BOARD_SIZE];
    int numOwnedProperties;

    int passedGoThisRound;
    int lapCount;

    Loan loan;
} Player;

typedef struct {
    int die1;
    int die2;
} DiceRoll;

typedef struct{

    int completedRounds;
    int gameOver;

    int currentLoanInterestRate;

    int currentInflationRate;

    PropertyGroup boomGroup;
    PropertyGroup declineGroup;

    int boomRoundsRemaining;
    int declineRoundsRemaining;

    int lastMarketAffectedRound[8];

} GameState;



#endif
