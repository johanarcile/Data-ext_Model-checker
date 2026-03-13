#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "structure_ta.h"


/*---------Description du modèle----------------------------------*/
/*
Nb_loclité = 2 Action = 3
                  X>=1         y>=1         Y>=1
AUTOMATE 01 : L0 --c-->l0; L0 --a-->l1; L1 --b-->l0;
                 x=0                      x=0, y = 0

                

INVARIANT  x et y <= 8;

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
    static DBM i_0 = {{0,0,0},{8,0,infty},{8,infty,0}};
    invariants[0] = &i_0;
    invariants[1] = &i_0;
   

    // Actions
    actions[0] = "a";
    actions[1] = "b";
    actions[2] = "c";

    // Transitions
    nb_trans_par_location[0] = 2;
    transitions[0] = malloc(nb_trans_par_location[0] * sizeof(Transition));
    transitions[0][0] = (Transition){.location_in = 0, .label_action = 2, .guard = {{0,-1,0},{infty,0,infty},{infty,infty,0}}, .reset = {0,infty}};
    transitions[0][1] = (Transition){.location_in = 1, .label_action = 0, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {infty,infty}};

    

    nb_trans_par_location[1] = 1;
    transitions[1] = malloc(nb_trans_par_location[1] * sizeof(Transition));
    transitions[1][0] = (Transition){.location_in = 0, .label_action = 1, .guard = {{0,0,-1},{infty,0,infty},{infty,infty,0}}, .reset = {0,0}};
   


    
  
    

}

// --------------------- Initialisation des variables ---------------------

void init_variables() { 
    //memset(&variable, 0, sizeof(Variable)); 
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
void init_update_functions() {
    update_functions[0] = update_a;
    update_functions[1] = update_b;
    update_functions[2] = update_c;
}

// --------------------- Contraintes ---------------------

bool const_a(Variable var) { return true; }
bool const_b(Variable var) { return true; }
bool const_c(Variable var) { return true; }

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
