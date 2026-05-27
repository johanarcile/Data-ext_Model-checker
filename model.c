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
    int nb_locations = 2;
    int nb_actions = 3;

    locations = malloc(nb_locations * sizeof(char*));
    invariants = malloc(nb_locations * sizeof(DBM*));
    actions = malloc(nb_actions * sizeof(char*));
    nb_trans_par_location = malloc(nb_locations * sizeof(int));
    transitions = malloc(nb_locations * sizeof(Transition*));
    update_functions = malloc(nb_actions * sizeof(UpdateFunction));
    constraints = malloc(nb_actions * sizeof(Constraint));

    // Locations
    locations[0] = "l0";
    locations[1] = "l1";
    
    // Invariants
    static DBM i_0 = {{0,0,0},{2,0,infty},{2,infty,0}};
    static DBM i_1 = {{0,0,0},{2,0,infty},{2,infty,0}};
    invariants[0] = &i_0;
    invariants[1] = &i_1;

    // Actions
    actions[0] = "a";
    actions[1] = "b";
    actions[2] = "c";

    // Transitions
    nb_trans_par_location[0] = 2;
    nb_trans_par_location[1] = 1;
    transitions[0] = malloc(nb_trans_par_location[0] * sizeof(Transition));
    transitions[1] = malloc(nb_trans_par_location[1] * sizeof(Transition));
    transitions[0][0] = (Transition){.location_in = 0, .label_action = 2, .guard = {{0,-1,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[0][1] = (Transition){.location_in = 1, .label_action = 0, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[1][0] = (Transition){.location_in = 0, .label_action = 1, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {0,0}};
}

// --------------------- Initialisation des variables ---------------------

void init_variables() { 
    //memset(&variable, 0, sizeof(Variable));
    variable.v = 0;
    variable.x = 0;
    variable.active=false;
    variable.table[0] = 0;
    variable.table[1] = 0;
    variable.table[2] = 0;
    strcpy(variable.name, "start");
    variable.n_size = 3;
    variable.n = malloc(variable.n_size * sizeof(Name));
    variable.n[0].nom_size = 2;
    variable.n[1].nom_size = 2;
    variable.n[2].nom_size = 2;
    variable.n[0].nom = malloc(variable.n[0].nom_size * sizeof(char*));
    variable.n[1].nom = malloc(variable.n[1].nom_size * sizeof(char*));
    variable.n[2].nom = malloc(variable.n[2].nom_size * sizeof(char*));
    variable.n[0].nom[0] = "nom variable";
    variable.n[0].nom[1] = "nom variable";
    variable.n[1].nom[0] = "nom variable";
    variable.n[1].nom[1] = "nom variable";
    variable.n[2].nom[0] = "nom variable";
    variable.n[2].nom[1] = "nom variable";
    variable.n[0].nom_size_1 = 13;
    variable.n[1].nom_size_1 = 13;
    variable.n[2].nom_size_1 = 13;
}

// --------------------- Update functions ---------------------

Variable update_a(Variable variable) {
     if (variable.v + 2 <= 500 && variable.v + 2 >= -10){
        variable.v += 2;
        variable.active = false;
        variable.x += 1;
        // variable.table[0]++;
        // variable.table[1] = variable.v;
        // variable.table[2] = variable.v;
        // snprintf(variable.name, NAME_SIZE, "transition a");
    }
    return variable;
}

Variable update_b(Variable variable) {
    if (variable.v + 1 <= 500 && variable.v + 1 >= -10){
        variable.v += 1;
        variable.v *= 2;
        variable.active = false;
        variable.x += 1;
        // variable.table[0]++;
        // variable.table[1] = variable.v;
        // variable.table[2] = variable.v;
        // snprintf(variable.name, NAME_SIZE, "transition b");
    }
    return variable;
}

Variable update_c(Variable variable) {
    if (variable.v * 2 <= 500 && variable.v * 2 >= -10) {
        variable.v *= 2;
        variable.active = false;
        variable.x += 1;
        //   variable.table[0]++;
        //   variable.table[1] = variable.v;
        //   variable.table[2] = variable.v;
        //   snprintf(variable.name, NAME_SIZE, "transition c");
    }
    return variable;
}

void init_update_functions() {
    update_functions[0] = update_a;
    update_functions[1] = update_b;
    update_functions[2] = update_c;
}

// --------------------- Contraintes ---------------------

bool const_a(Variable variable) {
return true;
}

bool const_b(Variable variable) {
return true;
}

bool const_c(Variable variable) {
return true;
}

void init_constraints() {
    constraints[0] = const_a;
    constraints[1] = const_b;
    constraints[2] = const_c;
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
