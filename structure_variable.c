#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include "structure_variable.h"


// --------------------- Def de variables -------------------------

// typedef struct Variable {

//     bool active;             // booléen
//     int v;
//     int x;
//     int table[TABLE_SIZE];   // tableau d'int
//     int table_size;          // nombre d’éléments utilisés

//     char name[NAME_SIZE];    // "string"
  
    

// } Variable;

// ----------------------Comparaison des variable----------------------------------
bool equal_var( Variable *v1,  Variable *v2) {

  
  
    if (v1->v != v2->v)
        {
       
            return false;
        }
      if (v1->x != v2->x)
      {
    
        return false;
      }
        

    if (v1->active != v2->active)
        {
       
        return false;
      }

    if (strcmp(v1->name, v2->name) != 0)
     {
  
        return false;
      }

    if (v1->table_size != v2->table_size)
        {
     
        return false;
      }

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (v1->table[i] != v2->table[i])

          {
     
        return false;
      }
    }

    return true;


    //return memcmp(v1, v2,sizeof(Variable)) == 0;
}

int compare_var(const Variable *v1, const Variable *v2) {

    if (v1->v < v2->v) return -1;
    if (v1->v > v2->v) return 1;
    // si ici donc v1.v == v2.V
     if (v1->x < v2->x) return -1;
    if (v1->x > v2->x) return 1;
    if (v1->active < v2->active) return -1;
    if (v1->active > v2->active) return 1;
     // si ici donc v1.v == v2.V et  v1.active = v2.active 
    int name_cmp = strcmp(v1->name, v2->name);
    if (name_cmp != 0) return name_cmp;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (v1->table[i] < v2->table[i])
            return -1;
        else if (v1->table[i] > v2->table[i]){
           return 1;
        }
    }
    return 0;
}

bool v1_inf_v2(Variable* v1, Variable* v2){
     return (compare_var(v1, v2) < 0);
}
bool v1_sup_v2(Variable* v1, Variable* v2){
    return compare_var(v1, v2)>0;
}


void print_variable(Variable * v){

        printf("\t Variables       : v = %d\n", v->v);
         printf("\t Variables       : x = %d\n", v->x);
        printf("\t Variables       : Active = %d\n", v->active);
        printf("\t Variables       : name = %s\n", v->name);
         printf("\t Variables       : table = [ %d, %d, %d,] \n", v->table[0], v->table[1],v->table[2]);
         
//          printf("offsetof active = %zu\n", offsetof(Variable, active));
// printf("offsetof x      = %zu\n", offsetof(Variable, x));
// printf("sizeof Variable = %zu\n", sizeof(Variable));
       
}



