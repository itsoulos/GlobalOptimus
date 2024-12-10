#ifndef SCHWEFEL222_H
#define SCHWEFEL222_H
# include <OPTIMUS/problem.h>

class schwefel222 : public Problem
{
public:
    schwefel222 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);
};
#endif // SCHWEFEL222_H
