#ifndef BHO_H
#define BHO_H
#include <OPTIMUS/optimizer.h>
#include <random>
#include <vector>
#include <chrono>

class BHO : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration;
    double localSearchRate, bestF2x;

    // Αρχικές “βιολογικές” παράμετροι (όπως στην κλασική BHO)
    double wound_strength, wp, hr;

    // Ήπιο DE(best/1,bin) (υπήρχε ήδη)
    double de_rate, de_F, de_CR;

    // Πληθυσμός
    std::vector<double> fitnessArray, bestSample;
    std::vector<std::vector<double>> samples;
    std::chrono::time_point<std::chrono::system_clock> before, after;

    // RNG
    std::mt19937 rng;
    std::uniform_real_distribution<double> uniform_dist;
    std::normal_distribution<double> normal_dist;

    // ========= HYPER-RAGE: Υπερ-επιθετικά extensions στο πνεύμα BHO =========
    // Scar Map ανά διάσταση + momentum κατεύθυνσης
    bool   scar_enabled;
    double scar_lr;
    double scar_pmin, scar_pmax;
    double scar_smin, scar_smax;
    int    bandage_len;
    int    sinceBest;

    std::vector<double> wound_p_dim;              // P(wound) per dim
    std::vector<double> wound_s_dim;              // strength scale per dim
    std::vector<std::vector<int>> bandage;        // bandage counters [i][j]
    std::vector<double> scar_momentum;            // (-1..+1) trend per dim
    double mom_decay;                              // momentum “μάθηση”
    double mom_bias;                               // bias προς momentum

    // Hot-dims focus (μαθαίνουμε “καυτές” διαστάσεις)
    std::vector<double> dim_score;                 // score ανά διάσταση
    double dim_decay;                              // ρυθμός λήθης score
    int    hot_k;                                  // top-K διαστάσεις
    double hot_boost_p, hot_boost_s;               // ενίσχυση P/scale σε hot dims

    // Rage & Hyper-Rage bursts
    bool   rage_enabled;
    int    rage_stagn_thr;       // no-improve iters για Rage
    int    rage_len;             // διάρκεια Rage
    double rage_p_mult, rage_s_mult;
    bool   rage_ignore_bandage;
    int    rage_timer;

    bool   hyper_rage_enabled;
    int    rage2_stagn_thr;      // no-improve iters για Hyper-Rage
    int    rage2_len;            // διάρκεια Hyper-Rage
    double rage2_p_mult, rage2_s_mult;
    int    hyper_rage_timer;

    // Lévy-wounds (βαριές ουρές)
    bool   levy_enabled;
    double levy_alpha, levy_scale, levy_sigma_u;

    // Alpha-Strike (εκρηκτικό wound σε λίγες διαστάσεις)
    double alpha_strike_rate;
    double alpha_strike_scale;

    // Soft stagnation boost (εκτός Rage)
    int    stagn_thr_soft;
    double stagn_boost;

    // Καταστροφικό micro-reset γύρω από το best (για λίγους)
    bool   cat_reset_enabled;
    int    cat_reset_thr;        // no-improve iters για reset
    double cat_reset_frac;       // ποσοστό πληθυσμού
    double cat_sigma;            // σ του reset γύρω από best

    // Healing μεταβολές σε burst & cooldown
    double heal_rage_reduce;     // μείωση healing σε Rage
    double heal_post_cool;       // αύξηση healing μετά το burst
    int    cooldown_len;         // διάρκεια cooldown
    int    cooldown_timer;

    // Πρόγραμμα έντασης πληγών
    double wound_strength_init;

    // === Βοηθητικά ===
    void apply_wounds(int i, std::vector<double>& solution, double curr_wound);
    void heal_wounds(std::vector<double>& solution);
    void de_move(int i, double F, double CR);
    double levy_step();

    void apply_alpha_strike(std::vector<double>& x, double curr_wound);
    void catastrophic_micro_reset();
    std::vector<int> topK_dims(int K);

public:
    BHO();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~BHO();
};

#endif // BHO_H
