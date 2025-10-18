#include <METHODS/trio.h>
using std::vector;

TRIO::TRIO()
{
    before = std::chrono::system_clock::now();

    addParam(Parameter("population", 100, 10, 2000000, "Population size"));
    addParam(Parameter("maxiters", 500, 10, 10000, "Maximum iters"));
    addParam(Parameter("trials_per_agent", 4, 1, 12, "Trials per individual (keep best)"));
    addParam(Parameter("agent_fraction", 0.55, 0.05, 1.0, "Fraction of pop updated per step"));

    // jDE self-adaptation
    addParam(Parameter("tauF", 0.10, 0.0, 1.0, "Prob to resample F_i"));
    addParam(Parameter("tauCR", 0.10, 0.0, 1.0, "Prob to resample CR_i"));
    addParam(Parameter("F_lo", 0.1, 0.0, 2.0, "Min F"));
    addParam(Parameter("F_hi", 1.2, 0.0, 3.0, "Max F"));
    addParam(Parameter("CR_lo", 0.0, 0.0, 1.0, "Min CR"));
    addParam(Parameter("CR_hi", 0.95, 0.0, 1.0, "Max CR"));

    addParam(Parameter("pbest_frac", 0.10, 0.01, 0.9, "p-best fraction"));

    // Stagnation / restart
    addParam(Parameter("stagnation_trigger", 18, 1, 10000, "No-improvement steps → restart"));
    addParam(Parameter("restart_frac", 0.10, 0.0, 1.0, "Worst fraction to reset"));
    addParam(Parameter("restart_sigma", 0.20, 0.0, 5.0, "σ of reset around best (×box)"));

    // LANDScope
    addParam(Parameter("trio_landscape", "off", "Landscape probe (off|probe)"));
    addParam(Parameter("trio_probe_frac", 0.004, 0.0, 0.5, "Probe budget fraction of max FC"));
    addParam(Parameter("trio_probe_steps", 32, 4, 200, "Random-walk steps for ruggedness"));
    addParam(Parameter("trio_probe_kdims", 0, 0, 1000000, "Axis dims for separability (0→min(20,dim))"));
}

void TRIO::init()
{
    // Params
    population         = getParam("population").getValue().toInt();
    doubleBox.setMinIters(100);
    maxIterations = getParam("maxiters").getValue().toInt();
    trials_per_agent   = getParam("trials_per_agent").getValue().toInt();
    agent_fraction     = getParam("agent_fraction").getValue().toDouble();

    tauF   = getParam("tauF").getValue().toDouble();
    tauCR  = getParam("tauCR").getValue().toDouble();
    F_lo   = getParam("F_lo").getValue().toDouble();
    F_hi   = getParam("F_hi").getValue().toDouble();
    CR_lo  = getParam("CR_lo").getValue().toDouble();
    CR_hi  = getParam("CR_hi").getValue().toDouble();

    pbest_frac         = getParam("pbest_frac").getValue().toDouble();

    stagnation_trigger = getParam("stagnation_trigger").getValue().toInt();
    restart_frac       = getParam("restart_frac").getValue().toDouble();
    restart_sigma      = getParam("restart_sigma").getValue().toDouble();

    landscape_mode     = getParam("trio_landscape").getValue();
    probe_frac         = getParam("trio_probe_frac").getValue().toDouble();
    probe_steps        = getParam("trio_probe_steps").getValue().toInt();
    probe_kdims        = getParam("trio_probe_kdims").getValue().toInt();

    // Problem
    dimension = myProblem->getDimension();
    lb_ = myProblem->getLeftMargin();
    ub_ = myProblem->getRightMargin();
    if ((int)lb_.size()<dimension || (int)ub_.size()<dimension){
        lb_.assign(dimension,-1.0);
        ub_.assign(dimension, 1.0);
    }
    if (dimension<=0){
        return;
    }

    // RNG
    std::random_device rd; rng.seed(rd());
    DIpop = std::uniform_int_distribution<int>(0, std::max(0,population-1));
    DIdim = std::uniform_int_distribution<int>(0, std::max(0,dimension-1));

    // Initial population
    sampleFromProblem(population, samples, fitnessArray);

    // Elite
    int elite=0; for (int i=1;i<population;++i) if (fitnessArray[i] < fitnessArray[elite]) elite=i;
    bestSample = samples[elite]; bestF2x = fitnessArray[elite];

    // Per-individual F_i, CR_i
    Fi.assign(population, 0.9);
    CRi.assign(population, 0.7);

    iteration=0; stagn_iters=0; startAgent=0;

    // Required tail
   // similarity.setSimilarityIterations(12);
   // myProblem->setMaxFunctionCalls(150000);

    // -------- LANDScope probe (optional) --------
    if (landscape_mode == "probe") {
        probeLandscape_();
        applyLandscapeProfile_();
    }
}

