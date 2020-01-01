#include "concreteClassA.h"
#include "abstractClass.h"
#include "concreteClassB.h"

#include "stdio.h"
int main()
{
    abstractClass *c;
    c = new concreteClassA();
    c->templateMethod();
    delete c;

    c = new concreteClassB();
    c->templateMethod();
    delete c;
}
