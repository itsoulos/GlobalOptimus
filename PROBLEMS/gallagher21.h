#ifndef GALLAGHER21_H
#define GALLAGHER21_H

#include <OPTIMUS/problem.h>
#include <vector>

class Gallagher21 : public Problem
{
private:
    std::vector<Data> peaks_position;
    std::vector<double> peaks_height;
    std::vector<double> peaks_width;
    
    void initializePeaks();
    
public:
    Gallagher21();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // GALLAGHER21_H