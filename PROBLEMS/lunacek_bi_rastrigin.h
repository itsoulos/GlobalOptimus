#ifndef LUNACEK_BI_RASTRIGIN_H
#define LUNACEK_BI_RASTRIGIN_H

#include <OPTIMUS/problem.h>

class LunacekBiRastrigin : public Problem
{
public:
    LunacekBiRastrigin();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    
private:
    double mu0;       // First attractor center parameter
    double mu1;       // Second attractor center parameter
    double d;         // Distance between attractors
    double s;         // Scaling parameter
    double omega;     // Weight parameter
    
    void calculateParameters();
};

#endif // LUNACEK_BI_RASTRIGIN_H
