#ifndef F18_H
#define F18_H
# include <OPTIMUS/problem.h>

class f18 : public Problem
{
public:
    f18 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);
};

#endif // F18_H
