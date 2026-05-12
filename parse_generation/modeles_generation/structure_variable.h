#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>

// --------------------- Def de variables -------------------------

structure_variable_h_define
structure_variable_h_typedef_primitive
structure_variable_h_typedef_struct

bool equal_var(Variable* v1, Variable* v2);
void print_variable(Variable* v);

#endif
