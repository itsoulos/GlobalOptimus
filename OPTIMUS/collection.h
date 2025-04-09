#ifndef COLLECTION_H
#define COLLECTION_H
# include "problem.h"

/**
 * @brief The Collection class implements a collection
 * of points (x,y) used in various optimization methods.
 */
class Collection
{
private:
    Matrix xdata;
    Data   ydata;
public:
    /**
     * @brief Collection The constructor of the class
     */
    Collection();
    /**
     * @brief addPointNoCheck This method adds (x,y) to the list of points
     * without checking for existance of y value.
     * @param x
     * @param y
     */
    void    addPointNoCheck(Data &x,double &y);
    /**
     * @brief addPoint This method adds (x,y) to the list of points.
     * @param x
     * @param y
     */
    void    addPoint(Data &x,double &y);
    /**
     * @brief getPoint Epistrefei ena simeio sto deigma
     * @param pos
     * @param x
     * @param y
     */
    void    getPoint(int pos,Data &x,double &y);
    /**
     * @brief haveGraphMinima Checks for the existance of a graph minima
     * @param x
     * @param y
     * @param distance
     * @return
     */
    bool    haveGraphMinima(Data &x,double &y,double distance);
    /**
     * @brief resizeInFraction Reduces the number of points to fraction percent.
     * @param fraction
     */
    void    resizeInFraction(double fraction);
    /**
     * @brief getSize
     * @return the size of the collection
     */
    int     getSize() const;

    /**
     * @brief getDistance Eyklidean distance between x and y.
     * @param x
     * @param y
     * @return
     */
    double  getDistance(Data &x,Data &y);

    /**
     * @brief isPointIn checks for existance of points.
     * @param x
     * @param y
     * @return true if point (x,y) is in collection.
     */
    bool isPointIn(Data &x,double  &y);

    /**
     * @brief replacePoint the point in position pos with (x,y)
     * @param pos
     * @param x
     * @param y
     */
    void    replacePoint(int pos,Data &x,double &y);
    /**
     * @brief getBestWorstValues return the minimum and the maximum
     * values in collection.
     * @param besty
     * @param worsty
     */
    void    getBestWorstValues(double &besty,double &worsty);
    /**
     * @brief sort This method sorts the point according to y values.
     */
    void    sort();
    /**
     * @brief clear Remove any point in the collection.
     */
    void    clear();
    ~Collection();
};

#endif // COLLECTION_H
