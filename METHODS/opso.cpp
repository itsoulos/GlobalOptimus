/*Konstantinos Barkas
Informatics & Telecommunications - UOI
*/

#include "opso.h"
#include <omp.h>

/* * Constructor: Ορίζουμε τις παραμέτρους που θα εμφανίζονται στο GUI
 * του Global Optimus.
 */
OPSO::OPSO()
{

    QStringList velocity_methods;
        // 1. Standard (όπως στο paper 13.2)
        // 2. Dynamic Vmax (20% του εύρους)
        // 3. ipso_vmax (ipso_paper)
        velocity_methods << "standard_vmax" << "dynamic_vmax" << "ipso_vmax";

    // Προσθήκη παραμέτρου για την επιλογή μηχανισμού ταχύτητας
    addParam(Parameter("opso_velocity_mode", velocity_methods[0], velocity_methods,
                               "Select velocity mechanism"));

    // Προσθήκη παραμέτρου για τον αριθμό των νημάτων
    addParam(Parameter("opso_threads", 5, 1, 10, "Number of OpenMP threads"));
    // Προσθήκη επιλογής τύπου αδράνειας (default τιμή 0)
        addParam(Parameter("opso_inertia_type", 0, 0, 13, "Inertia type (0-13)"));

    // Προσθήκη παραμέτρου για το πλήθος των σωματιδίων με εύρος 10 έως 2000
    addParam(Parameter("opso_particles", 200, 10, 2000, "Number of particles"));
    // Προσθήκη παραμέτρου για το πλήθος των επαναλήψεων
    addParam(Parameter("opso_maxiters",1000,10,2000,"Maximum number of iterations"));
    // Προσθήκη παραμέτρων w,c1, c2 με βάση το Paper (Πίνακας 8)
    addParam(Parameter("opso_w", 2.95, -2.0, 5.0, "Inertia weight start (w)"));
    addParam(Parameter("opso_c1", 2.82, 0.0, 20.0, "Cognitive constant (c1)"));
    addParam(Parameter("opso_c2", 12.5, 0.0, 20.0, "Social constant (c2)"));
}


/*
 * init: Αρχικοποίηση πληθυσμού και μνήμης.
 * Τραβάει τις τιμές από το σύστημα παραμέτρων και χρησιμοποιεί τα όρια του προβλήματος.
 */
