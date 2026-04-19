
#include "opso.h"

/* * Constructor: Ορίζουμε τις παραμέτρους που θα εμφανίζονται στο GUI
 * του Global Optimus.
 */
OPSO::OPSO()
{
    // Προσθήκη παραμέτρου για το πλήθος των σωματιδίων με εύρος 10 έως 2000
    addParam(Parameter("opso_particles", 100, 10, 2000, "Number of particles"));
    // Προσθήκη παραμέτρου για το βάρος αδράνειας (Inertia Weight)
    addParam(Parameter("opso_w", 0.5, 0.1, 1.0, "Inertia weight (w)"));
    // Προσθήκη παραμέτρου c1 για την έλξη προς την προσωπική καλύτερη θέση
    addParam(Parameter("opso_c1", 2.0, 0.0, 4.0, "Cognitive constant (c1)"));
    // Προσθήκη παραμέτρου c2 για την έλξη προς την ομαδική καλύτερη θέση
    addParam(Parameter("opso_c2", 2.0, 0.0, 4.0, "Social constant (c2)"));
}

/*
 * init: Αρχικοποίηση πληθυσμού και μνήμης.
 * Τραβάει τις τιμές από το σύστημα παραμέτρων και χρησιμοποιεί τα όρια του προβλήματος.
 */
void OPSO::init()
{
    //  Ανάκτηση της επιλεγμένης τιμής για τον αριθμό των σωματιδίων
    nParticles = getParam("opso_particles").getValue().toInt();
    // Ανάκτηση της τιμής της αδράνειας w
    w = getParam("opso_w").getValue().toDouble();
    // Ανάκτηση της σταθεράς c1
    c1 = getParam("opso_c1").getValue().toDouble();
    // Ανάκτηση της κοινωνικής σταθεράς c2
    c2 = getParam("opso_c2").getValue().toDouble();

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

/*
 * step: Εκτέλεση μιας επανάληψης του αλγορίθμου.
 */
void OPSO::step(std::vector<double> &current_fitnesses)
{
    // Δημιουργία κατανομής για τους τυχαίους συντελεστές r1, r2
    std::uniform_real_distribution<double> rDist(0.0, 1.0);

    // Φάση 1: Ενημέρωση μνήμης (pBest και gBest)
    for (int i = 0; i < nParticles; ++i)
    {
        // Αποθήκευση του fitness που υπολογίστηκε από το framework
        fitness[i] = current_fitnesses[i];

        // Έλεγχος αν η τρέχουσα θέση είναι καλύτερη από την προσωπική καλύτερη
        if (fitness[i] < pbest_fitness[i])
        {
            // Ενημέρωση του προσωπικού ρεκόρ fitness
            pbest_fitness[i] = fitness[i];
            // Αποθήκευση της τρέχουσας θέσης ως νέα pbest
            pbest[i] = x[i];

            // Έλεγχος αν η νέα pbest είναι καλύτερη από την παγκόσμια καλύτερη (gBest)
            if (pbest_fitness[i] < besty)
            {
                // Ενημέρωση του παγκόσμιου ρεκόρ fitness
                besty = pbest_fitness[i];
                // Αποθήκευση της θέσης ως νέα παγκόσμια βέλτιστη
                bestx = pbest[i];
            }
        }
    }

    // Φάση 2: Κίνηση σωματιδίων βάσει των εξισώσεων του Paper (Table 9)
    for (int i = 0; i < nParticles; ++i)
    {
        for (int d = 0; d < dimension; ++d)
        {
            // Παραγωγή τυχαίων αριθμών r1 και r2 για τη στοχαστικότητα της κίνησης
            double r1 = rDist(gen);
            double r2 = rDist(gen);

            // Εφαρμογή της εξίσωσης ταχύτητας: v = w*v + c1*r1*(pbest-x) + c2*r2*(gbest-x)
            v[i][d] = w * v[i][d] +
                      c1 * r1 * (pbest[i][d] - x[i][d]) +
                      c2 * r2 * (bestx[d] - x[i][d]);

            // Ενημέρωση της θέσης του σωματιδίου προσθέτοντας τη νέα ταχύτητα
            x[i][d] += v[i][d];

            // Περιορισμός της θέσης αν ξεπερνά το κάτω όριο
            if (x[i][d] < lb[d]) x[i][d] = lb[d];
            // Περιορισμός της θέσης αν ξεπερνά το πάνω όριο (Boundary Control)
            if (x[i][d] > ub[d]) x[i][d] = ub[d];
        }
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


// Destructor: Αποδέσμευση μνήμης και καθαρισμός όλων των vectors
OPSO::~OPSO()
{
    x.clear();
    v.clear();
    pbest.clear();
    fitness.clear();
    pbest_fitness.clear();
}
