#ifndef USERMETHOD_H
#define USERMETHOD_H

#include <OPTIMUS/optimizer.h>
#include <vector>
#include <cmath>

class UserMethod : public Optimizer
{
private:
    /** samples einai posa deigmata tha parei se kathe
     *  epanalipsi **/
    int samples;
    /** se poia epanalipsi eimai **/
    int iter;
    int maxiters;

    /** einai i pio kali timi pou exei ftasei i Pure
     *  random search **/
    /** xbest einai to xpoint kai ybest to ypoint **/

    Data xbest;
    double ybest;
public:
    UserMethod();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual ~UserMethod();
};

#endif // USERMETHOD_H
