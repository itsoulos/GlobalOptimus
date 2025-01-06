#ifndef F14_H
#define F14_H
# include <OPTIMUS/problem.h>

class f14 : public Problem
{
public:
    f14 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);
};


#endif // F14_H
