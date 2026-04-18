#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>

// --------------------- Def de variables -------------------------

typedef struct Variable {
    int v;
} Variable;

bool equal_var(Variable* v1, Variable* v2);
void print_variable(Variable* v);

#endif
