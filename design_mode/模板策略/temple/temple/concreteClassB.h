#ifndef __CONCRETE_CLASSB_H__
#define __CONCRETE_CLASSB_H__
#include "abstractClass.h"

class concreteClassB:public abstractClass
{
public:
    concreteClassB();
    ~concreteClassB();
public:
    void primitiveOperation1();
    void primitiveOperation2();

};


#endif