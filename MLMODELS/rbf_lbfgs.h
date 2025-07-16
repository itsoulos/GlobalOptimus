#ifndef RBF_LBFGS_H
#define RBF_LBFGS_H
#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <limits>
#include <cassert>

using std::vector;
using std::cout;
using std::endl;

struct Options {
    int maxIter = 150;
    int m = 5;
    double epsilon = 1e-6;
    double alpha0 = 1.0;
    double ftol = 1e-4;
};
class LBFGS {
public:


    template<typename Function>
    static int minimize(Function& func, vector<double>& x, double& fx, const Options& opts = Options()) {
        size_t n = x.size();
        vector<double> g(n), g_old(n), d(n), x_old(n), q(n), alpha(opts.m), rho(opts.m);
        vector<vector<double>> s(opts.m, vector<double>(n));
        vector<vector<double>> y(opts.m, vector<double>(n));

        fx = func(x, g);
        d = g;
        for (double& di : d) di *= -1;

        int k = 0, bound = 0;
        while (k < opts.maxIter) {
            double step = lineSearch(func, x, fx, g, d, opts);

            x_old = x;
            g_old = g;

            for (size_t i = 0; i < n; ++i)
                x[i] += step * d[i];

            double fx_new = func(x, g);

            for (size_t i = 0; i < n; ++i) {
                s[k % opts.m][i] = x[i] - x_old[i];
                y[k % opts.m][i] = g[i] - g_old[i];
            }

            rho[k % opts.m] = 1.0 / dot(y[k % opts.m], s[k % opts.m]);
            q = g;
            for (int i = std::min(k, opts.m - 1); i >= 0; --i) {
                alpha[i] = rho[i] * dot(s[i], q);
                for (size_t j = 0; j < n; ++j)
                    q[j] -= alpha[i] * y[i][j];
            }

            double H0 = dot(s[(k - 1 + opts.m) % opts.m], y[(k - 1 + opts.m) % opts.m]) /
                        dot(y[(k - 1 + opts.m) % opts.m], y[(k - 1 + opts.m) % opts.m]);
            for (size_t j = 0; j < n; ++j)
                d[j] = H0 * q[j];

            for (int i = 0; i < std::min(k, opts.m - 1); ++i) {
                double beta = rho[i] * dot(y[i], d);
                for (size_t j = 0; j < n; ++j)
                    d[j] += s[i][j] * (alpha[i] - beta);
            }

            for (double& di : d) di *= -1;

            if (norm(g) < opts.epsilon) break;
            ++k;
        }

        return k;
    }

private:
    template<typename Function>
    static double lineSearch(Function& func, const vector<double>& x, double fx,
                             const vector<double>& g, const vector<double>& d, const Options& opts) {
        double alpha = opts.alpha0;
        double phi0 = fx;
        double slope = dot(g, d);
        assert(slope < 0);

        vector<double> x_new = x, g_new;
        double phi;

        while (true) {
            for (size_t i = 0; i < x.size(); ++i)
                x_new[i] = x[i] + alpha * d[i];

            phi = func(x_new, g_new);
            if (phi <= phi0 + opts.ftol * alpha * slope)
                break;

            alpha *= 0.5;
            if (alpha < 1e-8) break;
        }
        return alpha;
    }

    static double dot(const vector<double>& a, const vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i)
            sum += a[i] * b[i];
        return sum;
    }

    static double norm(const vector<double>& a) {
        return sqrt(dot(a, a));
    }
};

#endif // RBF_LBFGS_H
