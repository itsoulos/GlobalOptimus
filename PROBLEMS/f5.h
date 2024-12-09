#ifndef f5_H
#define f5_H
# include <OPTIMUS/problem.h>

class f5 : public Problem
{
public:
    f5 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);


};

#endif // F5_H
