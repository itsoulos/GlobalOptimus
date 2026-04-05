#include "usermethod.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

static inline double absd(double x) { return (x < 0.0) ? -x : x; }
static inline double clampd(double x, double lo, double hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

UserMethod::UserMethod(Problem *p)
{
    myProblem = p;

    // Defaults (must be overridable via setters / external params)
    popsize = 30;
    max_iters = 500;
    seed = 1;
    sampling_mode = "uniform"; // "uniform" or "gaussian"

    max_evals = 0;   // if 0 -> set in init() as popsize * max_iters
    tol = 1e-8;
    tol_window = 10;
    max_stall = 50;

    p_local = 0.2;
    local_every_R = 20;
    top_k = 3;
    local_steps = 50;
    local_lr = 0.05;

    iter = 0;
    stall = 0;
    tol_hits = 0;

    besty = 1e100;
    besty_old = 1e100;
}

double UserMethod::rand01() const
{
    double r = (double)rand() / (double)RAND_MAX;
    if (r < 1e-12) r = 1e-12;
    if (r > 1.0 - 1e-12) r = 1.0 - 1e-12;
    return r;
}

double UserMethod::randUniform(double a, double b) const
{
    return a + (b - a) * rand01();
}

double UserMethod::randNormal(double mean, double stddev) const
{
    // Box–Muller transform
    const double u1 = rand01();
    const double u2 = rand01();
    const double pi = 3.14159265358979323846;
    const double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * pi * u2);
    return mean + stddev * z0;
}

void UserMethod::enforceBounds(Data &x) const
{
    for (int j = 0; j < (int)x.size(); ++j)
        x[j] = clampd(x[j], L[j], R[j]);
}

int UserMethod::bestIndex() const
{
    int b = 0;
    for (int i = 1; i < (int)fit.size(); ++i)
        if (fit[i] < fit[b]) b = i;
    return b;
}

Data UserMethod::levyFlight(int d, double beta) const
{
    // Mantegna's algorithm for Levy flight steps
    const double pi = 3.14159265358979323846;

    const double num = std::tgamma(1.0 + beta) * std::sin(pi * beta / 2.0);
    const double den = std::tgamma((1.0 + beta) / 2.0) * beta * std::pow(2.0, (beta - 1.0) / 2.0);
    const double sigma_u = std::pow(num / den, 1.0 / beta);

    Data step;
    step.resize(d);

    for (int j = 0; j < d; ++j)
    {
        const double u = randNormal(0.0, sigma_u);
        const double v = randNormal(0.0, 1.0);
        step[j] = u / std::pow(absd(v) + 1e-12, 1.0 / beta);
    }
    return step;
}

void UserMethod::localOptimize(Data &x, double &y)
{
    // Simple projected gradient descent (hybrid local optimization)
    for (int s = 0; s < local_steps; ++s)
    {
        Data g = myProblem->gradient(x);

        Data xn = x;
        for (int j = 0; j < (int)xn.size(); ++j)
            xn[j] = xn[j] - local_lr * g[j];

        enforceBounds(xn);

        double yn = myProblem->statFunmin(xn);
        if (yn + tol < y)
        {
            x = xn;
            y = yn;
        }
        else
        {
            break;
        }
    }
}

// ---------------- setters ----------------
void UserMethod::setPopsize(int n)            { if (n > 1) popsize = n; }
void UserMethod::setMaxIters(int it)          { if (it > 0) max_iters = it; }
void UserMethod::setSeed(int s)               { seed = s; }
void UserMethod::setSamplingMode(const std::string &m) { sampling_mode = m; }

void UserMethod::setMaxEvals(int e)           { if (e > 0) max_evals = e; }
void UserMethod::setTol(double t)             { if (t > 0.0) tol = t; }
void UserMethod::setTolWindow(int g)          { if (g > 0) tol_window = g; }
void UserMethod::setMaxStall(int s)           { if (s > 0) max_stall = s; }

