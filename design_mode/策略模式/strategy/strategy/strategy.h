#ifndef __STRATEGY_H__
#define __STRATEGY_H__

class strategy
{
public:
    strategy(){};
    ~strategy(){};

public:
    double getNumber(){return _number1;};
    void   setNumber(double number){_number1 = number;};
public:
    virtual double cashResult() = 0;

private:
    double _number1;
};
#endif