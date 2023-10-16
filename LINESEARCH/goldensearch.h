#ifndef GOLDENSEARCH_H
#define GOLDENSEARCH_H
# include <LINESEARCH/linesearch.h>

/**
 * @brief The GoldenSearch class implements the Golden Search
 * linesearch algorithm.
 */
class GoldenSearch : public LineSearch
{
private:
    double goldena,goldenb;
public:
    GoldenSearch(Problem *p);
    void setParams(double a,double b);
    virtual double getDirection(Data &x);
};

#endif // GOLDENSEARCH_H
