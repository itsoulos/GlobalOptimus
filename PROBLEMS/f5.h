#ifndef F5_H
#define F5_H
# include <OPTIMUS/problem.h>

class F5 : public Problem
{
public:
    F5 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);


};

#endif // F5_H
