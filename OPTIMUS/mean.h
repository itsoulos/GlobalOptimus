#ifndef MEAN_H
#define MEAN_H

/**
 * @brief The Mean class implements the mean termination rule.
 */
class Mean
{
private:
    double oldSum;
    int minimumIters;
    int meanIterations;
    int meanCount;
    int iteration;
public:
    /**
     * @brief Mean, the default constructor of the class.
     */
    Mean();
    /**
     * @brief init, initializes the variables of the termination rule.
     */
    void    init();
    /**
     * @brief setMinIters, set the minimum iters before the stopping rule
     *                     will be calculated.
     * @param m, the minimum number of iters.
     */
    void    setMinIters(int m);
    /**
     * @brief setMeanIterations
     * @param m
     */
    void    setMeanIterations(int m);
    /**
     * @brief terminate
     * @param value
     * @return
     */
    bool    terminate(double value);
    ~Mean();
};

#endif // MEAN_H
