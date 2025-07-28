#include "gallagher21.h"
#include <cmath>
#include <random>
#include <algorithm>

Gallagher21::Gallagher21()
    : Problem(1)
{
}

void Gallagher21::initializePeaks()
{
    int n = getDimension();
    peaks_position.resize(21);
    peaks_height.resize(21);
    peaks_width.resize(21);
    
    // Random number generation setup with fixed seed for reproducibility
    std::default_random_engine generator(42);
    std::uniform_real_distribution<double> position_dist(-5.0, 5.0);
    
    // Configure peaks according to Gallagher's specifications
    // Global optimum
    peaks_position[0].resize(n, 0.0);
    peaks_height[0] = 10.0;
    peaks_width[0] = 1.0;
    
    // First group of 10 peaks
    for (int i = 1; i <= 10; i++) {
        peaks_position[i].resize(n);
        for (int j = 0; j < n; j++) {
            peaks_position[i][j] = position_dist(generator);
        }
        peaks_height[i] = 9.8;
        peaks_width[i] = 1.1 + (i-1)*0.1; // Gradually increasing widths
    }
    
    // Second group of 10 peaks
    for (int i = 11; i <= 20; i++) {
        peaks_position[i].resize(n);
        for (int j = 0; j < n; j++) {
            peaks_position[i][j] = position_dist(generator);
        }
        peaks_height[i] = 9.0 + (i-11)*0.1; // Gradually increasing heights
        peaks_width[i] = 1.1 + (i-11)*0.2;  // Wider than first group
    }
}

double Gallagher21::funmin(Data &x)
{
    double max_value = -std::numeric_limits<double>::max();
    
    for (int i = 0; i < 21; i++) {
        double distance_sq = 0.0;
        for (int j = 0; j < getDimension(); j++) {
            distance_sq += (x[j] - peaks_position[i][j]) * (x[j] - peaks_position[i][j]);
        }
        double value = peaks_height[i] * exp(-distance_sq / (2.0 * peaks_width[i] * peaks_width[i]));
        if (value > max_value) {
            max_value = value;
        }
    }
    
    return 10.0 - max_value; // Transform so global minimum is 0 at (0,0,...,0)
}

Data Gallagher21::gradient(Data &x)
{
    Data g;
    g.resize(getDimension(), 0.0);
    
    // Find which peak gives the maximum value
    int max_index = 0;
    double max_value = -std::numeric_limits<double>::max();
    std::vector<double> peak_values(21);
    
    for (int i = 0; i < 21; i++) {
        double distance_sq = 0.0;
        for (int j = 0; j < getDimension(); j++) {
            distance_sq += (x[j] - peaks_position[i][j]) * (x[j] - peaks_position[i][j]);
        }
        peak_values[i] = peaks_height[i] * exp(-distance_sq / (2.0 * peaks_width[i] * peaks_width[i]));
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

void Gallagher21::init(QJsonObject &params)
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
