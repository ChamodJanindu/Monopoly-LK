#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

int generateInflationRate(void);
int applyInflationToValue(int value, int inflationRate);
void updateInflation(Property board[], GameState *game);

#endif