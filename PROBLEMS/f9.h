#ifndef F9_H
#define F9_H
# include <OPTIMUS/problem.h>

class f9 : public Problem
{
public:
    f9 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);

};

#endif // F9_H
