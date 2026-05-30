/*Konstantinos Barkas
Informatics & Telecommunications - UOI
*/

#include "opso.h"

/* * Constructor: Ορίζουμε τις παραμέτρους που θα εμφανίζονται στο GUI
 * του Global Optimus.
 */
OPSO::OPSO()
{
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
        similarity.setSimilarityIterations(20);


     max_iterations = getParam("opso_maxiters").getValue().toInt();
    // printf("DEBUG: Max Iterations is %d\n", max_iterations);

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
    gen.seed(std::random_device()());
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
            x[i][d] = lb[d] + dist(gen) * (ub[d] - lb[d]);
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
    //  ΥΠΟΛΟΓΙΣΜΟΣ ΤΟΥ ΓΡΑΜΜΙΚΑ ΜΕΤΑΒΑΛΛΟΜΕΝΟΥ W (Εξίσωση 2 & Πίνακας 8)
    // =========================================================================
    double w_start = w; // Βέλτιστο w_start (2.95) από πίνακα 8 του paper
    double w_end   = -0.1; // Βέλτιστο w_end (-0.1) από πίνακα 8 του paper
    
    // Τύπος: w = w_start - ((w_start - w_end) / MaxIterations) * CurrentIteration
    double current_w = w_start - (((w_start - w_end) * (double)iter) / (double)max_iterations);


    // 2. Αξιολόγηση Fitness & Ενημέρωση pBest/gBest
    for (int i = 0; i < nParticles; ++i)
    {
        // Χρήση της statFunmin, της αντικειμενικής συνάρτησης
        double fit = myProblem->statFunmin(x[i]);
        fitness[i] = fit;

        if (fit < pbest_fitness[i])
        {
            pbest_fitness[i] = fit;
            pbest[i] = x[i];
        }

        if (fit < besty)
        {
            besty = fit;
            bestx = x[i];
        }
    }

    // 3. Ενημέρωση Ταχύτητας και Θέσης
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < nParticles; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            double r1 = dist(gen);
            double r2 = dist(gen);

            // =========================================================================
            // ΧΡΗΣΗ ΤΟΥ CURRENT_W ΚΑΙ ΤΩΝ ΣΤΑΘΕΡΩΝ C1/C2 ΤΟΥ PAPER (Πίνακας 8)
            // =========================================================================
            // Αντικαθιστούμε το 'w' με το 'current_w' και τα c1, c2 με τα 2.82 και 12.5
            v[i][j] = current_w * v[i][j] +
                      c1 * r1 * (pbest[i][j] - x[i][j]) +
                      c2 * r2 * (bestx[j] - x[i][j]);

         
            // ==========================================================================
            // ΣΤΑΘΕΡΟ V_MAX ΓΙΑ ΟΛΕΣ ΤΙΣ ΔΙΑΣΤΑΣΕΙΣ (Πίνακας 8) - ΦΡΑΞΙΜΟ ΤΑΧΥΤΗΤΑΣ 
            // ==========================================================================
            double v_max = 13.2; 
            
            if (v[i][j] > v_max)  v[i][j] = v_max;
            if (v[i][j] < -v_max) v[i][j] = -v_max;          

            x[i][j] += v[i][j];

            // Περιορισμός στα όρια lb και ub του Optimizer
            if (x[i][j] <  lb[j]) x[i][j] =  lb[j];
            if (x[i][j] >  ub[j]) x[i][j] =  ub[j];
        }
    }

    // =========================================================================
    //  ΔΥΝΑΜΙΚΗ ΑΛΛΑΓΗ ορίων νευρωνικού
    // =========================================================================
    /*if (iter > 0 && iter % 50 == 0) // με αυτή την εντολή κάθε 50 επαναλήψεις μειώνει το εύρος
    {
        for (int j = 0; j < dim; ++j)
        {
            double current_range = ub[j] - lb[j];
            double new_range = current_range * 0.95; // Μείωση εύρους κατά 5%
            
            lb[j] = bestx[j] - (new_range / 2.0);
            ub[j] = bestx[j] + (new_range / 2.0);
        }
    }*/
    // =========================================================================
 iter++; // Αυξάνουμε την επανάληψη σε κάθε κύκλο του αλγορίθμου
 if (terminated())
      {
          done();
      }
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



// 1. Τοπική δήλωση besty
      double local_besty;
      local_besty = besty; // Παίρνουμε την τρέχουσα καλύτερη τιμή που βρήκε το PSO

        // Έλεγχος αν η τρέχουσα επανάληψη (iter) έφτασε ή ξεπέρασε το όριο
        if (iter >= max_iterations)
        {
            return true; // Σταμάτησε τον αλγόριθμο
        }


        /*
        // Εδώ χρησιμοποιούμε ακριβώς τις έτοιμες μεθόδους του framework
           if (terminationMethod == "doublebox")
            {
                return doubleBox.terminate(besty);
            }
            else if (terminationMethod == "similarity")
            {
                return similarity.terminate(besty);
            }
         */
         bool t1=false,t2=false;
             if(terminationMethod=="similarity" || terminationMethod=="all")
             {

                 t1=similarity.terminate(besty);
             if(t1) return true;
             }

             if(terminationMethod=="doublebox" || terminationMethod=="all")
             {
                 t2=doubleBox.terminate(besty);
             if(t2) return true;
             }
             if(terminationMethod=="all") return (t1 || t2);
             return false;


        return false; // Συνέχισε στην επόμενη επανάληψη

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