bool TRIO::terminated(){

    QString term = terminationMethod;
    if (iteration >= maxIterations) return true;
    if (term == "maxiters") return iteration >= maxIterations;
    else if (term == "doublebox") return doubleBox.terminate(bestF2x);
    else if (term == "similarity") return similarity.terminate(bestF2x);

   // if (myProblem->getFunctionCalls() >= myProblem->getMaxFunctionCalls()) return true;
    return false;
}

void TRIO::done(){
    //bestF2x = localSearch(bestSample);
    after = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
    std::cout << "TRIO finished. Duration: " << (double)ms/1000.0 << " sec\n";
}

void TRIO::showDebug(){
    if (iteration%50 == 0 && getParam("opt_debug").getValue()=="yes"){
        std::cout << "TRIO-EDGE iter="<<iteration
                  << " fc="<<myProblem->getFunctionCalls()
                  //<< " LS[rug="<<lstats.rugged<<", sep="<<lstats.separ<<", sc="<<lstats.scale<<"]"
                  << " best="<<std::setprecision(16)<<bestF2x
                  << std::endl;
    }
}

int TRIO::eliteIndex_() const{
    int idx=0; double bf=fitnessArray[0];
    for (int i=1;i<population;++i) if (fitnessArray[i] < bf){ bf=fitnessArray[i]; idx=i; }
    return idx;
}

// -------------------- LANDScope ------------------------

void TRIO::probeLandscape_()
{

    int maxFC = 150000;//(int)myProblem->getMaxFunctionCalls();
    int budget = std::max(8, (int)std::floor(probe_frac * std::max(1,maxFC)));
    budget = std::min(budget, 400); // upper safety


    vector<double> x = bestSample.empty() ? vector<double>(dimension,0.0) : bestSample;
    if (bestSample.empty()){
        for (int j=0;j<dimension;++j) x[j] = 0.5*(lb_[j]+ub_[j]);
    }

    auto evalClamp = [&](const vector<double>& v)->double{
        vector<double> y=v;
        for (int j=0;j<dimension;++j) y[j] = clamp(y[j], lb_[j], ub_[j]);
        return myProblem->statFunmin(y);
    };


    int steps = std::max(4, std::min(probe_steps, budget/2));
    double hrel = 0.02;
    vector<double> fseq; fseq.reserve(steps+1);
    vector<double> y = x;
    fseq.push_back(evalClamp(y));
    for (int t=0;t<steps;++t){
        for (int j=0;j<dimension;++j){
            double step = hrel * range(j) * N0(rng);
            y[j] = clamp(y[j] + step, lb_[j], ub_[j]);
        }
        fseq.push_back(evalClamp(y));
    }
    // diffs
    vector<double> d; d.reserve(steps);
    for (int t=1;t<(int)fseq.size();++t) d.push_back(fseq[t]-fseq[t-1]);
    double mean=0; for (double z:d) mean+=z; if(!d.empty()) mean/=d.size();
    double num=0, den=0;
    for (int t=1;t<(int)d.size();++t){
        num += (d[t-1]-mean)*(d[t]-mean);
    }
    for (double z:d){ den += (z-mean)*(z-mean); }
    double rho = (den>1e-30)? (num/den) : 0.0; // lag-1 correlation
    lstats.rugged = std::max(0.0, std::min(1.0, 0.5*(1.0 - rho))); // low rho -> high rugged -> closer to 1

    // scale proxy
    double mu=0; for (double z:fseq) mu+=z; mu/=std::max(1,(int)fseq.size());
    double var=0; for (double z:fseq){ double u=z-mu; var+=u*u; }
    var/=std::max(1,(int)fseq.size());
    lstats.scale = std::sqrt(std::max(1e-30, var));

    // --- Separability via axis perturbations vs. directional ---
    int kd = probe_kdims>0 ? std::min(probe_kdims, dimension) : std::min(20, dimension);
    vector<int> dims(dimension); std::iota(dims.begin(), dims.end(), 0);
    std::shuffle(dims.begin(), dims.end(), rng);
    dims.resize(kd);

    double hax = 0.02;
    double axis_sum = 0.0;
    for (int j : dims){
        vector<double> xp = x, xm = x;
        double h = hax * range(j);
        xp[j] = clamp(xp[j] + h, lb_[j], ub_[j]);
        xm[j] = clamp(xm[j] - h, lb_[j], ub_[j]);
        double fp = evalClamp(xp);
        double fm = evalClamp(xm);
        axis_sum += std::fabs(fp - fm);
    }
    // directional
    vector<double> dir(dimension,0.0);
    double ns=0; for (int j=0;j<dimension;++j){ double z=N0(rng); dir[j]=z; ns+=z*z; }
    ns=std::sqrt(std::max(1e-30,ns)); for (double &z:dir) z/=ns;
    vector<double> xp = x, xm = x;
    for (int j=0;j<dimension;++j){
        double h = hax * range(j) * dir[j];
        xp[j] = clamp(xp[j] + h, lb_[j], ub_[j]);
        xm[j] = clamp(xm[j] - h, lb_[j], ub_[j]);
    }
    double fpd = evalClamp(xp), fmd = evalClamp(xm);
    double dir_mag = std::fabs(fpd - fmd);
    double denom = axis_sum + dir_mag + 1e-30;
    lstats.separ = std::max(0.0, std::min(1.0, axis_sum / denom)); // μεγάλος λόγος => πιο separable


}

