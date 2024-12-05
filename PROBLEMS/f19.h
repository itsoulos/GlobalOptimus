#ifndef F19_H
#define F19_H
# include <OPTIMUS/problem.h>

class f19 : public Problem
{
public:
    f19 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);
};

#endif // F19_H
