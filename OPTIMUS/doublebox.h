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
public:
    DoubleBox();
    bool terminate(double value);
    ~DoubleBox();
};

#endif // DOUBLEBOX_H
