#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "structure_ta.h"


// --------------------- Variables globales ---------------------

char** locations;                 // Locations du TA
DBM** invariants;                   // Invariant de chaque location                     
char** actions;                   // Actions du TA
int* nb_trans_par_location;       // Nombre de transitions sortantes par état
Transition** transitions;        // Transitions sortantes de chaque état
Variable variable;               // Variable de données
UpdateFunction* update_functions; // Fonctions d'update
Constraint* constraints;         // Contraintes

// ---------------------Instantiation TA ---------------------

void init_ta() { //CAN BE OPTIMIZED BY #define NB_LOCATIONS AND NB_ACTIONS, AND HAVING ALL VARIABLES BE ARRAYS
    int nb_locations = model_c_nblocations;
    int nb_actions = model_c_nbactions;

    locations = malloc(nb_locations * sizeof(char*));
    invariants = malloc(nb_locations * sizeof(DBM*));
    actions = malloc(nb_actions * sizeof(char*));
    nb_trans_par_location = malloc(nb_locations * sizeof(int));
    transitions = malloc(nb_locations * sizeof(Transition*));
    update_functions = malloc(nb_actions * sizeof(UpdateFunction));
    constraints = malloc(nb_actions * sizeof(Constraint));

    // Locations
    model_c_locations
    
    // Invariants
    model_c_invariants

    // Actions
    model_c_actions

    // Transitions
    model_c_transitions
}

// --------------------- Initialisation des variables ---------------------

void init_variables() { 
    model_c_init_variables
}

// --------------------- Update functions ---------------------

model_c_update_functions
void init_update_functions() {
    model_c_init_update_functions
}

// --------------------- Contraintes ---------------------

model_c_constraints
void init_constraints() {
    model_c_init_constraints
}

// --------------------- Remplir la structure TA ---------------------

void fill_ta_struct(TA* ta) {
    init_ta();
    init_variables();
    init_update_functions();
    init_constraints();

    ta->locations = locations;
    ta->invariants = invariants;
    ta->actions = actions;
    ta->nb_trans_par_location = nb_trans_par_location;
    ta->transitions = transitions;
    ta->variable = variable;
    ta->update_functions = update_functions;
    ta->constraints = constraints;
}
