// METHODS/trio.h
#ifndef TRIO_H
#define TRIO_H

#include <OPTIMUS/optimizer.h>
#include <random>
#include <vector>
#include <array>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>
#include <iostream>
#include <iomanip>

class TRIO : public Optimizer
{
public:
    TRIO();
    void init() override;
    void step() override;
    bool terminated() override;
    void done() override;
    void showDebug() override;
    ~TRIO() override;

private:
    // Core state
    int dimension{0}, population{0}, iteration{0};
    int maxIterations;
    std::vector<std::vector<double>> samples;
    std::vector<double> fitnessArray;
    std::vector<double> bestSample;
    double bestF2x{std::numeric_limits<double>::infinity()};

    // Bounds
    std::vector<double> lb_, ub_;

    // RNG
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> U{0.0,1.0};
    std::uniform_int_distribution<int> DIpop; // [0, pop-1]
    std::uniform_int_distribution<int> DIdim; // [0, dim-1]
    std::normal_distribution<double> N0{0.0,1.0};

    // Timing
    std::chrono::time_point<std::chrono::system_clock> before, after;

    // ---- Hyper-params (DE-only core) ----
    int    trials_per_agent{4};     // # trials per individual (keep best)
    double agent_fraction{0.55};    // mini-batch για πολλές iterations
    int    startAgent{0};

    // jDE self-adaptation
    double tauF{0.10}, tauCR{0.10}; // prob to resample F_i, CR_i
    double F_lo{0.1}, F_hi{1.2};    // bounds for F
    double CR_lo{0.0}, CR_hi{0.95}; // bounds for CR

    // pbest fraction (για pbest/1/bin)
    double pbest_frac{0.10};

    // Stagnation → micro-restart
    int    stagn_iters{0};
    int    stagnation_trigger{18};
    double restart_frac{0.10};
    double restart_sigma{0.20};

    // Per-individual params (jDE)
    std::vector<double> Fi, CRi;

    // -------- LANDScope (optional landscape-aware bootstrapping) --------
    QString landscape_mode{"off"}; // "off" | "probe"
    double probe_frac{0.004};           // fraction of max FC for diagnostics
    int    probe_steps{32};             // steps in random walk
    int    probe_kdims{0};              // number of axis dims for separability probe
    struct LStats { double rugged{0.5}, separ{0.5}, scale{1.0}; } lstats;
    void   probeLandscape_();           // runs once in init if enabled
    void   applyLandscapeProfile_();    // choose Aggressive / Balanced / Exploit

    // Helpers
    inline double clamp(double v, double lo, double hi) const { return v<lo?lo:(v>hi?hi:v); }
    inline double range(int j) const { return ub_[j]-lb_[j]; }

    // Operators
    void   make_trial_best1   (int i, const std::vector<double>& xi, std::vector<double>& trial, double Fi_, double CRi_);
    void   make_trial_ctobest1(int i, const std::vector<double>& xi, std::vector<double>& trial, double Fi_, double CRi_);
    void   make_trial_pbest1  (int i, const std::vector<double>& xi, std::vector<double>& trial, double Fi_, double CRi_);

    bool   line_refine(const std::vector<double>& base, const std::vector<double>& trial_in, std::vector<double>& trial_out);

    // Utils
    int    eliteIndex_() const;
    void   microRestart_();
};

#endif // TRIO_H