void OPSO::init()
{



     // Μηδενισμός του μετρητή επαναλήψεων για να ξεκινάει σωστά κάθε τρέξιμο
    iter = 0;



         //  Αρχικοποίηση του αντικειμένου
         similarity.init();

        // (Αντί για 5 που υπάρχει ως default)
        similarity.setSimilarityIterations(100);


     max_iterations = getParam("opso_maxiters").getValue().toInt();
    // printf("DEBUG: Max Iterations is %d\n", max_iterations);

    // Ανάκτηση της επιλογής του χρήστη για το μηχανησμό ταχύτητας
     velocity_mode = getParam("opso_velocity_mode").getValue();
     // ΠΡΟΣΘΗΚΗ: Εκτύπωση για έλεγχο
           methodLogger->printMessage(QString("Επιλέχθηκε ως μηχανισμός ταχύτητας: ") + velocity_mode);

    // Ανάκτηση του αριθμού των νημάτων
    threads = getParam("opso_threads").getValue().toInt();

        // 2. Επιβολή στο OpenMP
        omp_set_dynamic(0); // Απαγορεύει στο σύστημα να αγνοήσει την εντολή μας
        omp_set_num_threads(threads);

        // 3. Επιβεβαίωση στο log
        methodLogger->printMessage(QString(" Ρυθμίστηκε για εκτέλεση με %1 νήματα").arg(threads));

    // Ανάκτηση του inertia type από το GUI
   int  inertia_type = getParam("opso_inertia_type").getValue().toInt();

    // Εκτύπωση του τύπου inertia
        methodLogger->printMessage(QString("Χρήση Inertia Type %1").arg(inertia_type));
    //  Ανάκτηση της επιλεγμένης τιμής για τον αριθμό των σωματιδίων
    nParticles = getParam("opso_particles").getValue().toInt();

    // Ανάκτηση της τιμής της αδράνειας w
    w = getParam("opso_w").getValue().toDouble();
    // Ανάκτηση της σταθεράς c1
    c1 = getParam("opso_c1").getValue().toDouble();
    // Ανάκτηση της κοινωνικής σταθεράς c2
    c2 = getParam("opso_c2").getValue().toDouble();


    //2. Παίρνουμε την διάσταση του προβλήματος
    int dimension = myProblem->getDimension();


    // Τραβάμε το αριστερό και δεξί όριο για να γίνει αρχικοποίηση
    lb = myProblem->getLeftMargin();
    ub = myProblem->getRightMargin();

    // Αρχικοποίηση της γεννήτριας τυχαίων αριθμών με hardware seed
    // gen.seed(std::random_device()());
    // Αρχικοποίηση ανεξάρτητων γεννητριών για κάθε σωματίδιο (για ασφάλεια με τα νήματα)
        cell_gen.resize(nParticles);
        std::random_device rd;
        for (int i = 0; i < nParticles; ++i)
        {
            cell_gen[i].seed(rd());
        }
    // Ορισμός αρχικού παγκόσμιου βέλτιστου fitness σε μια πολύ μεγάλη τιμή
    besty = 1e30;
    // Προσαρμογή μεγέθους του vector της καλύτερης θέσης στις διαστάσεις του προβλήματος

    bestx.resize(dimension);

    // Δέσμευση μνήμης για τις τρέχουσες θέσεις όλων των σωματιδίων
    x.resize(nParticles, std::vector<double>(dimension));
    // Δέσμευση μνήμης για τις ταχύτητες όλων των σωματιδίων
    v.resize(nParticles, std::vector<double>(dimension));
    // Δέσμευση μνήμης για τις προσωπικές καλύτερες θέσεις (pbest)
    pbest.resize(nParticles, std::vector<double>(dimension));
    // Δέσμευση μνήμης για τις τρέχουσες τιμές fitness
    fitness.resize(nParticles, 1e30);
    // Δέσμευση μνήμης για τις καλύτερες τιμές fitness κάθε σωματιδίου
    pbest_fitness.resize(nParticles, 1e30);

    // Ορισμός ομοιόμορφης κατανομής για τυχαίους αριθμούς στο [0, 1]
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Βρόχος αρχικοποίησης για κάθε σωματίδιο του σμήνους
    for (int i = 0; i < nParticles; ++i)
    {
        // Βρόχος για κάθε διάσταση του προβλήματος
        for (int d = 0; d < dimension; ++d)
        {
            // Τοποθέτηση σωματιδίου σε τυχαία θέση εντός των ορίων [lbound, ubound]
            x[i][d] = lb[d] + dist(cell_gen[i]) * (ub[d] - lb[d]);
            // Αρχικοποίηση ταχύτητας στο μηδέν (όπως ορίζει το paper)
            v[i][d] = 0.0;
            // Η αρχική θέση ορίζεται και ως η πρώτη προσωπική καλύτερη θέση
            pbest[i][d] = x[i][d];



        }
    }


}


