#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "structure_ta.h"


/*---------Description du modèle----------------------------------*/
/*
Nb_loclité = 12
                  X>=1        X>=1         x>=4       x>=4
AUTOMATE 01 : L0 --a-->l1; l0 --a-->l2; l1--c--l0; l2--c--l0
                                           x=0         x=0

                  y>=1         y>=1          y>=4        y>=4          y>=2         y>=4
AUTOMATE 02 : l3 --c-->l4; l3 --c-->l5;  l4 --b-->l3; l5 --b-->l3; l5 --d-->l6; l6 --b-->l5;
                                            y=0            y=0
INVARIANTS
x<=4; y<=4   apart dans l6 y<=3

*/

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
    int nb_locations = 12;
    int nb_actions = 4;

    locations = malloc(nb_locations * sizeof(char*));
    invariants = malloc(nb_locations * sizeof(DBM*));
    actions = malloc(nb_actions * sizeof(char*));
    nb_trans_par_location = malloc(nb_locations * sizeof(int));
    transitions = malloc(nb_locations * sizeof(Transition*));
    update_functions = malloc(nb_actions * sizeof(UpdateFunction));
    constraints = malloc(nb_actions * sizeof(Constraint));

    // Locations
    locations[0] = "l0l3";
    locations[1] = "l0l4";
    locations[2] = "l1l3";
    locations[3] = "l1l4";
    locations[4] = "l2l3";
    locations[5] = "l2l4";
    locations[6] = "l0l5";
    locations[7] = "l1l5";
    locations[8] = "l2l5";
    locations[9] = "l0l6";
    locations[10] = "l1l6";
    locations[11] = "l2l6";


    
    // Invariants
    static DBM i_0 = {{0,0,0},{4,0,infty},{4,infty,0}};
    static DBM i_6 = {{0,0,0},{4,0,infty},{3,infty,0}};
    invariants[0] = &i_0;
    invariants[1] = &i_0;
    invariants[2] = &i_0;
    invariants[3] = &i_0;
    invariants[4] = &i_0;
    invariants[5] = &i_0;
    invariants[6] = &i_0;
    invariants[7] = &i_0;
    invariants[8] = &i_0;
    invariants[9] = &i_6;
    invariants[10] = &i_6;
    invariants[11] = &i_6;


    // Actions
    actions[0] = "a";
    actions[1] = "b";
    actions[2] = "c";
    actions[3] = "d";


    // Transitions
    nb_trans_par_location[0] = 4;
    transitions[0] = malloc(nb_trans_par_location[0] * sizeof(Transition));
    transitions[0][0] = (Transition){.location_in = 1, .label_action = 2, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[0][1] = (Transition){.location_in = 2, .label_action = 0, .guard = {{0,-1,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[0][2] = (Transition){.location_in = 4, .label_action = 0, .guard = {{0,-2,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[0][3] = (Transition){.location_in = 6, .label_action = 2, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    

    nb_trans_par_location[1] = 3;
    transitions[1] = malloc(nb_trans_par_location[1] * sizeof(Transition));
    transitions[1][0] = (Transition){.location_in = 0, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,0}};
    transitions[1][1] = (Transition){.location_in = 3, .label_action = 0, .guard = {{0,-1,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[1][2] = (Transition){.location_in = 5, .label_action = 0, .guard = {{0,-2,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};

    nb_trans_par_location[2] = 3;
    transitions[2] = malloc(nb_trans_par_location[2] * sizeof(Transition));
    transitions[2][0] = (Transition){.location_in = 3, .label_action = 2, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[2][1] = (Transition){.location_in = 0, .label_action = 2, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[2][2] = (Transition){.location_in = 7, .label_action = 2, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    

    nb_trans_par_location[3] = 2;
    transitions[3] = malloc(nb_trans_par_location[3] * sizeof(Transition));
    transitions[3][0] = (Transition){.location_in = 2, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,0}};
    transitions[3][1] = (Transition){.location_in = 1, .label_action = 2, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    
    nb_trans_par_location[4] = 3;
    transitions[4] = malloc(nb_trans_par_location[4] * sizeof(Transition));
    transitions[4][0] = (Transition){.location_in = 5, .label_action = 2, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[4][1] = (Transition){.location_in = 0, .label_action = 1, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[4][2] = (Transition){.location_in = 8, .label_action = 2, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};


    nb_trans_par_location[5] = 2;
    transitions[5] = malloc(nb_trans_par_location[5] * sizeof(Transition));
    transitions[5][0] = (Transition){.location_in = 4, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,0}};
    transitions[5][1] = (Transition){.location_in = 1, .label_action = 1, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};  
    
    nb_trans_par_location[6] = 4;
    transitions[6] = malloc(nb_trans_par_location[6] * sizeof(Transition));
    transitions[6][0] = (Transition){.location_in = 0, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,0}};
    transitions[6][1] = (Transition){.location_in = 7, .label_action = 0, .guard = {{0,-1,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[6][2] = (Transition){.location_in = 8, .label_action = 0, .guard = {{0,-2,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[6][3] = (Transition){.location_in = 9, .label_action = 3, .guard = {{0,0,-2},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};


    nb_trans_par_location[7] = 3;
    transitions[7] = malloc(nb_trans_par_location[7] * sizeof(Transition));
    transitions[7][0] = (Transition){.location_in = 2, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,0}};
    transitions[7][1] = (Transition){.location_in = 6, .label_action = 2, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[7][2] = (Transition){.location_in = 10, .label_action = 3, .guard = {{0,0,-2},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};


    nb_trans_par_location[8] = 3;
    transitions[8] = malloc(nb_trans_par_location[8] * sizeof(Transition));
    transitions[8][0] = (Transition){.location_in = 4, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,0}};
    transitions[8][1] = (Transition){.location_in = 6, .label_action = 1, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[8][2] = (Transition){.location_in = 11, .label_action = 3, .guard = {{0,0,-2},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};

   nb_trans_par_location[9] = 3;
   transitions[9] = malloc(nb_trans_par_location[9] * sizeof(Transition));
   transitions[9][0] = (Transition){.location_in = 6, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
   transitions[9][1] = (Transition){.location_in = 10, .label_action = 0, .guard = {{0,-1,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
   transitions[9][2] = (Transition){.location_in = 11, .label_action = 0, .guard = {{0,-2,0},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};

    nb_trans_par_location[10] = 2;
    transitions[10] = malloc(nb_trans_par_location[10] * sizeof(Transition));
    transitions[10][0] = (Transition){.location_in = 9, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};
    transitions[10][1] = (Transition){.location_in = 7, .label_action = 2, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
   

     nb_trans_par_location[11] = 2;
    transitions[11] = malloc(nb_trans_par_location[11] * sizeof(Transition));
    transitions[11][0] = (Transition){.location_in = 9, .label_action = 1, .guard = {{0,-4,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[11][1] = (Transition){.location_in = 8, .label_action = 1, .guard = {{0,0,-4},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};


    

}

// --------------------- Initialisation des variables ---------------------

void init_variables() { 
    variable.v = 0;
    variable.x = 0;
    variable.active=false;
    variable.table_size = 3;
    variable.table[0] = 0;
    variable.table[1] = 0;
    variable.table[2] = 0;
    strcpy(variable.name, "start");
}

// --------------------- Update functions ---------------------

// Variable update_a(Variable var) { if (var.v + 2 <= 100 && var.v + 2 >= -10) var.v += 2; return var; }
// Variable update_b(Variable var) { if (var.v + 1 <= 100 && var.v + 1 >= -10) var.v += 1; return var; }//pour garantir acyclicité - --> +
// Variable update_c(Variable var) { if (var.v * 2 <= 100 && var.v * 2 >= -10) var.v *= 2; return var; }

Variable update_a(Variable var) {
    

   
     if (var.v + 2 <= 500 && var.v + 2 >= -10){ 
        
        var.v += 2;
        var.x += 1;
        var.active = true;
        var.table[0]++;
        var.table[1] = var.v;
        var.table[2] = var.v;
        snprintf(var.name, NAME_SIZE, "transition a");
    } 
  
    return var;
}

Variable update_b(Variable var) {
   
   
   
    if (var.v + 1 <= 500 && var.v + 1 >= -10){
        
        var.v += 1;
        var.x += 1;
        var.active = true;
         var.table[0]++;
        var.table[1] = var.v;
        var.table[2] = var.v;
        snprintf(var.name, NAME_SIZE, "transition b");
    } 

    
    return var;
}

Variable update_c(Variable var) {
  
    if (var.v * 2 <= 500 && var.v * 2 >= -10) {

      var.v *= 2;
      var.x += 1;
      var.active = false;
      var.table[0]++;
      var.table[1] = var.v;
      var.table[2] = var.v;
      snprintf(var.name, NAME_SIZE, "transition c");
    }
 
    return var;
}
Variable update_d(Variable var) {


      if (var.v * 2 <= 500 && var.v * 2 >= -10) {

      var.v += 2;
      var.x += 1;
      var.active = false;
      var.table[0]++;
      var.table[1] = var.v;
      var.table[2] = var.v;
      snprintf(var.name, NAME_SIZE, "transition c");
    }
 
    return var;
}
void init_update_functions() {
    update_functions[0] = update_a;
    update_functions[1] = update_b;
    update_functions[2] = update_c;
    update_functions[3] = update_d;

}

// --------------------- Contraintes ---------------------

bool const_a(Variable var) { return true; }
bool const_b(Variable var) { return true; }
bool const_c(Variable var) { return true; }
bool const_d(Variable var) { return true; }


void init_constraints() {
    constraints[0] = const_a;
    constraints[1] = const_b;
    constraints[2] = const_c;
    constraints[3] = const_d;

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
