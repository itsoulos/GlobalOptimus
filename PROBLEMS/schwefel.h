#ifndef SCHWEFEL_H
#define SCHWEFEL_H
# include <OPTIMUS/problem.h>
class schwefel : public Problem
{
public:
     schwefel ();
    double funmin(Data &x);
    Data gradient(Data &x);

};
#endif // SCHWEFEL_H
