#include "strategyContext.h"
#include "strategyNormal.h"
#include "strategyReturn.h"

strategyContext::strategyContext()
{

}

strategyContext::~strategyContext()
{

}

double strategyContext::getResult(int type, double number)
{
    double result = 0;
    if (type == 0)
    {
        strategyNormal normal(number);
        result =  normal.cashResult();
    }
    else if (type == 1)
    {
        strategyReturn startegyreturn(number);
        result =  startegyreturn.cashResult();
    }
    else
    {

    }
    return result;
}