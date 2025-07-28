#include "rotated_rosenbrock.h"
#include <random>


RotatedRosenbrock::RotatedRosenbrock()
    : Problem(1)
{
}

Data RotatedRosenbrock::rotate(const Data &x)
{
    Data rotated(x.size());
    for (size_t i = 0; i < rotationMatrix.size(); ++i)
    {
        rotated[i] = 0.0;
        for (size_t j = 0; j < rotationMatrix[i].size(); ++j)
        {
            rotated[i] += rotationMatrix[i][j] * x[j];
        }
    }
    return rotated;
}

Data RotatedRosenbrock::rotateTranspose(const Data &x)
{
    Data rotated(x.size());
    for (size_t i = 0; i < rotationMatrix.size(); ++i)
    {
        rotated[i] = 0.0;
        for (size_t j = 0; j < rotationMatrix[i].size(); ++j)
        {
            rotated[i] += rotationMatrix[j][i] * x[j]; // Transpose
        }
    }
    return rotated;
}

void RotatedRosenbrock::generateRotationMatrix()
{
    int n = getDimension();
    rotationMatrix.resize(n, std::vector<double>(n));

    // Δημιουργία τυχαίου πίνακα
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0, 1.0);

    // Γέμισμα με τυχαίες τιμές
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            rotationMatrix[i][j] = distribution(generator);
        }
    }

    // Gram-Schmidt ορθογωνοποίηση
    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < k; i++)
        {
            double dot = 0.0;
            for (int j = 0; j < n; j++)
            {
                dot += rotationMatrix[i][j] * rotationMatrix[k][j];
            }
            for (int j = 0; j < n; j++)
            {
                rotationMatrix[k][j] -= dot * rotationMatrix[i][j];
            }
        }

        // Κανονικοποίηση
        double norm = 0.0;
        for (int j = 0; j < n; j++)
        {
            norm += rotationMatrix[k][j] * rotationMatrix[k][j];
        }
        norm = sqrt(norm);
        for (int j = 0; j < n; j++)
        {
            rotationMatrix[k][j] /= norm;
        }
    }
}

double RotatedRosenbrock::funmin(Data &x)
{
    Data rotated_x = rotate(x);
    double sum = 0.0;
    for (int i = 0; i < getDimension()-1; i++)
    {
        double term1 = 100.0 * pow(rotated_x[i+1] - pow(rotated_x[i], 2), 2);
        double term2 = pow(1.0 - rotated_x[i], 2);
        sum += term1 + term2;
    }
    return sum;
}

Data RotatedRosenbrock::gradient(Data &x)
{
    Data rotated_x = rotate(x);
    Data rotated_g(getDimension(), 0.0);

    for (int i = 0; i < getDimension(); i++)
    {
        if (i == 0)
        {
            rotated_g[i] = -400.0 * rotated_x[i] * (rotated_x[i+1] - pow(rotated_x[i], 2)) - 2.0 * (1.0 - rotated_x[i]);
        }
        else if (i == getDimension()-1)
        {
            rotated_g[i] = 200.0 * (rotated_x[i] - pow(rotated_x[i-1], 2));
        }
        else
        {
            rotated_g[i] = 200.0 * (rotated_x[i] - pow(rotated_x[i-1], 2)) -
                          400.0 * rotated_x[i] * (rotated_x[i+1] - pow(rotated_x[i], 2)) -
                          2.0 * (1.0 - rotated_x[i]);
        }
    }

    return rotateTranspose(rotated_g);
}

void RotatedRosenbrock::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    generateRotationMatrix();

    Data l(n), r(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
