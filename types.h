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
    SQUARE_BANK,
    SQUARE_COMMUNITY_FUND
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

typedef enum{

    ECONOMIC_EVENT_NONE,

    ECONOMIC_EVENT_TOURISM_BOOM,
    ECONOMIC_EVENT_FUEL_CRISIS,
    ECONOMIC_EVENT_HEAVY_MONSOON,
    ECONOMIC_EVENT_RECESSION,
    ECONOMIC_EVENT_STOCK_MARKET_BOOM,
    ECONOMIC_EVENT_HOUSING_PROGRAMME,
    ECONOMIC_EVENT_FOREIGN_INVESTMENT,
    ECONOMIC_EVENT_POLITICAL_UNREST

} EconomicEventType;

typedef enum{

    REGULATION_NONE,

    REGULATION_PROPERTY_TAX,
    REGULATION_REDUCE_LOAN_INTEREST,
    REGULATION_HOUSING_SUBSIDY,
    REGULATION_LUXURY_PROPERTY_TAX,
    REGULATION_RAILWAY_MODERNIZATION,
    REGULATION_ELECTRICITY_TARIFF,
    REGULATION_INSURANCE,
    REGULATION_ANTI_SPECULATION

} GovernmentRegulationType;

typedef enum{
    REGIONAL_EVENT_NONE,
    REGIONAL_EVENT_SOUTHERN_TOURISM,
    REGIONAL_EVENT_PORT_CITY,
    REGIONAL_EVENT_IT_GROWTH,
    REGIONAL_EVENT_NORTHERN_DEVELOPMENT,
    REGIONAL_EVENT_TEA_EXPORT,
    REGIONAL_EVENT_AIRPORT_EXPANSION,
    REGIONAL_EVENT_UNIVERSITY_GROWTH,
    REGIONAL_EVENT_BEACH_POLLUTION,
    REGIONAL_EVENT_FLOOD_DAMAGE,
    REGIONAL_EVENT_TRANSPORT_STRIKE,
    REGIONAL_EVENT_ELECTRICITY_TARIFF,
    REGIONAL_EVENT_WATER_SHORTAGE
} RegionalEventType;

typedef enum{
    NATIONAL_CARD_TOURISM_HYPE,
    NATIONAL_CARD_FUEL_SHORTAGE,
    NATIONAL_CARD_HEAVY_FLOODS,
    NATIONAL_CARD_POLITICAL_RALLY,
    NATIONAL_CARD_STOCK_MARKET_RISE,
    NATIONAL_CARD_ECONOMIC_DOWNTURN,
    NATIONAL_CARD_HOUSING_SUBSIDY,
    NATIONAL_CARD_INTEREST_RATE_CUT,
    NATIONAL_CARD_INTEREST_RATE_INCREASE,
    NATIONAL_CARD_TAX_AMNESTY,
    NATIONAL_CARD_POWER_FAILURE,
    NATIONAL_CARD_FOREIGN_FUNDING,
    NATIONAL_CARD_PORT_EXPANSION,
    NATIONAL_CARD_FESTIVAL_SEASON,
    NATIONAL_CARD_LABOUR_STRIKE,
    NATIONAL_CARD_INSURANCE_DISCOUNT,
    NATIONAL_CARD_PROPERTY_REVALUATION,
    NATIONAL_CARD_CURRENCY_DEPRECIATION,
    NATIONAL_CARD_GOVERNMENT_GRANT,
    NATIONAL_CARD_NATIONAL_DISASTER
} NationalEventCardType;

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

    int developmentDeadlineRound;

    Insurance insurance;

    int nationalEventClosedRounds;
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

    int nationalCardRounds[20];
    PropertyGroup revaluationGroup;
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

    int rentInflationIndex;

    PropertyGroup boomGroup;
    PropertyGroup declineGroup;

    int boomRoundsRemaining;
    int declineRoundsRemaining;

    int lastMarketAffectedRound[8];

    EconomicEventType activeEconomicEvent;
    int economicEventRoundsRemaining;

    GovernmentRegulationType activeRegulation;
    int regulationRoundsRemaining;

    RegionalEventType activeRegionalEvent;
    int regionalEventRoundsRemaining;

    NationalEventCardType nationalEventDeck[20];
    int nationalEventTop;

} GameState;



#endif
