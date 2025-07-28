#ifndef SPHERE_H
#define SPHERE_H
# include <OPTIMUS/problem.h>
class Sphere : public Problem
{
public:
   Sphere ();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // SPHERE_H




