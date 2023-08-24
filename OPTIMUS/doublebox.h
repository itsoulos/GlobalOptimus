#ifndef DOUBLEBOX_H
#define DOUBLEBOX_H
# include <OPTIMUS/problem.h>

class DoubleBox
{
private:
    double xx1;
    double xx2;
    double variance;
    double stopat;
    double oldBesty;
    double besty;
    int iteration;
    int miniters;
public:
    DoubleBox(int miniters=5);
    bool terminate(double value);
    ~DoubleBox();
};

#endif // DOUBLEBOX_H
