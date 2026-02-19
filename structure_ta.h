#ifndef STRUCTURE_TA_H
#define STRUCTURE_TA_H

#include "structure_variable.h"
#include "structure_DBM.h"
#define TABLE_SIZE 3
#define NAME_SIZE 50

typedef Variable (*UpdateFunction)(Variable);
typedef bool (*Constraint)(Variable);

typedef struct Transition {
    int location_in;
    int label_action;
    DBM guard;
    clocks reset;
} Transition;

typedef struct {
    char** locations;                 // Locations du TA
    DBM** invariants;                   // Invariant de chaque location                     
    char** actions;                   // Actions du TA
    int* nb_trans_par_location;       // Nombre de transitions sortantes par état
    Transition** transitions;        // Transitions sortantes de chaque état
    Variable variable;               // Variable de données
    UpdateFunction* update_functions; // Fonctions d'update
    Constraint* constraints;         // Contraintes
} TA;

#endif
