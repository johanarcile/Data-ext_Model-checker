#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "structure_variable.h"

bool equal_var(Variable* v1, Variable* v2){
  if(v1->v != v2->v){
    return false;
  }
  if(v1->x != v2->x){
    return false;
  }
  for(int i = 0; i < TABLE_SIZE; i++){
    if(v1->table[i] != v2->table[i]){
      return false;
    }
  }
  for(int i = 0; i < NAME_SIZE; i++){
    if(v1->name[i] != v2->name[i]){
      return false;
    }
  }
  if(v1->active != v2->active){
    return false;
  }
  return true;
}

void print_variable(Variable* variable){
  printf("\t\t v = %d\n", variable->v);
  printf("\t\t x = %d\n", variable->x);
  printf("\t\t table :\n");
  for(int i = 0; i < TABLE_SIZE; i++){
    printf("\t\t\t table[%d] = %d\n", i, variable->table[i]);
  }
  printf("\t\t name :\n");
  for(int i = 0; i < NAME_SIZE; i++){
    printf("\t\t\t name[%d] = %c\n", i, variable->name[i]);
  }
  printf("\t\t active = %d\n", variable->active);
}
