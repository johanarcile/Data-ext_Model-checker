#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>

// --------------------- Def de variables -------------------------

typedef struct Variable {
    int v;
} Variable;
// --------------Fonctions de comparaison ----------------------------
bool equal_var(Variable v1, Variable v2);
bool v1_inf_v2(Variable v1, Variable v2);
bool v1_sup_v2(Variable v1, Variable v2);
#endif
