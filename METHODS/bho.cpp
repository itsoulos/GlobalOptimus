#include <METHODS/bho.h>
#include <random>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>
#include <limits>

using std::vector;

// ------------------------------
// Constructor + “κουμπιά”
// ------------------------------
BHO::BHO()
{
    before = std::chrono::system_clock::now();

    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 1000000, "Maximum iters"));
    addParam(Parameter("localSearchRate", 0.00, 0.0, 1.0, "Localsearch rate"));

    // BHO βασικά
    addParam(Parameter("wound_strength", 0.14, 0.0, 1.0, "Injury intensity"));
    addParam(Parameter("wp", 0.38, 0.0, 1.0,"Injury probability per dimension"));
    addParam(Parameter("hr", 0.8, 0.0, 1.0, "Healing probability per dimension"));

    // Ήπιο DE(best/1,bin)
    addParam(Parameter("de_rate", 0.15, 0.0, 1.0, "Probability of DE(best/1,bin) move"));
    addParam(Parameter("de_F", 0.7, 0.0, 2.0, "DE scale factor F"));
    addParam(Parameter("de_CR", 0.5, 0.0, 1.0, "DE crossover rate CR"));

    // Scar + Momentum
    addParam(Parameter("scar_bho", "yes", "Enable Scar-Map (yes/no)"));
    addParam(Parameter("scar_lr", 0.06, 0.001, 0.5, "Scar learning rate"));
    addParam(Parameter("bandage_len", 4, 0, 50, "Freeze improved dims for k iters"));
    addParam(Parameter("scar_pmin", 0.05, 0.0, 1.0, "Min wound prob per dim"));
    addParam(Parameter("scar_pmax", 0.98, 0.0, 1.0, "Max wound prob per dim"));
    addParam(Parameter("scar_smin", 0.50, 0.0, 5.0, "Min wound strength scale per dim"));
    addParam(Parameter("scar_smax", 3.00, 0.0, 5.0, "Max wound strength scale per dim"));
    addParam(Parameter("mom_decay", 0.25, 0.0, 1.0, "Momentum update (per success)"));
    addParam(Parameter("mom_bias", 0.75, 0.0, 2.0, "Bias strength towards momentum"));

    // Hot-dims
    addParam(Parameter("dim_decay", 0.05, 0.0, 1.0, "Per-iter decay of dim score"));
    addParam(Parameter("hot_k", 6, 0, 200, "Top-K hot dims to boost"));
    addParam(Parameter("hot_boost_p", 1.5, 1.0, 10.0, "Prob boost on hot dims"));
    addParam(Parameter("hot_boost_s", 1.6, 1.0, 10.0, "Strength boost on hot dims"));

    // Rage
    addParam(Parameter("rage_bho", "yes", "Enable RAGE bursts (yes/no)"));
    addParam(Parameter("rage_stagn_thr", 12, 1, 1000, "No-improve iters -> RAGE"));
    addParam(Parameter("rage_len", 10, 1, 200, "RAGE duration"));
    addParam(Parameter("rage_p_mult", 2.2, 1.0, 20.0, "RAGE P multiplier"));
    addParam(Parameter("rage_s_mult", 2.8, 1.0, 20.0, "RAGE S multiplier"));
    addParam(Parameter("rage_ignore_bandage", "yes", "Ignore bandage during RAGE (yes/no)"));

    // Hyper-Rage (βαρύτερο burst)
    addParam(Parameter("hyper_rage", "yes", "Enable Hyper-RAGE (yes/no)"));
    addParam(Parameter("rage2_stagn_thr", 28, 1, 2000, "No-improve iters -> Hyper-RAGE"));
    addParam(Parameter("rage2_len", 12, 1, 200, "Hyper-RAGE duration"));
    addParam(Parameter("rage2_p_mult", 3.0, 1.0, 30.0, "Hyper-RAGE P multiplier"));
    addParam(Parameter("rage2_s_mult", 3.5, 1.0, 30.0, "Hyper-RAGE S multiplier"));

    // Lévy-wounds
    addParam(Parameter("levy_bho", "yes", "Use Lévy-wounds (yes/no)"));
    addParam(Parameter("levy_alpha", 1.5, 1.1, 2.0, "Lévy alpha"));
    addParam(Parameter("levy_scale", 0.6, 0.0, 5.0, "Lévy scale"));

    // Alpha-Strike
    addParam(Parameter("alpha_strike_rate", 0.10, 0.0, 1.0, "Chance of Alpha-Strike per individual"));
    addParam(Parameter("alpha_strike_scale", 0.9, 0.0, 5.0, "Alpha-Strike step scale (×range)"));

    // Soft stagnation
    addParam(Parameter("stagn_thr_soft", 10, 0, 200, "Soft stagnation threshold"));
    addParam(Parameter("stagn_boost", 0.40, 0.0, 5.0, "Extra wound factor when soft-stagnant"));

    // Catastrophic micro-reset
    addParam(Parameter("cat_reset", "yes", "Enable catastrophic micro-reset (yes/no)"));
    addParam(Parameter("cat_reset_thr", 40, 1, 5000, "No-improve iters -> micro-reset"));
    addParam(Parameter("cat_reset_frac", 0.08, 0.0, 1.0, "Fraction of pop to reset"));
    addParam(Parameter("cat_sigma", 0.25, 0.0, 5.0, "Sigma (×range) around best for reset"));

    // Healing μεταβολές
    addParam(Parameter("heal_rage_reduce", 0.35, 0.0, 1.0, "Reduce healing rate in Rage"));
    addParam(Parameter("heal_post_cool", 0.25, 0.0, 10.0, "Increase healing after Rage"));
    addParam(Parameter("cooldown_len", 8, 0, 200, "Cooldown iterations after bursts"));
}

