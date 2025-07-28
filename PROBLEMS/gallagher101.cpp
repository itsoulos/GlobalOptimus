#include "gallagher101.h"
#include <random>
#include <algorithm>

Gallagher101::Gallagher101()
    : Problem(1)
{
}

void Gallagher101::initializePeaks()
{
    int n = getDimension();
    peaks_position.resize(101);
    peaks_height.resize(101);
    peaks_width.resize(101);
    
    // Random number generation with fixed seed for reproducibility
    std::default_random_engine generator(42);
    std::uniform_real_distribution<double> position_dist(-5.0, 5.0);
    std::uniform_real_distribution<double> height_dist(1.1, 9.8);
    std::uniform_real_distribution<double> width_dist(1.1, 10.0);
    
    // Global optimum (first peak)
    peaks_position[0].resize(n, 0.0);  // Center at origin
    peaks_height[0] = 10.0;            // Highest peak
    peaks_width[0] = 1.0;              // Narrowest peak
    
    // 100 local optima with random positions
    for (int i = 1; i < 101; i++) {
        peaks_position[i].resize(n);
        for (int j = 0; j < n; j++) {
            peaks_position[i][j] = position_dist(generator);
        }
        peaks_height[i] = height_dist(generator);
        peaks_width[i] = width_dist(generator);
    }
}

double Gallagher101::gaussianPeak(const Data& x, const Data& center, double height, double width) const
{
    double distance_sq = 0.0;
    for (size_t i = 0; i < x.size(); i++) {
        distance_sq += (x[i] - center[i]) * (x[i] - center[i]);
    }
    return height * exp(-distance_sq / (2.0 * width * width));
}

double Gallagher101::funmin(Data &x)
{
    double max_value = -std::numeric_limits<double>::max();
    
    for (int i = 0; i < 101; i++) {
        double value = gaussianPeak(x, peaks_position[i], peaks_height[i], peaks_width[i]);
        if (value > max_value) {
            max_value = value;
        }
    }
    
    return 10.0 - max_value; // Transform so global minimum is 0 at origin
}

Data Gallagher101::gradient(Data &x)
{
    Data g(getDimension(), 0.0);
    
    // Find which peak gives the maximum value
    int max_index = 0;
    double max_value = -std::numeric_limits<double>::max();
    std::vector<double> peak_values(101);
    
    for (int i = 0; i < 101; i++) {
        peak_values[i] = gaussianPeak(x, peaks_position[i], peaks_height[i], peaks_width[i]);
        if (peak_values[i] > max_value) {
            max_value = peak_values[i];
            max_index = i;
        }
    }
    
    // Compute gradient for the dominant peak
    for (int j = 0; j < getDimension(); j++) {
        g[j] = -peak_values[max_index] * (x[j] - peaks_position[max_index][j]) / 
               (peaks_width[max_index] * peaks_width[max_index]);
    }
    
    return g;
}

void Gallagher101::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    initializePeaks();
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++) {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
