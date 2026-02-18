#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>

// --------------------- Def de variables -------------------------

typedef struct Variable {
    int v;
} Variable;

// ----------------------Comparaison des variable----------------------------------
bool equal_var(Variable v1, Variable v2){
    if (v1.v == v2.v) return true;
    else return false;
}
bool v1_inf_v2(Variable v1, Variable v2){
    if (v1.v < v2.v) return true;
    else return false;
}
bool v1_sup_v2(Variable v1, Variable v2){
    if (v1.v > v2.v) return true;
    else return false;
}
#endif