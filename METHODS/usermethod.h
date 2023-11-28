#ifndef USERMETHOD_H
#define USERMETHOD_H
# include <OPTIMUS/optimizer.h>

class UserMethod :public Optimizer
{
private:
    int paramValue;
public:
    UserMethod();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    ~UserMethod();
};

#endif // USERMETHOD_H
