#include "abstractClass.h"
#include "stdio.h"

abstractClass::abstractClass()
{

}
abstractClass::~abstractClass()
{

}

void abstractClass::templateMethod()
{
    primitiveOperation1();
    primitiveOperation2();
    printf("templateMethod done\n");
}