void OPSO::step()
{



    // Αρχικοποίηση των διαστάσεων του προβλήματος
    int dim = myProblem->getDimension();

  // =========================================================================
    // ΑΛΛΑΓΗ 1: ΥΠΟΛΟΓΙΣΜΟΣ ΤΟΥ ΓΡΑΜΜΙΚΑ ΜΕΤΑΒΑΛΛΟΜΕΝΟΥ W (Εξίσωση 2 & Πίνακας 8)
    // =========================================================================
    double w_start = w; // Βέλτιστο w_start (2.95) από πίνακα 8 του paper
    double w_end   = -0.1; // Βέλτιστο w_end (-0.1) από πίνακα 8 του paper

    // Τύπος: w = w_start - ((w_start - w_end) / MaxIterations) * CurrentIteration
    double current_w = w_start - (((w_start - w_end) * (double)iter) / (double)max_iterations);




    // 2. Αξιολόγηση Fitness & Ενημέρωση pBest/gBest με OpenMP (πάραλληλη υλοποίηση του βρόχου)
    #pragma omp parallel for // πάρε το for loop που ακολουθεί και μοίρασε τις επαναλήψεις σε όλους διαθέσιμους πυρήνες του επεξεργαστή (compiler directive της OpenMP)
    for (int i = 0; i < nParticles; ++i)
    {

        double fit;
        // Χρήση της statFunmin, της αντικειμενικής συνάρτησης
        // Κλειδώνουμε την κληση της staFunmin για να ειναι thread-safe.
        #pragma omp critical(statfunmin_eval) // κλειδώσε το παρακάτω κομμάτι κώδικα το οποίο επιτρέπεται να εκτελείται μόνο από ένα νήμα τη φορά
        {                                     // στην παρένθεση το όνομα του συγκεκριμένου section
            fit = myProblem->statFunmin(x[i]);
        }
        fitness[i] = fit;

        if (fit < pbest_fitness[i])
        {
            pbest_fitness[i] = fit;
            pbest[i] = x[i];
        }


       // Επειδή το besty και το bestx είναι κοινά για όλα τα threads,
       // χρειαζόμαστε critical section για να μην γράφουν ταυτόχρονα πολλά threads
      // κλείδωμα διπλού ελέγχου για ταχύτητα και ασφάλεια
        if (fit < besty)
         {
            #pragma omp critical(gbest_update) // αυτή είναι άλλη μια compiler directive της OpenMP και σημαίνει ότι όποιο νήμα φτάσει εδώ πρέπει να περιμένει στην ουρά. Μόνο ένα
            // νήμα τη φορά επιτρέπεται να μπει και να εκτελέσει τον κώδικα μέσα στις αγκύλες
           // το critical αναγκάζει τα νήματα να γράφουν ένα-ένα με ακρίβεια
                {
                   if (fit < besty) // Double-check (κλειδώνουμε τη σωστή τιμή)
                   {
                            besty = fit;
                            //  bestx = x[i];
                            bestx.assign(x[i].begin(),x[i].end());
                            // ΠΡΟΣΘΗΚΗ: Εκτύπωση κάθε φορά που το besty βελτιώνεται
                            // methodLogger->printMessage(QString("oPSO: Νέο Besty βρέθηκε: %1").arg(besty));
                    }
                 }
          }
    }

    // Διασφάλιση συγχρονισμού πριν προχωρήσουμε στις θέσεις
    // Όλα τα νήματα σταματούν εδώ και περιμένουν μέχρι όλα τα νήματα της παράλληλης περιοχής να φτάσουν εδώ
        #pragma omp barrier

    // 3. Ενημέρωση Ταχύτητας και Θέσης
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < nParticles; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            double r1 = dist(cell_gen[i]);
            double r2 = dist(cell_gen[i]);

            // =========================================================================
            // ΑΛΛΑΓΗ 2: ΧΡΗΣΗ ΤΟΥ CURRENT_W ΚΑΙ ΤΩΝ ΣΤΑΘΕΡΩΝ C1/C2 ΤΟΥ PAPER (Πίνακας 8)
            // =========================================================================
            // Αντικαθιστούμε το 'w' με το 'current_w' και τα c1, c2 με τα 2.82 και 12.5
            v[i][j] = current_w * v[i][j] +
                      c1 * r1 * (pbest[i][j] - x[i][j]) +
                      c2 * r2 * (bestx[j] - x[i][j]);


          // ==========================================================================
          // MΗΧΑΝΙΣΜΟΣ ΕΠΙΛΟΓΗΣ ΤΑΑΧΥΤΗΤΑΣ: Σταθερό Vmax 13,2 || Δυναμικό Vmax || IPSO
          // ==========================================================================


                    // Επιλογή Μηχανισμού
                    if (velocity_mode == "standard_vmax") {
                        // Paper opso : Πίνακας 8 --- vmax = 13.2               

                            double vmax = 13.2;
                            if (v[i][j] >  vmax) v[i][j] =  vmax; // αν η ταχύτητα πάει να ξεφύγει βάλτο στο 13.2
                            if (v[i][j] < -vmax) v[i][j] = -vmax; // βάλτο στο -13.2
                    }
                    else if (velocity_mode == "dynamic_vmax") {
                        // Η δική μου μέθοδος 
                        double range = ub[j] - lb[j]; // παίρνουμε το εύρος αφαιρώντας τα όρια
                        double progress = (double)iter / (double)max_iterations; // η πρόοδος είναι ένας δεκαδικός τρέχουσα επανάληψη / μέγιστος αρ. επαναλήψεων 
                        double dynamic_v_max = (0.2 * range) * (1.0 - 0.5 * progress); // η μέγιστη ταχύτητα είναι ίσο με 20 % του εύρους * 0,5 * την πρόοδο (ανεβαίνει σιγα σιγα)

                        if (v[i][j] >  dynamic_v_max) v[i][j] =  dynamic_v_max; // 
                        if (v[i][j] < -dynamic_v_max) v[i][j] = -dynamic_v_max;

                    } else if (velocity_mode == "ipso_vmax") {
                        double ipso_w = calculateInertia(this->inertia_type, iter, max_iterations);
                        const double c1 = 2.0;
                        const double c2 = 2.0;

                        v[i][j] = ipso_w * v[i][j] +
                                  r1 * c1 * (pbest[i][j] - x[i][j]) +
                                  r2 * c2 * (bestx[j] - x[i][j]);
                     }





            x[i][j] += v[i][j];


           // 3. ΠΕΡΙΟΡΙΣΜΟΣ ΟΡΙΩΝ 

           // Σωστός χειρισμός ορίων
               if (x[i][j] < lb[j]) {
                   x[i][j] = lb[j];
                   v[i][j] = 0.0; // Στο paper, συχνά πρέπει το σωματίδιο να "κολλάει" στο όριο
               } else if (x[i][j] > ub[j]) {
                   x[i][j] = ub[j];
                   v[i][j] = 0.0;
               }


        }
    }


 iter++; // Αυξάνουμε την επανάληψη σε κάθε κύκλο του αλγορίθμου
 if (terminated())
      {
          done();
      }

}
// o τύπος υπολογισμού της inertia w, με βάση το paper ideal pso
double OPSO::calculateInertia(int type, int current_iter, int max_iter) {
    double g = (double)current_iter / (double)max_iter;
    double w = 0.7;
   double R = (double)std::rand() / (double)RAND_MAX; ; // Ορισμός του R εδώ!

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
        default: w = 0.7; break;
    }
    return w;
}