void BHO::init()
{
    dimension     = myProblem->getDimension();
    population    = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localSearchRate").getValue().toDouble();

    // Βασικά BHO
    wound_strength    = getParam("wound_strength").getValue().toDouble();
    wp = getParam("wound_probability").getValue().toDouble();
    hr      = getParam("healing_rate").getValue().toDouble();

    // DE
    de_rate = getParam("de_rate").getValue().toDouble();
    de_F    = getParam("de_F").getValue().toDouble();
    de_CR   = getParam("de_CR").getValue().toDouble();

    // Scar + momentum
    scar_enabled = (getParam("scar_bho").getValue() == "yes");
    scar_lr      = getParam("scar_lr").getValue().toDouble();
    bandage_len  = getParam("bandage_len").getValue().toInt();
    scar_pmin    = getParam("scar_pmin").getValue().toDouble();
    scar_pmax    = getParam("scar_pmax").getValue().toDouble();
    scar_smin    = getParam("scar_smin").getValue().toDouble();
    scar_smax    = getParam("scar_smax").getValue().toDouble();
    mom_decay    = getParam("mom_decay").getValue().toDouble();
    mom_bias     = getParam("mom_bias").getValue().toDouble();

    // Hot-dims
    dim_decay    = getParam("dim_decay").getValue().toDouble();
    hot_k        = getParam("hot_k").getValue().toInt();
    hot_boost_p  = getParam("hot_boost_p").getValue().toDouble();
    hot_boost_s  = getParam("hot_boost_s").getValue().toDouble();

    // Rage
    rage_enabled        = (getParam("rage_bho").getValue() == "yes");
    rage_stagn_thr      = getParam("rage_stagn_thr").getValue().toInt();
    rage_len            = getParam("rage_len").getValue().toInt();
    rage_p_mult         = getParam("rage_p_mult").getValue().toDouble();
    rage_s_mult         = getParam("rage_s_mult").getValue().toDouble();
    rage_ignore_bandage = (getParam("rage_ignore_bandage").getValue() == "yes");
    rage_timer          = 0;

    // Hyper-Rage
    hyper_rage_enabled = (getParam("hyper_rage").getValue() == "yes");
    rage2_stagn_thr    = getParam("rage2_stagn_thr").getValue().toInt();
    rage2_len          = getParam("rage2_len").getValue().toInt();
    rage2_p_mult       = getParam("rage2_p_mult").getValue().toDouble();
    rage2_s_mult       = getParam("rage2_s_mult").getValue().toDouble();
    hyper_rage_timer   = 0;

    // Lévy
    levy_enabled = (getParam("levy_bho").getValue() == "yes");
    levy_alpha   = getParam("levy_alpha").getValue().toDouble();
    levy_scale   = getParam("levy_scale").getValue().toDouble();
    {
        double a = std::min(2.0, std::max(1.01, levy_alpha));
        auto Gamma = [](double z)->double { return std::tgamma(z); };
        double num = Gamma(1.0 + a) * std::sin(M_PI * a / 2.0);
        double den = Gamma((1.0 + a) / 2.0) * a * std::pow(2.0, (a - 1.0) / 2.0);
        levy_sigma_u = std::pow(num / den, 1.0 / a);
    }

    // Alpha-Strike
    alpha_strike_rate  = getParam("alpha_strike_rate").getValue().toDouble();
    alpha_strike_scale = getParam("alpha_strike_scale").getValue().toDouble();

    // Soft stagnation
    stagn_thr_soft = getParam("stagn_thr_soft").getValue().toInt();
    stagn_boost    = getParam("stagn_boost").getValue().toDouble();

    // Catastrophic reset
    cat_reset_enabled = (getParam("cat_reset").getValue() == "yes");
    cat_reset_thr     = getParam("cat_reset_thr").getValue().toInt();
    cat_reset_frac    = getParam("cat_reset_frac").getValue().toDouble();
    cat_sigma         = getParam("cat_sigma").getValue().toDouble();

    // Healing αλλαγές
    heal_rage_reduce = getParam("heal_rage_reduce").getValue().toDouble();
    heal_post_cool   = getParam("heal_post_cool").getValue().toDouble();
    cooldown_len     = getParam("cooldown_len").getValue().toInt();
    cooldown_timer   = 0;

    // RNG
    std::random_device rd; rng.seed(rd());
    uniform_dist = std::uniform_real_distribution<double>(0.0, 1.0);
    normal_dist  = std::normal_distribution<double>(0.0, 1.0);

    iteration = 0;
    bestF2x = 1e+100;
    sampleFromProblem(population, samples, fitnessArray);

    // Αρχικός best
    for (int i = 0; i < population; i++) {
        double y = fitnessArray.at(i);
        if (i == 0 || y < bestF2x) { bestF2x = y; bestSample = samples.at(i); }
    }

    // Καταστάσεις
    wound_strength_init = wound_strength;
    sinceBest = 0;

    // Scar/hot
    wound_p_dim.assign(dimension, wp);
    wound_s_dim.assign(dimension, 1.0);
    bandage.assign(population, vector<int>(dimension, 0));
    scar_momentum.assign(dimension, 0.0);
    dim_score.assign(dimension, 0.0);

    similarity.setSimilarityIterations(12);
    //myProblem->setMaxFunctionCalls(150000);
}

