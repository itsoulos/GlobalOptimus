#ifndef SIMILARITY_H
#define SIMILARITY_H

/**
 * @brief The Similarity class implements the similarity stopping rule
 */
class Similarity
{
private:
    double oldBest;
    int minimumIters;
    /**
     * @brief similarityIterations, the number of iterations
     * where the same minimum value will be discovered before termination.
     */
    int similarityIterations;
    /**
     * @brief similarityCount the current number of iterations
     * with the same minimum value.
     */
    int similarityCount;
    /**
     * @brief iteration
     */
    int iteration;
public:
    /**
     * @brief Similarity the default constructor of the class
     */
    Similarity();
    /**
     * @brief init this method initializes the variables of this termination rule.
     */
    void    init();
    /**
     * @brief setMinIters, changes the number of minimum iterations required before termination.
     * @param m
     */
    void    setMinIters(int m);
    /**
     * @brief setSimilarityIterations
     * @param m
     */
    void    setSimilarityIterations(int m);
    /**
     * @brief terminate
     * @param value
     * @return true if the method should be terminated
     */
    bool    terminate(double value);
    ~Similarity();
};

#endif // SIMILARITY_H
