#include "strategyNormal.h"

strategyNormal::strategyNormal(double number)
{
    setNumber(number);
}

strategyNormal::~strategyNormal()
{

}

double strategyNormal::cashResult()
{
    return getNumber();
}