// ------------------------- Lévy step (Mantegna) -------------------------
double BHO::levy_step()
{
    double u = levy_sigma_u * normal_dist(rng);
    double v = normal_dist(rng);
    double denom = std::pow(std::fabs(v), 1.0 / std::max(1.01, levy_alpha));
    if (denom < 1e-16) denom = 1e-16;
    return levy_scale * (u / denom);
}

// ----------------------- Top-K helper (hot dims) ------------------------
std::vector<int> BHO::topK_dims(int K)
{
    if (K <= 0) return {};
    vector<int> idx(dimension); std::iota(idx.begin(), idx.end(), 0);
    std::partial_sort(idx.begin(), idx.begin() + std::min(K, dimension), idx.end(),
        [&](int a, int b){ return dim_score[a] > dim_score[b]; });
    idx.resize(std::min(K, dimension));
    return idx;
}

// ---------------------------- Alpha-Strike ------------------------------
void BHO::apply_alpha_strike(std::vector<double>& x, double curr_wound)
{
    if (uniform_dist(rng) >= alpha_strike_rate) return;

    const vector<double> lb = myProblem->getLeftMargin();
    const vector<double> ub = myProblem->getRightMargin();

    // διάλεξε m καυτές διαστάσεις (ή 1 αν λίγες)
    int m = std::max(1, hot_k/2);
    vector<int> hot = topK_dims(m);
    if (hot.empty()) {
        hot.push_back(rng() % dimension);
    }

    for (int j : hot)
    {
        double range = ub[j] - lb[j];
        double jump  = alpha_strike_scale * range * (levy_enabled ? (1.0 + std::fabs(levy_step())) : (1.0 + std::fabs(normal_dist(rng))));
        // κατεύθυνση: μισές φορές προς best, μισές random/momentum
        double dir = 0.0;
        if (uniform_dist(rng) < 0.5) {
            dir = (bestSample[j] - x[j] >= 0.0 ? +1.0 : -1.0);
        } else {
            double sgn = (scar_momentum[j] >= 0.0 ? +1.0 : -1.0);
            dir = (uniform_dist(rng) < 0.7 ? sgn : (uniform_dist(rng) < 0.5 ? +1.0 : -1.0));
        }
        double step = curr_wound * dir * jump;
        x[j] = std::clamp(x[j] + step, lb[j], ub[j]);
    }
}