void TRIO::applyLandscapeProfile_()
{

    // thresholds:
    bool rugged   = (lstats.rugged >= 0.6);            // πολύ τραχύ
    bool separ    = (lstats.separ  >= 0.65);           // σχετικά separable
    bool smooth   = (lstats.scale  <= 0.05*lstats.scale + 1e-9); // no-op guard; κρατάμε απλά var info

    if (rugged){

        trials_per_agent = std::max(5, trials_per_agent+1);
        F_lo = std::max(0.15, F_lo); F_hi = std::min(1.6, F_hi + 0.2);
        CR_hi = std::min(0.9, CR_hi);
        pbest_frac = std::max(0.06, pbest_frac*0.8);
        restart_sigma = std::min(0.28, restart_sigma*1.15);
    } else if (separ){

        F_hi = std::max(1.0, F_hi); F_lo = std::max(0.08, F_lo*0.9);
        CR_lo = std::max(0.0, CR_lo); CR_hi = std::min(0.98, std::max(0.9, CR_hi));
        trials_per_agent = std::max(3, trials_per_agent-1);
        pbest_frac = std::min(0.18, std::max(0.10, pbest_frac*1.2));
    } else {
        // Balanced
        F_hi = std::min(1.3, F_hi);
        CR_hi = std::min(0.95, CR_hi);
    }
}

// -------------------- DE variants ----------------------

void TRIO::make_trial_best1(int i, const vector<double>& xi, vector<double>& trial, double F, double CR)
{
    if (population < 3){ trial = xi; return; }
    int r1, r2;
    do { r1 = DIpop(rng); } while (r1==i);
    do { r2 = DIpop(rng); } while (r2==i || r2==r1);

    trial = xi;
    int jrand = (dimension>0) ? DIdim(rng) : 0;
    for (int j=0;j<dimension;++j){
        if (U(rng) < CR || j==jrand){
            double v = bestSample[j] + F*(samples[r1][j] - samples[r2][j]);
            trial[j] = clamp(v, lb_[j], ub_[j]);
        }
    }
}

void TRIO::make_trial_ctobest1(int i, const vector<double>& xi, vector<double>& trial, double F, double CR)
{
    if (population < 3){ trial = xi; return; }
    int r1, r2;
    do { r1 = DIpop(rng); } while (r1==i);
    do { r2 = DIpop(rng); } while (r2==i || r2==r1);

    trial = xi;
    int jrand = (dimension>0) ? DIdim(rng) : 0;
    for (int j=0;j<dimension;++j){
        if (U(rng) < CR || j==jrand){
            double base = xi[j] + F*(bestSample[j] - xi[j]) + F*(samples[r1][j] - samples[r2][j]);
            trial[j] = clamp(base, lb_[j], ub_[j]);
        }
    }
}

void TRIO::make_trial_pbest1(int i, const vector<double>& xi, vector<double>& trial, double F, double CR)
{
    if (population < 3){ trial = xi; return; }
    int pnum = std::max(1, (int)std::round(pbest_frac * population));
    vector<int> idx(population); std::iota(idx.begin(), idx.end(), 0);
    std::partial_sort(idx.begin(), idx.begin()+pnum, idx.end(),
                      [&](int a,int b){ return fitnessArray[a] < fitnessArray[b]; });
    int pbest = idx[ std::uniform_int_distribution<int>(0, pnum-1)(rng) ];

    int r1, r2;
    do { r1 = DIpop(rng); } while (r1==i || r1==pbest);
    do { r2 = DIpop(rng); } while (r2==i || r2==r1 || r2==pbest);

    trial = xi;
    int jrand = (dimension>0) ? DIdim(rng) : 0;
    for (int j=0;j<dimension;++j){
        if (U(rng) < CR || j==jrand){
            double v = xi[j] + F*(samples[pbest][j] - xi[j]) + F*(samples[r1][j] - samples[r2][j]);
            trial[j] = clamp(v, lb_[j], ub_[j]);
        }
    }
}

