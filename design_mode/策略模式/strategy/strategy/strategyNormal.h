#ifndef __STRATEGY_NORMAL_H__
#define __STRATEGY_NORMAL_H__
#include "strategy.h"
class strategyNormal : public strategy
{
public:
    strategyNormal(double number);
    ~strategyNormal();
public:
    double cashResult();

};
#endif