// ------------------------------ Wounds ----------------------------------
void BHO::apply_wounds(int i, std::vector<double>& solution, double curr_wound)
{
    const vector<double> lb = myProblem->getLeftMargin();
    const vector<double> ub = myProblem->getRightMargin();

    // Soft stagnation boost
    double wound_boost = 1.0;
    if (sinceBest >= stagn_thr_soft) {
        double f = 1.0 + stagn_boost * std::min(1.0, (double)(sinceBest - stagn_thr_soft) / std::max(1, stagn_thr_soft));
        wound_boost *= f;
    }

    // Rage/Hyper-Rage
    bool rage1 = (rage_timer > 0);
    bool rage2 = (hyper_rage_timer > 0);

    // Hot-dims set
    vector<int> hot = topK_dims(hot_k);

    for (int j = 0; j < dimension; j++)
    {
        // bandage bypass σε bursts
        if (!( (rage1 && rage_ignore_bandage) || rage2 )) {
            if (scar_enabled && bandage_len > 0 && bandage[i][j] > 0) continue;
        }

        // base probability
        double p = scar_enabled ? std::clamp(wound_p_dim[j], scar_pmin, scar_pmax) : wp;

        // boost για hot dims
        if (!hot.empty() && std::find(hot.begin(), hot.end(), j) != hot.end()) {
            p = std::min(0.999, p * hot_boost_p);
        }

        // Rage/Hyper-Rage πολλαπλασιαστές
        if (rage1) p = std::min(0.999, p * rage_p_mult);
        if (rage2) p = 0.999; // Hyper-Rage: σχεδόν πάντα πληγή

        if (uniform_dist(rng) < p)
        {
            // ένταση
            double scale = scar_enabled ? std::clamp(wound_s_dim[j], scar_smin, scar_smax) : 1.0;
            if (!hot.empty() && std::find(hot.begin(), hot.end(), j) != hot.end()) scale *= hot_boost_s;
            if (rage1) scale *= rage_s_mult;
            if (rage2) scale *= rage2_s_mult;

            // βήμα: Lévy ή Gaussian
            double base = levy_enabled ? levy_step() : normal_dist(rng);
            double range = ub[j] - lb[j];

            // momentum bias
            if (scar_enabled && std::fabs(scar_momentum[j]) > 1e-8) {
                double sgn = (scar_momentum[j] >= 0.0 ? 1.0 : -1.0);
                base = (1.0 - mom_bias) * base + mom_bias * std::fabs(base) * sgn;
            }

            double disturbance = curr_wound * wound_boost * scale * base * range;
            solution[j] = std::clamp(solution[j] + disturbance, lb[j], ub[j]);
        }
    }

    // Εκρηκτικό alpha-strike στο τέλος (σπάνιο αλλά δυνατό)
    apply_alpha_strike(solution, curr_wound);
}

// ------------------------------- Heal -----------------------------------
void BHO::heal_wounds(std::vector<double>& solution)
{
    const vector<double> lb = myProblem->getLeftMargin();
    const vector<double> ub = myProblem->getRightMargin();

    bool rage1 = (rage_timer > 0);
    bool rage2 = (hyper_rage_timer > 0);

    // προσαρμοσμένο healing rate
    double hrate = hr;
    if (rage1 || rage2) hrate = std::max(0.0, hr * (1.0 - heal_rage_reduce));
    else if (cooldown_timer > 0) hrate = std::min(1.0, hr * (1.0 + heal_post_cool));

    double alpha = 0.15 + 0.35 * hrate;

    for (int j = 0; j < dimension; j++)
    {
        if (uniform_dist(rng) < hrate)
        {
            solution[j] += alpha * (bestSample[j] - solution[j]);
            solution[j] = std::clamp(solution[j], lb[j], ub[j]);
        }
    }

    // σε bursts: μικρή πιθανότητα “καυτηρίασης” (copy-from-best)
    if (rage1 || rage2) {
        double copy_rate = rage2 ? 0.12 : 0.08;
        for (int j = 0; j < dimension; j++) {
            if (uniform_dist(rng) < copy_rate) {
                solution[j] = bestSample[j];
            }
        }
    }
}

