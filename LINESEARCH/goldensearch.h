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
    /**
     * @brief GoldenSearch The constructor of the class for problem p.
     * @param p
     */
    GoldenSearch(Problem *p);
    /**
     * @brief setParams Sets the bounds of line search.
     * @param a
     * @param b
     */
    void setParams(double a,double b);
    /**
     * @brief getDirection
     * @param x
     * @return A new direction starting from point x.
     */
    virtual double getDirection(Data &x);
};

#endif // GOLDENSEARCH_H
