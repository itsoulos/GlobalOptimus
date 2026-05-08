# ifndef __OPSO__H
# define __OPSO__H
#include <OPTIMUS/collection.h>
#include <OPTIMUS/optimizer.h>
#include <vector>
#include <random>

/*

Εδώ δηλώνουμε ότι η κλάση OPSO είναι ένας optimizer. Αυτό επιτρέπει στο Global O

*/

class OPSO : public Optimizer
{
private:


    std::vector<double> lb, ub; // τα όρια
    double besty;
    std::vector<double> bestx;
    std::vector<double> fitness;


    int nParticles; // Ο αριθμός των σωματιδίων (ο πληθυσμός)
    //  int dimension; // Ο αριθμός των παραμέτρων προς βελτιστοποίηση - διαστάσ
    double w, c1, c2; // οι 3 σταθερές του PSO inertia, cognitive, social
    int iter; // Μεταβλητή για την τρέχουσα επανάληψη
    int max_iterations; // Το μέγιστο όριο των επαναλήψεων




    // Δεδομένα Σμήνους -
    std::vector<std::vector<double>> x;  // πίνακας με τις τρέχουσες θέσεις των
    std::vector<std::vector<double>> v;  // πίνακας με τις τρέχουσες ταχύτητες.
    std::vector<std::vector<double>> pbest;  // η καλύτερη θέση που έχει βρει το

    std::vector<double> pbest_fitness;       // η καλύτερη τιμή fitness που πέτυ

    std::mt19937 gen; // η γεννήτρια που παράγει τους τυχαίους αριθμούς για τις

    // constructor

public:
    OPSO();

    virtual void init(); // override; -  Εδώ δημιουργούνται οι αρχικές τυχαίες θ
    virtual void step(); // override;- είναι η μηχανή του αλγορίθμου. Εκτελεί μι
    virtual bool terminated (); // Η συνάρτηση τερματισμού του αλγορίθμου
    virtual void done();
    virtual void showDebug();
    /*
    Getters & Setters.
    */
    virtual void setFitness(int index, double f); // ενημερώνει την τιμή καταλλη
    virtual double getFitness(int index) const; // επιστρέφει την τρέχουσα τιμή
    virtual std::vector<double> getGenome(int index) const; // επιστρέφει τις συ

    virtual double getBestFitness() const; // Επιστρέφει την καλύτερη τιμή (το π
    virtual std::vector<double> getBestPosition() const; //Επιστρέφει το διάνυσμ

    virtual ~OPSO(); // καθαρίζει την μνήμη όταν ο αλγόριθμος τελειώσει τη δουλε
};

# endif