// ----------------------------- DE(best/1,bin) ---------------------------
void BHO::de_move(int i, double F, double CR)
{
    int r1, r2;
    do { r1 = rng() % population; } while (r1 == i);
    do { r2 = rng() % population; } while (r2 == i || r2 == r1);

    const vector<double> lb = myProblem->getLeftMargin();
    const vector<double> ub = myProblem->getRightMargin();

    int jrand = rng() % dimension;
    for (int j = 0; j < dimension; ++j)
    {
        if (uniform_dist(rng) < CR || j == jrand)
        {
            double v = bestSample[j] + F * (samples[r1][j] - samples[r2][j]);
            samples[i][j] = std::clamp(v, lb[j], ub[j]);
        }
    }
}

// ----------------------- Catastrophic micro-reset -----------------------
void BHO::catastrophic_micro_reset()
{
    if (!cat_reset_enabled) return;
    if (sinceBest < cat_reset_thr) return;

    const vector<double> lb = myProblem->getLeftMargin();
    const vector<double> ub = myProblem->getRightMargin();

    int elite = 0; double bestVal = fitnessArray[0];
    for (int i=1;i<population;++i) if (fitnessArray[i] < bestVal) { bestVal = fitnessArray[i]; elite = i; }

    int nreset = std::max(1, (int)std::round(cat_reset_frac * population));
    for (int k=0; k<nreset; ++k) {
        int i; do { i = rng() % population; } while (i == elite);
        vector<double> cand = samples[i];
        for (int j=0;j<dimension;++j) {
            double range = ub[j] - lb[j];
            cand[j] = bestSample[j] + cat_sigma * range * normal_dist(rng);
            cand[j] = std::clamp(cand[j], lb[j], ub[j]);
        }
        double f = myProblem->statFunmin(cand);
        if (f < fitnessArray[i]) {
            samples[i] = cand; fitnessArray[i] = f;
            if (f < bestF2x) { bestF2x = f; bestSample = cand; }
        }
    }
}

