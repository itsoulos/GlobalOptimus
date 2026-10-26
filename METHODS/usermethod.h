#ifndef USERMETHOD_H
#define USERMETHOD_H

#include <OPTIMUS/optimizer.h>
#include <vector>
#include <cmath>

class UserMethod : public Optimizer
{
private:
public:
    UserMethod();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual ~UserMethod();
};

#endif // USERMETHOD_H
