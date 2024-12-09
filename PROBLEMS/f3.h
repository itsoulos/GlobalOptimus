#ifndef F3_H
#define F3_H
# include <OPTIMUS/problem.h>

class f3 : public Problem
{
public:
    f3 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);

};

#endif // F3_H
