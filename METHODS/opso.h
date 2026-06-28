/*Konstantinos Barkas -- Parallel and Optimized Particle Swarm Optimization --- Dept. Informatics and Telecommunications UOI*/
# ifndef __OPSO__H
# define __OPSO__H
#include <chrono> // Απαραίτητο για το std::chrono::time_point
#include <QString> // Απαραίτητο για τη χρήση τύπων QString
# include <OPTIMUS/optimizer.h>

/*
  Κλάση OPSO: Υλοποίηση ενός παράλληλου υβριδικού μετα-ευρετικού αλγορίθμου PSO (Parallel Optimized PSO).
  Κληρονομεί από την κλάση Optimizer.
*/
class OPSO : public Optimizer
{
private:
    // --- Χρονισμός ---
    std::chrono::time_point<std::chrono::system_clock> before, after; // Χρονικά σημεία για μέτρηση απόδοσης, πριν + μετά για την μέτρηση της χρονικής διάρκειας του αλγορίθμου

    // --- Παράμετροι Εκτέλεσης ---
    QString velocity_mode; // Καθορίζει τον μηχανισμό υπολογισμού της ταχύτητας σωματιδίων
    int inertia_type;      // Τύπος αδράνειας (0-14 με βάση των κώδικα switch case από αλγόριθμο ipso στο optimus)
    int threads;           // Αριθμός νημάτων για παράλληλη επεξεργασία
    std::vector<double> lb, ub; // Όρια αναζήτησης: Lower Bound  (lb) και Upper Bound (ub)

    // --- Κατάσταση Βέλτιστης Λύσης ---
    double besty;                // Η καλύτερη τιμή που βρέθηκε παγκοσμίως, global best από αντικειμενική συνάρτηση
    std::vector<double> bestx;   // Η θέση που αντιστοιχεί στην καλύτερη παγκόσμια τιμή besty
    std::vector<double> fitness; // Τρέχουσες τιμές fitness για κάθε σωματίδιο

    double prev_fitness_sum; // Κρατάει το άθροισμα fitness της προηγούμενης επανάληψης για έλεγχο σύγκλισης
    int S_delta;             // Παγκόσμιος μετρητής στασιμότητας

    std::vector<int> stagnation_counter; // Μετρητής στασιμότητας

    // --- Παράμετροι Νησιών  ---
    int subPopulation;      // Ενεργοποίηση/Αριθμός υποπληθυσμών
    int sub_size;           // Μέγεθος κάθε υποπληθυσμού
    int propagationRate;    // Συχνότητα ανταλλαγής πληροφορίας μεταξύ νησιών
    int propagationNumber;  // Πλήθος ατόμων που μεταναστεύουν
    QString propagationMethod; // Στρατηγική μετανάστευσης
    int prop;               // Μετρητής για τον κύκλο διάδοσης (propagation)
    double localsearchRate; // Πιθανότητα εκτέλεσης τοπικής αναζήτησης
    int subPopEnable;       // σημαία ενεργοποίησης δομής νησιών
    QString subPopEnableStr; // Αναπαράσταση του subPopEnable σε κείμενο
    QString termination;    // Κριτήριο τερματισμού (similarity, doublebox)

    // --- Βοηθητικά δεδομένα υποπληθυσμών ---
    std::vector<double> bestF2x;      // Καλύτερη θέση υποπληθυσμού 2
    std::vector<double> bestF2xOld;   // Προηγούμενη καλύτερη θέση υποπληθυσμού 2
    std::vector<std::vector<double>> bestSamply; // Αποθήκευση δειγμάτων για έλεγχο κατάστασης

    // --- Μηχανισμοί τερματισμού ανά νήμα ---
    std::vector<Similarity> threadSimilarity; // Έλεγχος similarity πληθυσμού
    std::vector<DoubleBox> threadDoublebox;   // Έλεγχος double box πλυθυσμού

    // --- ΒΟΗΘΗΤΙΚΕΣ ΜΕΘΟΔΟΙ ΝΗΣΙΩΝ ---
    bool checkSubCluster(int subCluster);            // Ελέγχει την κατάσταση ενός υποπληθυσμού
    void propagate();                                // Διαχειρίζεται τη μετακίνηση ατόμων μεταξύ νησιών
    void propagateBetween(int sender, int receiver); // Μεταφέρει δεδομένα μεταξύ δύο συγκεκριμένων νησιών
    void updateLeadersForSubpop(int k);              // Ενημερώνει τους "ηγέτες" ανά υποπληθυσμό
    double randomDouble(double min, double max);     // Παραγωγή τυχαίου δεκαδικού σε εύρος [min, max]

    // --- Παράμετροι PSO ---
    int nParticles;    // Συνολικός αριθμός σωματιδίων
    int dimension;     // Διαστάσεις του προβλήματος βελτιστοποίησης
    double w, c1, c2;  // Παράμετροι PSO: Αδράνεια (w), γνωστική (c1) και κοινωνική (c2) συνιστώσα
    int iter;          // Τρέχουσα επανάληψη
    int max_iterations;// Μέγιστος αριθμός επαναλήψεων

    // --- Δεδομένα Σμήνους  ---
    std::vector<std::vector<double>> x;      // Θέσεις σωματιδίων
    std::vector<std::vector<double>> v;      // Ταχύτητες σωματιδίων
    std::vector<std::vector<double>> pbest;  // Προσωπική καλύτερη θέση κάθε σωματιδίου
    std::vector<double> pbest_fitness;       // Fitness της προσωπικής καλύτερης θέσης
    std::vector<std::mt19937> cell_gen;      // Γεννήτριες τυχαίων αριθμών ανά νήμα (Mersenne Twister)

public:
    OPSO(); // Κατασκευαστής της κλάσης

    // --- Virtual μέθοδοι (πολυμορφισμός) ---
    virtual void init();                                         // Αρχικοποίηση πληθυσμού και μεταβλητών
    virtual void step();                                         // Εκτέλεση μίας επανάληψης του αλγορίθμου
    virtual double calculateInertia(int type, int current_iter, int max_iter,std::mt19937& gen); // Υπολογισμός δυναμικής αδράνειας
    virtual bool terminated ();                                  // Έλεγχος συνθηκών τερματισμού
    virtual void done();                                         // Ενέργειες μετά την ολοκλήρωση
    virtual void showDebug();                                    // Εκτύπωση πληροφοριών αποσφαλμάτωσης

    // --- Getters & Setters ---
    virtual void setFitness(int index, double f);                // Θέτει το fitness ενός σωματιδίου
    virtual double getFitness(int index) const;                  // Επιστρέφει το fitness ενός σωματιδίου
    virtual std::vector<double> getGenome(int index) const;      // Επιστρέφει τη θέση ενός σωματιδίου
    virtual double getBestFitness() const;                       // Επιστρέφει το καλύτερο παγκόσμιο fitness
    virtual std::vector<double> getBestPosition() const;         // Επιστρέφει τη θέση της καλύτερης λύσης

    void    localCrossover(int thread,int pos);
    void    localMutate(int pos);
    virtual double getLevyStep(double beta,int i); // levy flights για αποφυγη πρόωρης συγκλισης και τοπικών ελάχιστων
    virtual ~OPSO(); // Καταστροφέας της κλάσης
};

#endif
