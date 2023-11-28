#ifndef MYMETHOD_H
#define MYMETHOD_H
# include <OPTIMUS/optimizer.h>
class MyMethod : public Optimizer
{
private:
   int paramValue;
public:
    MyMethod();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
    ~MyMethod();
};

#endif