// ------------------------------- STEP -----------------------------------
void BHO::step()
{
    iteration++;

    // 1) Elitism
    int elite = 0;
    {
        double bestVal = fitnessArray[0];
        for (int i = 1; i < population; ++i)
            if (fitnessArray[i] < bestVal) { bestVal = fitnessArray[i]; elite = i; }
        bestF2x = bestVal;
        bestSample = samples[elite];
    }
    double prevBest = bestF2x;

    // 2) Πρόγραμμα wound strength
    double curr_wound = std::max(0.05 * wound_strength_init,
                                 wound_strength_init * (1.0 - (double)iteration / std::max(1, maxIterations)));

    // 3) Rage / Hyper-Rage timers
    if (rage_enabled) {
        if (sinceBest >= rage_stagn_thr && rage_timer == 0) rage_timer = rage_len;
        else if (rage_timer > 0) rage_timer--;
    }
    if (hyper_rage_enabled) {
        if (sinceBest >= rage2_stagn_thr && hyper_rage_timer == 0) hyper_rage_timer = rage2_len;
        else if (hyper_rage_timer > 0) hyper_rage_timer--;
    }

    // 3b) cooldown
    if (cooldown_timer > 0) cooldown_timer--;

    // 4) Καταστροφικό micro-reset (σπάνιο)
    catastrophic_micro_reset();

    // 5) Για κάθε άτομο
    for (int i = 0; i < population; i++)
    {
        if (i == elite) continue;

        const vector<double> x_old = samples[i];
        const double f_old = fitnessArray[i];

        // DE (ήπιο)
        if (uniform_dist(rng) < de_rate) de_move(i, de_F, de_CR);

        // Wounds + Heal
        vector<double> trial = samples[i];
        apply_wounds(i, trial, curr_wound);
        heal_wounds(trial);

        // Δείκτες αλλαγής/κατεύθυνσης
        vector<char> changed(dimension, 0), towardBest(dimension, 0), sign_dir(dimension, 0);
        for (int j = 0; j < dimension; ++j) {
            double dj = trial[j] - x_old[j];
            if (std::fabs(dj) > 1e-12) {
                changed[j] = 1;
                double tb = (bestSample[j] - x_old[j]) * dj;
                towardBest[j] = (tb > 0.0) ? 1 : 0;
                sign_dir[j] = (dj >= 0.0) ? 1 : -1;
            }
        }

        // Αξιολόγηση & Greedy
        double f_trial = myProblem->statFunmin(trial);
        bool accepted = (f_trial < f_old);

        if (accepted)
        {
            samples[i] = trial;
            fitnessArray[i] = f_trial;
            if (f_trial < bestF2x) { bestF2x = f_trial; bestSample = samples[i]; }

            // Scar updates + Momentum + Dim-score
            if (scar_enabled) {
                for (int j=0;j<dimension;++j) if (changed[j]) {
                    double gP = scar_lr * (0.5 + 0.5 * (int)towardBest[j]);
                    double gS = scar_lr * (0.25 + 0.75 * (int)towardBest[j]);
                    wound_p_dim[j] = std::clamp(wound_p_dim[j] + gP, scar_pmin, scar_pmax);
                    wound_s_dim[j] = std::clamp(wound_s_dim[j] + gS, scar_smin, scar_smax);

                    // momentum
                    double sgn = (double)sign_dir[j];
                    scar_momentum[j] = (1.0 - mom_decay) * scar_momentum[j] + mom_decay * sgn;

                    // bandage (αν όχι bypass σε burst)
                    bool bypass = ( (rage_timer>0 && rage_ignore_bandage) || (hyper_rage_timer>0) );
                    if (towardBest[j] && bandage_len > 0 && !bypass) bandage[i][j] = bandage_len;

                    // dim score boost
                    dim_score[j] += 1.0 + 0.5 * (int)towardBest[j];
                }
            }
        }
        else
        {
            // rollback
            samples[i] = x_old;
            fitnessArray[i] = f_old;

            if (scar_enabled) {
                for (int j=0;j<dimension;++j) if (changed[j]) {
                    double gP = scar_lr * (0.5 + 0.5 * (int)!towardBest[j]);
                    double gS = scar_lr * (0.25 + 0.75 * (int)!towardBest[j]);
                    wound_p_dim[j] = std::clamp(wound_p_dim[j] - gP, scar_pmin, scar_pmax);
                    wound_s_dim[j] = std::clamp(wound_s_dim[j] - gS, scar_smin, scar_smax);
                    scar_momentum[j] *= (1.0 - 0.5 * mom_decay);
                    dim_score[j] *= (1.0 - 0.25 * dim_decay);
                }
            }
        }
    }

    // 6) Τοπική αναζήτηση (αν ζητηθεί)
    if (localSearchRate > 0.0)
    {
        int elite2 = 0; double bestVal2 = fitnessArray[0];
        for (int i=1;i<population;++i) if (fitnessArray[i] < bestVal2) { bestVal2 = fitnessArray[i]; elite2 = i; }

        for (int i = 0; i < population; i++)
        {
            if (i == elite2) continue;
            if (uniform_dist(rng) < localSearchRate)
            {
                const vector<double> x_old = samples[i];
                const double f_old = fitnessArray[i];

                double f_new = localSearch(samples[i]);

                if (f_new < f_old) {
                    fitnessArray[i] = f_new;
                    if (f_new < bestF2x) { bestF2x = f_new; bestSample = samples[i]; }
                } else {
                    samples[i] = x_old;
                    fitnessArray[i] = f_old;
                }
            }
        }
    }

    // 7) Στασιμότητα, cooldown, decay χαρτών
    if (bestF2x < prevBest) {
        sinceBest = 0;
        // ξεκινά cooldown όταν λήγει burst
        if ((rage_timer == 0 && hyper_rage_timer == 0) && cooldown_len > 0) {
            cooldown_timer = cooldown_len;
        }
    } else {
        sinceBest++;
    }

    if (scar_enabled) {
        for (int j=0;j<dimension;++j) dim_score[j] *= (1.0 - dim_decay);
        if (bandage_len > 0) {
            for (int i=0;i<population;++i)
                for (int j=0;j<dimension;++j)
                    if (bandage[i][j] > 0) bandage[i][j]--;
        }
    }
}

bool BHO::terminated()
{

    QString term = terminationMethod;
    if (iteration >= maxIterations)
        return true;
    if (term == "maxiters")
        return iteration >= maxIterations;
    else if (term == "doublebox")
        return doubleBox.terminate(bestF2x);
    else if (term == "similarity")
        return similarity.terminate(bestF2x);
   // if (myProblem->getFunctionCalls() >= myProblem->getMaxFunctionCalls())
   //     return true;
    return false;
}

void BHO::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (iteration%50==0 && debug)
        methodLogger->printMessage(QString::asprintf("BHO-HYPER-RAGE. Iteration=%5d BEST=%20.10lg", iteration, bestF2x));
}

void BHO::done()
{
    bestF2x = localSearch(bestSample);
    after = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
    std::cout << "Duration: " << (double)ms / 1000.0 << " sec" << std::endl;
}

BHO::~BHO() {}
