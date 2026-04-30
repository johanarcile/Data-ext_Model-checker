
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "structure_variable.h"

bool equal_var(Variable* v1, Variable* v2){
  if(v1->v != v2->v){
    return false;
  }
  return true;
}

void print_variable(Variable* var){
  printf("\t v = %d\n", var->v);
}
