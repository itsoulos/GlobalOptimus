#ifndef SIMILARITY_H
#define SIMILARITY_H


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
    int iteration;
public:
    Similarity();
    void    init();
    void    setMinIters(int m);
    void    setSimilarityIterations(int m);
    bool    terminate(double value);
    ~Similarity();
};

#endif // SIMILARITY_H
