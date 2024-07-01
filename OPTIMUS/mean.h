#ifndef MEAN_H
#define MEAN_H


class Mean
{
private:
    double oldSum;
    int minimumIters;
    int meanIterations;
    int meanCount;
    int iteration;
public:
    Mean();
    void    init();
    void    setMinIters(int m);
    void    setMeanIterations(int m);
    bool    terminate(double value);
    ~Mean();
};

#endif // MEAN_H