void UserMethod::setPLocal(double p)          { if (p >= 0.0 && p <= 1.0) p_local = p; }
void UserMethod::setLocalEveryR(int r)        { if (r >= 0) local_every_R = r; }
void UserMethod::setTopK(int k)               { if (k > 0) top_k = k; }
void UserMethod::setLocalSteps(int s)         { if (s > 0) local_steps = s; }
void UserMethod::setLocalLr(double lr)        { if (lr > 0.0) local_lr = lr; }

// ---------------- lifecycle ----------------
void UserMethod::init()
{
    srand(seed);

    const int d = myProblem->getDimension();
    L = myProblem->getLeftMargin();
    R = myProblem->getRightMargin();

    if (max_evals <= 0) max_evals = popsize * max_iters;

    hawks.assign(popsize, Data(d, 0.0));
    fit.assign(popsize, 1e100);

    besty = 1e100;
    besty_old = 1e100;
    bestx.assign(d, 0.0);

    iter = 0;
    stall = 0;
    tol_hits = 0;

    // Initial sampling (two modes)
    for (int i = 0; i < popsize; ++i)
    {
        Data x;
        if (sampling_mode == "gaussian")
        {
            x.resize(d);
            for (int j = 0; j < d; ++j)
            {
                const double mu = 0.5 * (L[j] + R[j]);
                const double sd = (R[j] - L[j]) / 6.0;
                x[j] = randNormal(mu, sd);
            }
            enforceBounds(x);
        }
        else
        {
            // uniform
            x = myProblem->getSample();
        }

        const double y = myProblem->statFunmin(x);
        hawks[i] = x;
        fit[i] = y;

        if (y < besty)
        {
            besty = y;
            bestx = x;
        }
    }
}

