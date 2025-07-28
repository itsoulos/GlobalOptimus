 
#ifndef ATTRACTIVE_SECTOR_H
#define ATTRACTIVE_SECTOR_H
#include <OPTIMUS/problem.h>

class AttractiveSector : public Problem
{
public:
    AttractiveSector();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // ATTRACTIVE_SECTOR_H