// trial−base (λ∈{1,0.5})
bool TRIO::line_refine(const vector<double>& base, const vector<double>& trial_in, vector<double>& trial_out)
{
    vector<double> y = base;
    for (int j=0;j<dimension;++j){
        double v = base[j] + 1.0*(trial_in[j] - base[j]);
        y[j] = clamp(v, lb_[j], ub_[j]);
    }
    double f1 = myProblem->statFunmin(y);

    vector<double> z = base;
    for (int j=0;j<dimension;++j){
        double v = base[j] + 0.5*(trial_in[j] - base[j]);
        z[j] = clamp(v, lb_[j], ub_[j]);
    }
    double f2 = myProblem->statFunmin(z);

    if (f1 <= f2){ trial_out = y; return true; }
    else         { trial_out = z; return true; }
}

// -------------------- Restarts -------------------------

void TRIO::microRestart_()
{
    int nreset = std::max(1, (int)std::round(restart_frac * population));
    vector<int> idx(population); std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](int a,int b){ return fitnessArray[a] < fitnessArray[b]; });

    const int elite = idx[0];
    for (int k=0; k<nreset; ++k){
        int i = idx[ population-1 - k ];
        if (i == elite) continue;
        vector<double> cand = samples[i];
        for (int j=0;j<dimension;++j){
            double step = restart_sigma * range(j) * N0(rng);
            cand[j] = clamp(bestSample[j] + step, lb_[j], ub_[j]);
        }
        double f = myProblem->statFunmin(cand);
        if (f < fitnessArray[i]){
            samples[i]=cand; fitnessArray[i]=f;
            if (f < bestF2x){ bestF2x=f; bestSample=cand; }
        }
    }
}

// -------------------- Main loop ------------------------

void TRIO::step()
{
    iteration++;

    // Elite refresh
    int elite = eliteIndex_();
    if (fitnessArray[elite] < bestF2x){ bestF2x = fitnessArray[elite]; bestSample = samples[elite]; }
    double prevBest = bestF2x;

    // mini-batch
    int batch = std::max(1, (int)std::floor(agent_fraction * population));
    int s = startAgent;
    int e = std::min(population, s + batch);

    for (int i=s; i<e; ++i){
        if (i==elite) continue;

        // jDE self-adaptation
        if (U(rng) < tauF)  Fi[i]  = F_lo + (F_hi - F_lo)*U(rng);
        if (U(rng) < tauCR) CRi[i] = CR_lo + (CR_hi - CR_lo)*U(rng);

        vector<double> x = samples[i];
        double fx = fitnessArray[i];

        // Ensemble trials — keep best
        vector<double> bestT = x; double bestTf = std::numeric_limits<double>::infinity();

        for (int t=0; t<trials_per_agent; ++t){
            vector<double> T;
            int v = (iteration + i + t) % 3;
            if (v==0)      make_trial_best1   (i, x, T, Fi[i], CRi[i]);
            else if (v==1) make_trial_ctobest1(i, x, T, Fi[i], CRi[i]);
            else           make_trial_pbest1  (i, x, T, Fi[i], CRi[i]);

            double fT = myProblem->statFunmin(T);
            if (fT < bestTf){ bestTf=fT; bestT=T; }
        }


        vector<double> Tref;
        (void)line_refine(x, bestT, Tref);
        double fRef = myProblem->statFunmin(Tref);
        if (fRef < bestTf){ bestTf = fRef; bestT = Tref; }

        // Greedy
        if (bestTf < fx){
            samples[i] = bestT; fitnessArray[i] = bestTf;
            if (bestTf < bestF2x){ bestF2x = bestTf; bestSample = bestT; }
        }else{
            // diversify: χειρότερος αντικαθίσταται σποραδικά
            int worst = 0; double wf = fitnessArray[0];
            for (int k=1;k<population;++k) if (fitnessArray[k] > wf){ wf = fitnessArray[k]; worst = k; }
            if (bestTf < wf && U(rng) < 0.20){
                samples[worst] = bestT; fitnessArray[worst] = bestTf;
                if (bestTf < bestF2x){ bestF2x = bestTf; bestSample = bestT; }
            }
        }
    }

    startAgent = e;
    if (startAgent >= population) startAgent = 0;

    // Stagnation → micro-restart
    if (bestF2x + 1e-18 < prevBest) stagn_iters = 0; else stagn_iters++;
    if (stagn_iters >= stagnation_trigger){
        microRestart_();
        stagn_iters = 0;
    }
}



TRIO::~TRIO() {}