void UserMethod::step()
{
    const int d = myProblem->getDimension();
    const Data rabbit = bestx;

    // mean position Xm
    Data Xm(d, 0.0);
    for (int i = 0; i < popsize; ++i)
        for (int j = 0; j < d; ++j)
            Xm[j] += hawks[i][j];
    for (int j = 0; j < d; ++j)
        Xm[j] /= (double)popsize;

    const double T = (double)max_iters;
    const double t = (double)iter;

    for (int i = 0; i < popsize; ++i)
    {
        const double E0 = 2.0 * rand01() - 1.0;           // [-1,1]
        const double E  = 2.0 * E0 * (1.0 - t / T);       // escaping energy
        const double q  = rand01();
        const double r  = rand01();

        Data Xnew = hawks[i];

        if (absd(E) >= 1.0)
        {
            // Exploration phase
            const int rand_idx = (int)(rand01() * popsize) % popsize;
            const Data Xrand = hawks[rand_idx];

            if (q >= 0.5)
            {
                const double r1 = rand01();
                const double r2 = rand01();
                for (int j = 0; j < d; ++j)
                {
                    const double term = absd(Xrand[j] - 2.0 * r2 * hawks[i][j]);
                    Xnew[j] = Xrand[j] - r1 * term;
                }
            }
            else
            {
                const double r3 = rand01();
                const double r4 = rand01();
                for (int j = 0; j < d; ++j)
                {
                    const double randPos = L[j] + r4 * (R[j] - L[j]);
                    Xnew[j] = (rabbit[j] - Xm[j]) - r3 * randPos;
                }
            }

            enforceBounds(Xnew);
            const double fnew = myProblem->statFunmin(Xnew);

            if (fnew < fit[i])
            {
                hawks[i] = Xnew;
                fit[i] = fnew;
            }
            continue;
        }

        // Exploitation phase
        const double J = 2.0 * (1.0 - rand01()); // jump strength in (0,2)

        if (r >= 0.5 && absd(E) >= 0.5)
        {
            // Soft besiege
            for (int j = 0; j < d; ++j)
            {
                const double dX = rabbit[j] - hawks[i][j];
                Xnew[j] = dX - E * absd(J * rabbit[j] - hawks[i][j]);
            }
            enforceBounds(Xnew);
            const double fnew = myProblem->statFunmin(Xnew);
            if (fnew < fit[i]) { hawks[i] = Xnew; fit[i] = fnew; }
        }
        else if (r >= 0.5 && absd(E) < 0.5)
        {
            // Hard besiege
            for (int j = 0; j < d; ++j)
            {
                const double dX = rabbit[j] - hawks[i][j];
                Xnew[j] = rabbit[j] - E * absd(dX);
            }
            enforceBounds(Xnew);
            const double fnew = myProblem->statFunmin(Xnew);
            if (fnew < fit[i]) { hawks[i] = Xnew; fit[i] = fnew; }
        }
        else if (r < 0.5 && absd(E) >= 0.5)
        {
            // Soft besiege + rapid dives
            Data Y(d, 0.0);
            for (int j = 0; j < d; ++j)
                Y[j] = rabbit[j] - E * absd(J * rabbit[j] - hawks[i][j]);
            enforceBounds(Y);

            Data Lf = levyFlight(d, 1.5);
            Data Z(d, 0.0);
            for (int j = 0; j < d; ++j)
                Z[j] = Y[j] + rand01() * Lf[j] * (Y[j] - hawks[i][j]);
            enforceBounds(Z);

            const double fY = myProblem->statFunmin(Y);
            const double fZ = myProblem->statFunmin(Z);

            Data bestCand = hawks[i];
            double fbest = fit[i];
            if (fY < fbest) { bestCand = Y; fbest = fY; }
            if (fZ < fbest) { bestCand = Z; fbest = fZ; }

            hawks[i] = bestCand;
            fit[i] = fbest;
        }
        else
        {
            // Hard besiege + rapid dives
            Data Y(d, 0.0);
            for (int j = 0; j < d; ++j)
                Y[j] = rabbit[j] - E * absd(J * rabbit[j] - Xm[j]);
            enforceBounds(Y);

            Data Lf = levyFlight(d, 1.5);
            Data Z(d, 0.0);
            for (int j = 0; j < d; ++j)
                Z[j] = Y[j] + rand01() * Lf[j] * (Y[j] - hawks[i][j]);
            enforceBounds(Z);

            const double fY = myProblem->statFunmin(Y);
            const double fZ = myProblem->statFunmin(Z);

            Data bestCand = hawks[i];
            double fbest = fit[i];
            if (fY < fbest) { bestCand = Y; fbest = fY; }
            if (fZ < fbest) { bestCand = Z; fbest = fZ; }

            hawks[i] = bestCand;
            fit[i] = fbest;
        }
    }

    // update best
    besty_old = besty;
    const int b = bestIndex();
    if (fit[b] < besty)
    {
        besty = fit[b];
        bestx = hawks[b];
    }

    // tol window
    if (absd(besty - besty_old) <= tol) tol_hits++;
    else tol_hits = 0;

    // stagnation
    if (besty + tol < besty_old) stall = 0;
    else stall++;

    // Hybrid local optimization every R iters, on top-k, with probability p_local
    if (local_every_R > 0 && (iter % local_every_R == 0) && rand01() < p_local)
    {
        std::vector<int> idx(popsize);
        for (int i = 0; i < popsize; ++i) idx[i] = i;

        std::partial_sort(idx.begin(),
                          idx.begin() + std::min(top_k, popsize),
                          idx.end(),
                          [&](int a, int b){ return fit[a] < fit[b]; });

        const int kk = std::min(top_k, popsize);
        for (int k = 0; k < kk; ++k)
        {
            const int ii = idx[k];
            localOptimize(hawks[ii], fit[ii]);
            if (fit[ii] < besty) { besty = fit[ii]; bestx = hawks[ii]; }
        }
    }

    iter++;
}

bool UserMethod::terminated()
{
    if (iter >= max_iters) return true;
    if (max_evals > 0 && myProblem->getFunctionCalls() >= max_evals) return true;
    if (tol_hits >= tol_window) return true;
    if (stall >= max_stall) return true;
    return false;
}

void UserMethod::done()
{
    // apply local optimization to the best point
    localOptimize(bestx, besty);
}

void UserMethod::solve()
{
    init();
    while (!terminated())
        step();
    done();
}

Data UserMethod::getBestx() const { return bestx; }
double UserMethod::getBesty() const { return besty; }

UserMethod::~UserMethod() {}

