#include "strategyContext.h"

int main()
{
    strategyContext strategy;
    double normal = strategy.getResult(0, 10.0);

    double ret = strategy.getResult(1, 10.0);
    return 0;
}