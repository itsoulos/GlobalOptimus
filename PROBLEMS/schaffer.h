#ifndef SCHAFFER_H
#define SCHAFFER_H
# include <OPTIMUS/problem.h>
class schaffer : public Problem
{
public:
    schaffer ();
    double funmin(Data &x);
    Data gradient(Data &x);

};
#endif // SCHAFFER_H
