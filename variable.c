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
  if(v1->n_size != v2->n_size){
    return false;
  }
  for(int i = 0; i < v1->n_size; i++){
    if(v1->n[i].nom_size != v2->n[i].nom_size){
      return false;
    }
    if(v1->n[i].nom_size_1 != v2->n[i].nom_size_1){
      return false;
    }
    for(int j = 0; j < v1->n[i].nom_size; j++){
      for(int k = 0; k < v1->n[i].nom_size_1; k++){
        if(v1->n[i].nom[j][k] != v2->n[i].nom[j][k]){
          return false;
        }
      }
    }
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
  printf("\t\t\t name = %s\n", variable->name);
  printf("\t\t active = %d\n", variable->active);
  printf("\t\t n_size = %d\n", variable->n_size);
  printf("\t\t n :\n");
  for(int i = 0; i < variable->n_size; i++){
    printf("\t\t\t n[%d].nom_size = %d\n", i, variable->n[i].nom_size);
    printf("\t\t\t n[%d].nom_size_1 = %d\n", i, variable->n[i].nom_size_1);
    printf("\t\t\t n[%d].nom :\n", i);
    for(int j = 0; j < variable->n[i].nom_size; j++){
      printf("\t\t\t\t n[%d].nom[%d] = %s\n", i, j, variable->n[i].nom[j]);
    }
  }
}
