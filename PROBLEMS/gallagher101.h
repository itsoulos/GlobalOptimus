#ifndef GALLAGHER101_H
#define GALLAGHER101_H

#include <OPTIMUS/problem.h>
#include <vector>

class Gallagher101 : public Problem
{
public:
    Gallagher101();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    
private:
    std::vector<Data> peaks_position;
    std::vector<double> peaks_height;
    std::vector<double> peaks_width;
    
    void initializePeaks();
    double gaussianPeak(const Data& x, const Data& center, double height, double width) const;
};

#endif // GALLAGHER101_H