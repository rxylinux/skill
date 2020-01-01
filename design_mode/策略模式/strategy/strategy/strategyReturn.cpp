#include "strategyReturn.h"

strategyReturn::strategyReturn(double number)
{
    setNumber(number);
}

strategyReturn::~strategyReturn()
{

}

double strategyReturn::cashResult()
{
    return getNumber() * 0.6;
}