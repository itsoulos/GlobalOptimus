#ifndef ROTATED_ROSENBROCK_H
#define ROTATED_ROSENBROCK_H
#include <OPTIMUS/problem.h>

class RotatedRosenbrock : public Problem
{
private:
    std::vector<std::vector<double>> rotationMatrix;
    void generateRotationMatrix();
    Data rotate(const Data &x);
    Data rotateTranspose(const Data &x);
public:
    RotatedRosenbrock();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // ROTATED_ROSENBROCK_H