// Ενημέρωση του fitness ενός συγκεκριμένου σωματιδίου (index)
void OPSO::setFitness(int index, double f)
{
    if(index >= 0 && index < nParticles) fitness[index] = f;
}

// Επιστροφή του fitness ενός συγκεκριμένου σωματιδίου
double OPSO::getFitness(int index) const
{
    return fitness[index];
}

// Επιστροφή των συντεταγμένων (θέσης) ενός συγκεκριμένου σωματιδίου
std::vector<double> OPSO::getGenome(int index) const
{
    return x[index];
}

// Επιστροφή της καλύτερης τιμής fitness που βρέθηκε συνολικά
double OPSO::getBestFitness() const
{
    return besty;
}

// Επιστροφή της βέλτιστης θέσης που βρέθηκε συνολικά
std::vector<double> OPSO::getBestPosition() const
{
    return bestx;
}


bool OPSO::terminated()
{
    // Κλειδώνουμε και παίρνουμε την τιμή με ασφάλεια
    double local_besty;
    #pragma omp critical(gbest_update)
    {
        local_besty = besty;
    }

    if (iter >= max_iterations)
    {
        return true;
    }

    bool t1 = false, t2 = false;
    if(terminationMethod == "similarity" || terminationMethod == "all")
    {
        // Διορθώθηκε: Περνάμε το local_besty
        t1 = similarity.terminate(local_besty);
        if(t1) return true;
    }

    if(terminationMethod == "doublebox" || terminationMethod == "all")
    {
        // Διορθώθηκε: Περνάμε το local_besty
        t2 = doubleBox.terminate(local_besty);
        if(t2) return true;
    }

    if(terminationMethod == "all") return (t1 || t2);
    return false;
}

void OPSO::done()
{
    // Η τοπική αναζήτηση παίρνει την καλύτερη θέση (bestx)
    // και την "φιλτράρει" για να βρει ακόμα καλύτερο score.
    besty = localSearch(bestx);
}

void OPSO::showDebug()
{
    // Εκτυπώνει στο log την τρέχουσα επανάληψη και το καλύτερο fitness
    methodLogger->printMessage(
        QString::asprintf("Iter=%4d BEST VALUE=%10.4lf", iter, besty));
}

// Destructor: Αποδέσμευση μνήμης και καθαρισμός όλων των vectors
OPSO::~OPSO()
{
    x.clear();
    v.clear();
    pbest.clear();
    fitness.clear();
    pbest_fitness.clear();
}
