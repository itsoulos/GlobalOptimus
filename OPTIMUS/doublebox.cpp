#include "doublebox.h"

DoubleBox::DoubleBox()
{
    xx1= 0.0;
    xx2= 0.0;
    iteration = 0;
    besty = 1e+100;
    oldBesty = 1e+100;
    variance = 0.0;
    stopat = 0.0;
}

bool    DoubleBox::terminate(double value)
{
    double v = fabs(1.0+value);
    xx1+=v;
    xx2+=v * v;
    variance=xx2/(iteration+1)-(xx1/(iteration+1))*(xx1/(iteration+1));
    if(iteration == 0 || value<oldBesty)
    {
        stopat = variance/2.0;
    }
    iteration++;
    return variance <=stopat;
}

DoubleBox::~DoubleBox()
{
    //nothing here
}
