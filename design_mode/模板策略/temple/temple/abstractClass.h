#ifndef __ABSTRACT_CLASS_H__
#define __ABSTRACT_CLASS_H__

class abstractClass
{
public:
    abstractClass();
    ~abstractClass();
public:
    virtual void primitiveOperation1() = 0;
    virtual void primitiveOperation2() = 0;

    void templateMethod();

};


#endif