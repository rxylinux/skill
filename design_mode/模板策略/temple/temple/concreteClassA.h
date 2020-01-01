#ifndef __CONCRETE_CLASS_H__
#define __CONCRETE_CLASS_H__
#include "abstractClass.h"

class concreteClassA:public abstractClass
{
public:
    concreteClassA();
    ~concreteClassA();
public:
    void primitiveOperation1();
    void primitiveOperation2();

};


#endif