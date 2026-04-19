/*
Είναι Header Guards. Αποτρέπουν το "διπλό include".
Αν το αρχείο κληθεί πολλές φορές σε διαφορετικά σημεία του κώδικα, ο compiler θα το διαβάσει μόνο μία φορά για να αποφύγει σφάλματα επανακαθορισμού.
*/
#ifndef OPSO_H
#define OPSO_H

/*
Αυτά τα αρχεία είναι ο πυρήνας του Global Optimus. To collection.h περιέχει δομές δεδομένων και ο optimizer.h περιέχει την βασική κλάση από την οποία
πρέπει να κληρονομήσουμε
*/

/*
Το vector χρησιμοποιείται για την αποθήκευση των δεδομένων και το random για την γεννήτρια mt19937 η οποία είναι πολύ πιο ακριβής από την απλή rand ().
*/
#include <OPTIMUS/collection.h>
#include <OPTIMUS/optimizer.h>
#include <vector>
#include <random>

/*

Εδώ δηλώνουμε ότι η κλάση OPSO είναι ένας optimizer. Αυτό επιτρέπει στο Global Optimus να την χρησιμοποιήσει πολυμορφικά (δηλαδή, να την καλέσει χωρίς να ξέρει από πριν ότι είναι η OPSO)

*/

class OPSO : public Optimizer
{
private:
    int nParticles; // Ο αριθμός των σωματιδίων (ο πληθυσμός)
    int dimension; // Ο αριθμός των παραμέτρων προς βελτιστοποίηση - διαστάσεις
    double w, c1, c2; // οι 3 σταθερές του PSO inertia, cognitive, social
    std::vector<double> lb, ub; // τα κάτω και πάνω όρια του προβλήματος


    // Μεταβλητές αποτελεσμάτων
    // std::vector<double> bestx;  // Η βέλτιστη θέση
    // double besty;               // Η βέλτιστη τιμή fitness

    // Δεδομένα Σμήνους -
    std::vector<std::vector<double>> x;  // πίνακας με τις τρέχουσες θέσεις των σωματιδίων
    std::vector<std::vector<double>> v;  // πίνακας με τις τρέχουσες ταχύτητες.
    std::vector<std::vector<double>> pbest;  // η καλύτερη θέση που έχει βρει το κάθε σωματίδιο από μόνο του
    std::vector<double> fitness;             //η τρέχουσα τιμή της συνάρτησης για κάθε σωματίδιο.
    std::vector<double> pbest_fitness;       // η καλύτερη τιμή fitness που πέτυχε το κάθε σωματίδιο.
    double besty; // η καλύτερη τιμή fitness (gBest score)
    std::vector<double> bestx; // Η καλύτερη θέση (gBest position)
    std::mt19937 gen; // η γεννήτρια που παράγει τους τυχαίους αριθμούς για τις παραμέτρους r1, r2 για της εξίσωσης κίνησης.

    // constructor

public:
    OPSO();

    virtual void init(); // override; -  Εδώ δημιουργούνται οι αρχικές τυχαίες θέσεις και μηδενίζονται οι ταχύτητες
    virtual void step(std::vector<double> &current_fitnesses); // override;- είναι η μηχανή του αλγορίθμου. Εκτελεί μια επανάληψη. Δέχεται ένα πίνακα με τα νέα fitness

    /*
    Getters & Setters.
    */
    virtual void setFitness(int index, double f); // ενημερώνει την τιμή καταλληλότητας για ένα συγκεκριμένο σωματίδιο
    virtual double getFitness(int index) const; // επιστρέφει την τρέχουσα τιμή fitness ενός σωματιδίου
    virtual std::vector<double> getGenome(int index) const; // επιστρέφει τις συντεταγμένες ενός συγκεκριμένου σωματιδίου

    virtual double getBestFitness() const; // Επιστρέφει την καλύτερη τιμή (το παγκόσμιο ελάχιστο) που έχει βρει ο OPSO από την αρχή της εκτέλεσης μέχρι τώρα
    virtual std::vector<double> getBestPosition() const; //Επιστρέφει το διάνυσμα των παραμέτρων (το gbest) που αντιστοιχεί στην καλύτερη λύση.

    virtual ~OPSO(); // καθαρίζει την μνήμη όταν ο αλγόριθμος τελειώσει τη δουλειά του.
};

#endif
