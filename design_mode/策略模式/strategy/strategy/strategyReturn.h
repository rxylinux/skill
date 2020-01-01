#ifndef __STRATEGY_RETURN_H__
#define __STRATEGY_RETURN_H__
#include "strategy.h"
class strategyReturn : public strategy
{
public:
    strategyReturn(double number);
    ~strategyReturn();
public:
    double cashResult();

};
#endif