#ifndef STRUCTURE_VAR_H_2
#define STRUCTURE_VAR_H_2

#include <stdbool.h>


// --------------------- Def de variables -------------------------

typedef struct Variable {
    int v;
    int w;
    bool active;             // booléen
  
    
} Variable;

/* ------------------------------------  */
// --------------Fonctions de comparaison ----------------------------
bool equal_var(Variable* v1, Variable* v2);
// bool v1_inf_v2(Variable* v1, Variable* v2);
// bool v1_sup_v2(Variable* v1, Variable* v2);
void print_variable(Variable * v);


bool check_p_var(Variable* V);



int heuristique_checkp_var(Variable* V);

bool check_p_var_true(Variable* V);
bool check_p_var_false(Variable* V) ;

#endif
