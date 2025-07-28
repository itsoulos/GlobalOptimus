#ifndef EQUAL_MAXIMA_H
#define EQUAL_MAXIMA_H
# include <OPTIMUS/problem.h>

class EqualMaxima : public Problem
{
public:
    EqualMaxima();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);
};

#endif // EQUAL_MAXIMA_H


