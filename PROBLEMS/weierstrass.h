#ifndef WEIERSTRASS_H
#define WEIERSTRASS_H

#include <OPTIMUS/problem.h>
#include <vector>

class Weierstrass : public Problem
{
private:
    int k_max;  // Maximum number of terms in the series
    double a;   // Parameter a (0 < a < 1)
    double b;   // Parameter b (positive integer)
    
public:
    Weierstrass();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // WEIERSTRASS_H