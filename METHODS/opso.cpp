
/*Konstantinos Barkas -- Parallel and Optimized Particle Swarm Optimization --- Dept. Informatics and Telecommunications UOI*/
#include "opso.h"
#include <omp.h>
#include <random>

/* * Constructor: Ορίζουμε τις παραμέτρους με τις οποίες θα λειτουργεί ο αλγόριθμος
 * του Global Optimus και αρχικοποιούμε το χρονόμετρο.
 */
OPSO::OPSO()
{
    before = std::chrono::system_clock::now(); // Έναρξη χρονομέτρησης

    // Λίστα διαθέσιμων μηχανισμών για τον περιορισμό ταχύτητας
    QStringList velocity_methods;
    velocity_methods << "standard_vmax" << "dynamic_vmax" << "ipso_vmax";

    // Εγγραφή παραμέτρων στο περιβάλλον του Global Optimus
    addParam(Parameter("opso_velocity_mode", velocity_methods[0], velocity_methods,
                            "Select velocity mechanism"));

    addParam(Parameter("opso_inertia_type", 0, 0, 14, "Inertia type (0-14)"));
    addParam(Parameter("opso_particles", 200, 10, 2000, "Number of particles"));
    addParam(Parameter("opso_maxiters",1000,10,2000,"Maximum number of iterations"));
    addParam(Parameter("opso_w", 2.95, -2.0, 5.0, "Inertia weight start (w)"));
    addParam(Parameter("opso_c1", 2.82, 0.0, 20.0, "Cognitive constant (c1)"));
    addParam(Parameter("opso_c2", 12.5, 0.0, 20.0, "Social constant (c2)"));

    // =========================================================================
    // ΝΗΣΙΑ & PROPAGATION -- παράμετροι
    // =========================================================================
    addParam(Parameter("prop", 0, 0, 1, "Enable propagation (0 or 1)"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Localsearch rate"));
    addParam(Parameter("subPopulation", "5", "The number of subelements/islands."));
    addParam(Parameter("propagationRate", "10", "Frequency of propagation (iterations)."));
    addParam(Parameter("propagationNumber", 0, 0, 50, "Number of positions to propagate."));

    // Επιλογές στρατηγικής μετανάστευσης πληθυσμού μεταξύ των νησιών
    QStringList propagationMethodList;
    propagationMethodList << "1to1" << "1toN" << "Nto1" << "NtoN";
    addParam(Parameter("propagationMethod", propagationMethodList[0],
                            propagationMethodList,
                            "Propagation strategy: 1to1, 1toN, Nto1, NtoN"));

    addParam(Parameter("subPopEnable", "1", "Number of subpopulations required to terminate."));
}

/*
 * init: Αρχικοποίηση πληθυσμού, μνήμης και παραμέτρων OpenMP πριν την έναρξη.
 */
void OPSO::init()
{
    termination = terminationMethod.trimmed(); // Λήψη κριτηρίου τερματισμού με καθαρισμό κενών από την trimmed
    iter = 0; // Μηδενισμός μετρητή επαναλήψεων

    // Ανάκτηση παραμέτρων από το περιβάλλον του Global Optimus
    max_iterations = getParam("opso_maxiters").getValue().toInt();
    velocity_mode = getParam("opso_velocity_mode").getValue();
    nParticles = getParam("opso_particles").getValue().toInt();
    w = getParam("opso_w").getValue().toDouble();
    c1 = getParam("opso_c1").getValue().toDouble();
    c2 = getParam("opso_c2").getValue().toDouble();

    prop = getParam("prop").getValue().toInt();
    subPopulation = getParam("subPopulation").getValue().toInt();
    propagationRate = getParam("propagationRate").getValue().toInt();
    propagationNumber = getParam("propagationNumber").getValue().toInt();
    propagationMethod = getParam("propagationMethod").getValue();
    localsearchRate = getParam("localsearchRate").getValue().toDouble();

    // Επεξεργασία συνθήκης τερματισμού για υποπληθυσμούς (μετατροπή string σε αριθμητική τιμή)
    subPopEnableStr = getParam("subPopEnable").getValue().trimmed();

    if (subPopEnableStr.compare("ANY", Qt::CaseInsensitive) == 0) {
        subPopEnable = 1;
    } else if (subPopEnableStr.compare("MAJORITY", Qt::CaseInsensitive) == 0) {
        subPopEnable = subPopulation / 2;
        if (subPopEnable <= 0) subPopEnable = 1;
    } else if (subPopEnableStr.compare("ALL", Qt::CaseInsensitive) == 0) {
        subPopEnable = subPopulation;
    } else {
        bool ok = false;
        int value = subPopEnableStr.toInt(&ok);
        if (ok) subPopEnable = value;
        else subPopEnable = 1;
    }

    methodLogger->printMessage(QString("Επιλέχθηκε ως μηχανισμός ταχύτητας: ") + velocity_mode);

    // Ρύθμιση OpenMP για παράλληλη επεξεργασία νησιών (threads = αριθμός νησίδων)
    threads = subPopulation;
    if (threads <= 0) threads = 1;

    omp_set_dynamic(0); // Απενεργοποίηση δυναμικής ρύθμισης νημάτων για σταθερότητα
    omp_set_num_threads(threads); // Καθορισμός αριθμού νημάτων ίσο με τα νησιά
    methodLogger->printMessage(QString("Ρυθμίστηκε για εκτέλεση με %1 νήματα").arg(threads));

    inertia_type = getParam("opso_inertia_type").getValue().toInt();
    methodLogger->printMessage(QString("Χρήση Inertia Type %1").arg(inertia_type));

    // Ανάκτηση διαστάσεων και ορίων (κάτω, πάνω όριο) πεδίου αναζήτησης από το πρόβλημα
    int dimension = myProblem->getDimension();
    lb = myProblem->getLeftMargin();
    ub = myProblem->getRightMargin();

    // Δέσμευση μνήμης για τους υποπληθυσμούς (μεγέθη για τις καλύτερες λύσεις)
    sub_size = nParticles / subPopulation;
    bestF2x.resize(subPopulation, 1e+100); // Αρχικοποίηση με άπειρο
    bestF2xOld.resize(subPopulation, 1e+100);
    bestSamply.resize(subPopulation, std::vector<double>(dimension, 0.0));

    // --- Αρχικοποίηση μηχανισμού στασιμότητας ---
     stagnation_counter.assign(subPopulation, 0);


    // Αρχικοποίηση αντικειμένων ελέγχου τερματισμού για κάθε νήμα/νησίδα
    threadDoublebox.resize(subPopulation);
    threadSimilarity.resize(subPopulation);
    for (int k = 0; k < subPopulation; k++) {
        threadDoublebox[k].init();
        threadSimilarity[k].init();

        threadSimilarity[k].setSimilarityIterations(25); // Ορισμός παραθύρου για το Similarity
    }

    // Προετοιμασία γεννητριών τυχαίων αριθμών για κάθε σωματίδιο (για αποφυγή ανταγωνισμού για πολυνηματικό προγραμματισμό)
    cell_gen.resize(nParticles);
    std::random_device rd;
    for (int i = 0; i < nParticles; ++i) {
        cell_gen[i].seed(rd());
    }
    besty = 1e30; // Αρχικοποίηση παγκόσμιου καλύτερου fitness σε μία πολύ μεγάλη τιμή
    bestx.resize(dimension); // Αρχικοποίηση διαστάσεων παγκόσμιου καλύτερου

    // Αρχικοποίηση πινάκων κατάστασης σμήνους (θέσεις, ταχύτητες, ατομικά καλύτερα)
    x.resize(nParticles, std::vector<double>(dimension));
    v.resize(nParticles, std::vector<double>(dimension));
    pbest.resize(nParticles, std::vector<double>(dimension));
    fitness.resize(nParticles, 1e30);
    pbest_fitness.resize(nParticles, 1e30);

    // Αρχική τυχαία δειγματοληψία του χώρου αναζήτησης
    sampleFromProblem(nParticles, x, fitness);

    // Αρχικοποίηση προσωπικών καλύτερων θέσεων (pbest) παράλληλα
    #pragma omp parallel for
    for (int i = 0; i < nParticles; ++i) {
        pbest[i] = x[i];
        pbest_fitness[i] = fitness[i];
        for (int d = 0; d < dimension; ++d) {
            v[i][d] = 0.0; // Αρχική μηδενική ταχύτητα για όλα τα σωματίδια
        }
    }

    // Εύρεση του αρχικού καλύτερου σωματιδίου σε κάθε νησίδα
    for (int k = 0; k < subPopulation; k++) {
        updateLeadersForSubpop(k); // Ενημέρωση τοπικού ηγέτη για νησίδα k
        if (bestF2x[k] < besty) {
            besty = bestF2x[k]; // Ενημέρωση παγκόσμιου καλύτερου αν βρεθεί βελτίωση
            bestx = bestSamply[k];
        }
    }

    // Αρχικοποίηση μεταβλητών για τον έλεγχο στασιμότητας
    prev_fitness_sum = 0.0;
    S_delta = 0;
}

void    OPSO::localMutate(int pos)
{
    int s = myProblem->getDimension();
    for(int i=0;i<s;i++)
    {
        double gold = x[pos][i];
        double delta = 0.05 * rand()*1.0/RAND_MAX*gold;
        double direction = rand() % 2==1?1.0:-1.0;
        double gnew = gold+direction * delta;
        x[pos][i]=gnew;
        if(!myProblem->isPointIn(x[pos]))
        {
            x[pos][i]=gold;
            continue;
        }
        double f = myProblem->funmin(x[pos]);
        if(f<fitness[pos])
        {
            fitness[pos]=f;
            printf("NEW BEST[%d]=%lf \n",pos,f);
        }
        else
        {
            x[pos][i]=gold;
        }
    }
}
void    OPSO::localCrossover(int thread,int pos)
{
    Data g;
    g.resize(myProblem->getDimension());
    for(int iters=1;iters<=sub_size/5;iters++)
    {
        int gpos=thread*sub_size+rand() % sub_size;
        int cutpoint=rand() % x[0].size();
        for(int j=0;j<(int)g.size();j++) g[j]=pbest[pos][j];
        double alpha = rand() *1.0/RAND_MAX;
        alpha = -0.5 + 2.0 * alpha;//[-0.5,1.5]
        g[cutpoint]=alpha * pbest[pos][cutpoint]+
                      (1.0-alpha)*pbest[gpos][cutpoint];
        if(!myProblem->isPointIn(g)) continue;
        double f=myProblem->statFunmin(g);

        if(fabs(f)<fabs(pbest_fitness[pos]))
        {
            pbest[pos][cutpoint]=g[cutpoint];
  //           printf("new FITNESS[%d]=%lf=>%lf \n",pos,fitness[pos],f);
            pbest_fitness[pos]=f;

        }
        else
        {
            g[cutpoint]=alpha * pbest[gpos][cutpoint]+
                          (1.0-alpha)*pbest[pos][cutpoint];
            if(!myProblem->isPointIn(g)) continue;
            double f=myProblem->statFunmin(g);
            if(fabs(f)<fabs(pbest_fitness[pos]))
            {

                pbest[pos][cutpoint]=g[cutpoint];
                pbest_fitness[pos]=f;
//                printf("new FITNESS[%d]=%lf=>%lf \n",pos,fitness[pos],f);

            }
        }
    }
}

/*
 * step: Κύριος βρόχος του αλγορίθμου.
 */
void OPSO::step()
{
    int dim = myProblem->getDimension();

    double proodos = (double)iter / (double)max_iterations;

    // Υπολογισμός γραμμικής μεταβολής βάρους αδράνειας (inertia weight) από το w_start = 2.95 έως w_end = -0.1 με βάση το paper για OPSO
    double w_start = w;
    double w_end   = -0.1;
    double current_w =0.5 + (rand()*1.0/RAND_MAX)/2.0;
        //w_start - (((w_start - w_end) * (double)iter) / (double)max_iterations); // εξίσωση 2 paper για OPSO

    // Έλεγχος στασιμότητας σμήνους για τον μηχανισμό ταχύτητας iPSO (υπολογίζεται σειριακά)
    if (velocity_mode == "ipso_vmax" && this->inertia_type == 14)
    {
        double current_fitness_sum = 0.0;
        for (int i = 0; i < nParticles; i++) {
            current_fitness_sum += std::fabs(fitness[i]); // Άθροισμα απόλυτων τιμών fitness
        }

        if (iter == 0) {
            prev_fitness_sum = current_fitness_sum; // Αρχικοποίηση προηγούμενου αθροίσματος
        }

        double delta_iter = std::fabs(current_fitness_sum - prev_fitness_sum);
        prev_fitness_sum = current_fitness_sum; // Ενημέρωση για την επόμενη επανάληψη
        // or delta_iter == 0
        if (iter > 0 && delta_iter == 0.0) {
            S_delta += 1; // Αύξηση μετρητή αν το σμήνος είναι στάσιμο
        }
    }

    // Αποθήκευση ιστορικού καλύτερων λύσεων ανά νησίδα
    for (int k = 0; k < subPopulation; k++) {
        bestF2xOld[k] = bestF2x[k];
    }

    // Παράλληλη εκτέλεση του βήματος κίνησης σωματιδίων ανά νησίδα
    #pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {


        int start = k * sub_size; // Καθορισμός αρχικού σωματιδίου για τη νησίδα k
        int end = std::min(start + sub_size, nParticles);

        if(iter%10==0)
        {
               //random cross items
            for(int i=0;i<sub_size/10;i++)
            {
                int rand_pos = start+rand() % sub_size;
                localCrossover(k,rand_pos);
            }


        }
        // Αξιολόγηση fitness και ενημέρωση προσωπικού καλύτερου / παγκόσμιου καλύτερου
        for (int i = start; i < end; i++)
        {
            double fit;
            #pragma omp critical(statfunmin_eval) // Προστασία κλήσης της αντικειμενικής συνάρτησης για τα νήματα
            {
                fit = myProblem->statFunmin(x[i]);
            }
            fitness[i] = fit;

            if (fit < pbest_fitness[i]) { // Ενημέρωση προσωπικού καλύτερου
                pbest_fitness[i] = fit;
                pbest[i] = x[i];
            }

            if (fit < besty) { // Ενημέρωση παγκόσμιου καλύτερου
                #pragma omp critical(gbest_update)
                {
                    if (fit < besty) {
                        besty = fit;
                        bestx.assign(x[i].begin(), x[i].end());
                    }
                }
            }
        }

        // Κίνήση σωματιδίων με βάση την κανονική κατανομή
        std::normal_distribution<double> normal_dist(0.0, 1.0);

        for (int i = start; i < end; ++i)
        {
            for (int j = 0; j < dim; ++j)
            {
                double r1, r2;
                // Παραγωγή τυχαίων τιμών με περιορισμό στο [-2, 2] -- περικομμένη κανονική κατανομή
                do { r1 = normal_dist(cell_gen[i]); } while (r1 < -2.0 || r1 > 2.0); // αν η τυχαία τιμή πέσει έξω από το -2 ή το 2 απορρίπτεται
                do { r2 = normal_dist(cell_gen[i]); } while (r2 < -2.0 || r2 > 2.0);

                r1 = std::fabs(r1); // Χρήση απόλυτης τιμής για τους συντελεστές
                r2 = std::fabs(r2);

                // Υπολογισμός νέας ταχύτητας (με βάση το paper του OPSO -- Εξίσωση 1 προσαρμοσμένη όμως για παραλληλη υλοποίηση)
                v[i][j] = current_w * v[i][j] +
                          c1 * r1 * (pbest[i][j] - x[i][j]) +
                          c2 * r2 * (bestSamply[k][j] - x[i][j]);

                // Επιλογή τύπου ταχύτητας ανάλογα με τον επιλεγμένο μηχανισμό  --- 3 τύποι ταχύτητας και επιλογή από το rundata.sh script
                if (velocity_mode == "standard_vmax") {
                    double vmax = 13.2;
                    if (v[i][j] >  vmax) v[i][j] =  vmax;
                    if (v[i][j] < -vmax) v[i][j] = -vmax;
                }
                else if (velocity_mode == "dynamic_vmax") { // Δυναμικό Vmax που μειώνεται με τις επαναλήψεις --
                    double range = ub[j] - lb[j];
                    double proodos = (double)iter / (double)max_iterations;
                    double dynamic_v_max = (0.2 * range) * (1.0 - 0.5 * proodos);
                    if (v[i][j] >  dynamic_v_max) v[i][j] =  dynamic_v_max;
                    if (v[i][j] < -dynamic_v_max) v[i][j] = -dynamic_v_max;
                }
                else if (velocity_mode == "ipso_vmax") { // Ο δυναμικά μεταβαλλόμενος τύπος ταχύτητας με βάση το paper ideal PSO
                    double ipso_w = calculateInertia(this->inertia_type, iter, max_iterations, cell_gen[k]);
                    const double c1_ipso = 2.0;    // 1.49618;
                    const double c2_ipso = 1.0;
                    v[i][j] = ipso_w * v[i][j] +
                              r1 * c1_ipso * (pbest[i][j] - x[i][j]) +
                              r2 * c2_ipso * (bestSamply[k][j] - x[i][j]);
                }

                x[i][j] += v[i][j]; // Ενημέρωση θέσης


                /*

                // ---  LÉVY FLIGHT ---
                    // Εφαρμόζουμε το άλμα σε ένα ποσοστό (π.χ. 10%) των σωματιδίων
                    // Χρησιμοποιούμε τη γεννήτρια i για να είναι ασφαλής με τα νήματα (κάθε thread έχει τη δική του)
                    std::uniform_real_distribution<double> dist_chance(0.0, 1.0);

                    // 1. Καθορισμός δυναμικού scaling
                    // Ξεκινάμε με ένα μικρό scale (0.0001)
                    double base_scaling = 0.0001;

                    // 2. Ενεργοποίηση με μικρότερο ποσοστό στο πρώτο μισό της εκπαίδευσης
                    if (proodos < 0.50) {

                        base_scaling = 0.005; // Μικρότερο και πιο ασφαλές 
                       double L = getLevyStep(1.5, i); // Κλήση της συνάρτησης με το index i
                       double range = ub[j] - lb[j];

                        x[i][j] += L * base_scaling * range; 
                    }                   

                    if (proodos < 0.85 && dist_chance(cell_gen[i]) < 0.10) {
                        double L = getLevyStep(1.5, i); // Κλήση της συνάρτησης με το index i
                        double range = ub[j] - lb[j];

                        // Το 0.01 είναι το scale factor. Αν το δεις πολύ νευρικό, μείωσέ το στο 0.005.
                        x[i][j] += L * base_scaling * range;
                    }

                    */

                    // --- Leavy flight και guassian θόρυβος που ακολουθεί την κανονική κατανομή ---
                    double progress = (double)iter / (double)max_iterations;

                    // 1. Υπολογισμός πιθανότητας (prob) και έντασης (intensity)
                    // Όσο προχωράμε, η πιθανότητα και η ένταση μειώνονται (Fine-tuning στο τέλος)
                    double prob = 0.10 * (1.0 - progress);
                    double intensity = 0.0001 * std::exp(-5.0 * progress);

                    std::uniform_real_distribution<double> dist_chance(0.0, 1.0);

                    if (dist_chance(cell_gen[i]) < prob) {
                        double range = ub[j] - lb[j];

                        // Στο πρώτο μισό χρησιμοποιούμε Lévy για μεγάλα άλματα (εξερεύνηση)
                        if (progress < 0.5) {
                            double L = getLevyStep(1.5, i);
                            x[i][j] += L * intensity * range;
                        }
                        // Στο δεύτερο μισό χρησιμοποιούμε Gaussian θόρυβο για μικρές διορθώσεις (εκμετάλλευση)
                        else {
                            std::normal_distribution<double> norm(0.0, 1.0);
                            x[i][j] += norm(cell_gen[i]) * intensity * range;
                        }

                        // επανεκκίνηση ταχύτητας για να "ξεκολλήσει" το σωματίδιο από την προηγούμενη πορεία
                        v[i][j] = 0.0;
                    }



                // Επιβολή ορίων χώρου για την αποφυγή εξόδου εκτός ορίων
                if (x[i][j] < lb[j]) {
                    x[i][j] = lb[j];
                    v[i][j] = 0.0;
                } else if (x[i][j] > ub[j]) {
                    x[i][j] = ub[j];
                    v[i][j] = 0.0;
                }
            }
        }
    }

    // Ενημέρωση ηγετών νησίδων μετά την κίνηση των σωματιδίων
    for (int k = 0; k < subPopulation; k++) {
        updateLeadersForSubpop(k);
    }

    // Διαχείριση μετανάστευσης (Propagation) εάν ενεργοποιηθεί
    if (prop && subPopulation > 1 && (iter % propagationRate == 0)) {
        propagate(); // Κλήση μηχανισμού μετανάστευσης
        for (int k = 0; k < subPopulation; k++) {
            updateLeadersForSubpop(k); // Επανυπολογισμός ηγετών μετά τη μετανάστευση
        }
    }

    // Εφαρμογή Τοπικής Αναζήτησης στον παγκόσμιο ηγέτη (bestx) βάσει στατιστικής πιθανότητας με τη βοήθεια της rand
    //  double r = (double)std::rand() / (double)RAND_MAX;
    // για νήματα δεν πρέπει να χρησιμοποιούμε rand

    // --- ΕΝΑΡΞΗ ΑΛΛΑΓΗΣ: Δυναμικό Local Search Rate ---
    // double progress = (double)iter / (double)max_iterations;

    // 1. Υπολογισμός δυναμικού rate (μειώνεται όσο πλησιάζουμε στο τέλος)
   //  double adaptive_rate = localsearchRate * (1.0 - progress);

    // 2. Ενίσχυση αν το σμήνος είναι στάσιμο (S_delta είναι ο μετρητής σου)
    // Αν το S_delta είναι υψηλό, σημαίνει ότι δεν έχουμε βελτίωση, οπότε δίνουμε ώθηση
    //if (S_delta > 10) {
   // adaptive_rate = std::max(0.1, adaptive_rate);
    //}

    // 3. Κατώφλι για να μην μηδενίζει ποτέ εντελώς η πιθανότητα (fine-tuning)
    //adaptive_rate = std::max(0.01, adaptive_rate);
    // r < adaptive_rate

    // r < localsearchRate


    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double r = dist(cell_gen[0]);
    if (r < localsearchRate) {
        #pragma omp critical(gbest_update)
        {
            double local_search_fit = localSearch(bestx); // Βελτιστοποίηση θέσης ηγέτη
            if (local_search_fit < besty) {
                besty = local_search_fit;
            }
        }
    }

    iter++; // Αύξηση μετρητή επαναλήψεων
}

// Εύρεση του καλύτερου σωματιδίου (τοπικού ηγέτη) μέσα σε συγκεκριμένη νησίδα k
void OPSO::updateLeadersForSubpop(int k)
{
    int start = k * sub_size;
    int end = std::min(start + sub_size, nParticles);
    if (end <= start) return;

    double min_fit = 1e+30;
    int best_idx = start;

    for (int i = start; i < end; ++i) { // επανάληψη στα σωματίδια της νησίδας
        if (fitness[i] < min_fit) {
            min_fit = fitness[i];
            best_idx = i;
        }
    }

        // χρησιμοποιούμε τη γεννήτρια νησίδας
        std::uniform_real_distribution<double> dist(0.0, 1.0);


          double prob = dist(cell_gen[k]);

          double progress = (double)iter / (double)max_iterations;

          /*
           double wave = 0.15 * std::sin(progress * 4.0 * M_PI);

           double current_prob = 0.80 * (1.0 - (progress*progress))+ wave; // Ξεκινάει με 100%, καταλήγει στο 0%

           // double current_prob = 0.40 * std::exp(-3.0*progress);

           if (min_fit < bestF2x[k] || prob < current_prob) {
            bestF2x[k] = min_fit;
            bestSamply[k] = x[best_idx];
            }

           */

               // 1. Υπολογισμός διαφοράς σφάλματος
              double delta = min_fit - bestF2x[k];

              // 2. Θερμοκρασία 
              double T = 1.0 - progress;
              if (T < 0.0001) T = 0.0001;

              //  Δυναμικό Scaling (Αυτόματη προσαρμογή)
              // Αντί για σταθερό 0.05, χρησιμοποιούμε το τρέχον καλύτερο fitness
              // για να κανονικοποιήσουμε το delta αυτόματα.
              static double avg_delta = 0.01; // Αρχική τιμή, θα προσαρμοστεί μόνο του
              if (delta > 0) avg_delta = 0.9 * avg_delta + 0.1 * delta; // μέσος


              double p = (delta < 0) ? 1.0 : std::exp(-delta / (T * avg_delta + 1e-9));              // 4. Απόφαση

              if (delta < 0 || prob < p) {
                  bestF2x[k] = min_fit;
                  bestSamply[k] = x[best_idx];

              }

}


// Εκτέλεση και δρομολόγηση της στρατηγικής μετανάστευσης (Propagete)
void OPSO::propagate()
{
    // Χρήση της γεννήτριας 0 για τις καθολικές αποφάσεις του αλγορίθμου
    std::uniform_int_distribution<int> dist(0, subPopulation - 1);

    // Υλοποίηση διαφόρων στρατηγικών μετανάστευσης
    if (propagationMethod == "1to1") {
        int sender = dist(cell_gen[0]);
        int receiver;
        do { receiver = dist(cell_gen[0]); } while (receiver == sender);
        propagateBetween(sender, receiver);
    }
    else if (propagationMethod == "1toN") {
        int sender = dist(cell_gen[0]);
        for (int receiver = 0; receiver < subPopulation; ++receiver) {
            if (receiver != sender) propagateBetween(sender, receiver);
        }
    }
    else if (propagationMethod == "Nto1") {
        int receiver = dist(cell_gen[0]);
        for (int sender = 0; sender < subPopulation; ++sender) {
            if (sender != receiver) propagateBetween(sender, receiver);
        }
    }
    else if (propagationMethod == "NtoN") {
        for (int sender = 0; sender < subPopulation; ++sender) {
            for (int receiver = 0; receiver < subPopulation; ++receiver) {
                if (sender != receiver) propagateBetween(sender, receiver);
            }
        }
    }
}

// Εκτέλεση ανταλλαγής λύσεων μεταξύ νησίδων
void OPSO::propagateBetween(int sender, int receiver)
{
    int senderStart = sender * sub_size;
    int senderEnd = std::min(senderStart + sub_size, nParticles);
    int receiverStart = receiver * sub_size;
    int receiverEnd = std::min(receiverStart + sub_size, nParticles);

    // Ταξινόμηση fitness (καλύτεροι σωματίδια sender, χειρότερα σωματίδια receiver)
    std::vector<std::pair<double, int>> senderFitnessIndex;
    for (int i = senderStart; i < senderEnd; ++i) {
        senderFitnessIndex.emplace_back(fitness[i], i);
    }
    std::sort(senderFitnessIndex.begin(), senderFitnessIndex.end());

    std::vector<std::pair<double, int>> receiverFitnessIndex;
    for (int i = receiverStart; i < receiverEnd; ++i) {
        receiverFitnessIndex.emplace_back(fitness[i], i);
    }
    std::sort(receiverFitnessIndex.begin(), receiverFitnessIndex.end(), std::greater<>());

    // Αντικατάσταση των χειρότερων του δέκτη με τα καλύτερα του αποστολέα
    for (int i = 0; i < propagationNumber; ++i) {
        if (i >= (int)senderFitnessIndex.size() || i >= (int)receiverFitnessIndex.size())
            break;

        int bestIndex = senderFitnessIndex[i].second;
        int worstIndex = receiverFitnessIndex[i].second;

        x[worstIndex] = x[bestIndex];
        v[worstIndex] = v[bestIndex];
        fitness[worstIndex] = fitness[bestIndex];
        pbest[worstIndex] = pbest[bestIndex];
        pbest_fitness[worstIndex] = pbest_fitness[bestIndex];
    }
}

// Υπολογισμός δυναμικής αδράνειας βάσει επιλεγμένου τύπου αδράνειας από το script (inertia_type)
double OPSO::calculateInertia(int type, int current_iter, int max_iter,std::mt19937& gen) {
    double g = (double)current_iter / (double)max_iter;
    double w = 0.7;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
     double R = dist(gen); // Χρησιμοποιούμε τη γεννήτρια

    switch (type) {
        case 0:  w = 0.9 - g * (0.9 - 0.4); break;
        case 1:  w = 0.4 + 0.5 * std::exp(-10.0 * g); break;
        case 2:  w = 0.9 - (0.5 * g); break;
        case 3:  w = 0.7; break;
        case 4:  w = 0.5 + 0.4 * std::cos(M_PI * g); break;
        case 5:  w = 0.4 + 0.5 / (1.0 + std::exp(10.0 * (g - 0.5))); break;
        case 6:  w = 0.4 + 0.5 * R; break;
        case 7:  w = 0.9 * std::pow(0.5, g); break;
        case 8:  w = 0.9 - 0.5 * std::sin(g * 3.14159); break;
        case 9:  w = 0.4 + 0.5 * (R > 0.5 ? 1.0 : 0.0); break;
        case 10: w = 0.4 + 0.5 * std::fabs(std::sin(20.0 * g)); break;
        case 11: w = 0.9 - g * (0.7) - 0.1 * std::sin(10.0 * g); break;
        case 12: w = 0.1 + 0.5 * (R + 0.5) * std::exp(-2.0 * g); break;
        case 13: w = 0.9 - 0.5 * std::pow(g, 2); break;
        case 14: { // Προσαρμοστική αδράνεια βάσει στασιμότητας με βάση το paper Ideal PSO
            double w_max = 0.9;
            double w_min = 0.4;
            double C_delta = 1.0;
            if (current_iter > 0) {
                C_delta = (double)S_delta / (double)current_iter;
            }
            if (C_delta == 0.0) C_delta = 1.0;
            w = w_max - ((double)current_iter / C_delta) * (w_max - w_min);
            if (w > w_max) w = w_max;
            if (w < w_min) w = w_min;
            break;
        }
        default: w = 0.7; break;
    }
    return w;
}

// Βοηθητική συνάρτηση για τον υπολογισμό του Lévy step
double OPSO::getLevyStep(double beta, int i) {
    std::normal_distribution<double> norm(0.0, 1.0);
    double u = norm(cell_gen[i]); // Χρήση του i για αποφυγή σύγκρουσης
    double v = norm(cell_gen[i]);

    double numerator = std::tgamma(1.0 + beta) * std::sin(M_PI * beta / 2.0);
    double denominator = std::tgamma((1.0 + beta) / 2.0) * beta * std::pow(2.0, (beta - 1.0) / 2.0);
    double sigma_u = std::pow(numerator / denominator, 1.0 / beta);

    double step = u / std::pow(std::fabs(v), 1.0 / beta);
    return sigma_u * step;
}

// Setters και Getters
void OPSO::setFitness(int index, double f) { if(index >= 0 && index < nParticles) fitness[index] = f; }
double OPSO::getFitness(int index) const { return fitness[index]; }
std::vector<double> OPSO::getGenome(int index) const { return x[index]; }
double OPSO::getBestFitness() const { return besty; }
std::vector<double> OPSO::getBestPosition() const { return bestx; }

// Έλεγχος συνθηκών τερματισμού για όλο το σμήνος
bool OPSO::terminated()
{
    int finished_subpops = 0;
    for (int i = 0; i < subPopulation; i++)
    {
        if (checkSubCluster(i)) // Έλεγχος αν η νησίδα έχει τερματίσει
            finished_subpops++;
    }

    if (iter >= max_iterations) return true; // Τερματισμός αν φτάσαμε στις μέστιες επαναλήψεις

    return finished_subpops >= subPopEnable; // Τερματισμός αν ικανοποιείται το όριο νησίδων
}

// Έλεγχος σύγκλισης για μεμονωμένη νησίδα βάσει της επιλεγμένης μεθόδου τερματισμού
bool OPSO::checkSubCluster(int subCluster)
{
    const int miniters = 50; // Ελάχιστες επαναλήψεις πριν τον έλεγχο τερματισμού
    if (iter < miniters) return false;

    double diff = std::fabs(bestF2xOld.at(subCluster) - bestF2x.at(subCluster)); // Διαφορά καλύτερου fitness



     //   printf("DEBUG: Sub=%d, Iter=%d, Best=%e, OldBest=%e, Diff=%e\n",
    //                   subCluster, iter, bestF2x.at(subCluster), bestF2xOld.at(subCluster), diff);


    // Τερματισμός μέσω similarity
    if (termination.compare("Similarity", Qt::CaseInsensitive) == 0 && threadSimilarity[subCluster].terminate(diff))
        {
            methodLogger->printMessage(QString(">>> Νησί %1 τερματίσε με SIMILARITY στο iter=%2 με diff=%3")
                                       .arg(subCluster).arg(iter).arg(diff));
            return true;
        }

    // Τερματισμός μέσω DoubleBox
    if (termination.compare("DoubleBox", Qt::CaseInsensitive) == 0 && threadDoublebox[subCluster].terminate(diff))
    {
        methodLogger->printMessage(QString(">>> Νησί %1 τερματίσε με DOUBLEBOX στο iter=%2 με diff=%3")
                                   .arg(subCluster).arg(iter).arg(diff));
        return true;
    }

    if (iter >= max_iterations) return true; // Τερματισμός αν φτάσαμε στις μέγιστες επαναλήψεις

    return false;
}

// Ενέργειες που εκτελούνται μετά τον τερματισμό του αλγορίθμου
void OPSO::done()
{
    besty = localSearch(bestx); // Τελική βελτίωση της καλύτερης λύσης με τοπική αναζήτηση

    after = std::chrono::system_clock::now(); // Λήξη χρονομέτρησης
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    methodLogger->printMessage(QString("Συνολικός Χρόνος Εκτέλεσης: %1 sec").arg((double)milliseconds.count() / 1000.0));
}

// Εκτύπωση κατάστασης στην οθόνη, επανάληψη και καλύτερη τιμή
void OPSO::showDebug()
{
    methodLogger->printMessage(
        QString::asprintf("Iter=%4d BEST VALUE=%10.4lf", iter, besty));
}

// Destructor: Καθαρισμός μνήμης όλων των διανυσμάτων
OPSO::~OPSO()
{
    x.clear();
    v.clear();
    pbest.clear();
    fitness.clear();
    pbest_fitness.clear();

    bestF2x.clear();
    bestF2xOld.clear();
    bestSamply.clear();
    threadSimilarity.clear();
    threadDoublebox.clear();
}
