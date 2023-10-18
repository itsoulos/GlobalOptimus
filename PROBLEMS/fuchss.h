#ifndef FUCHSS_H
#define FUCHSS_H
# include <OPTIMUS/problem.h>


class FuchsS :public Problem
{
private:
    double e1(double omega);
    double e2(double omega);
    double q1(double q,double omega,double d);
    double q2(double q,double omega,double d);
public:
    FuchsS();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~FuchsS();
};

#endif // FUCHSS_H
