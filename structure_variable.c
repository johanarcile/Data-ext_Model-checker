#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#define TABLE_SIZE 3
#define NAME_SIZE 50

// --------------------- Def de variables -------------------------

typedef struct Variable {
    int v;
    int table[TABLE_SIZE];   // tableau d'int
    int table_size;          // nombre d’éléments utilisés

    char name[NAME_SIZE];    // "string"
    bool active;             // booléen
    int x;
    

} Variable;

// ----------------------Comparaison des variable----------------------------------
bool equal_var(const Variable *v1, const Variable *v2) {

  
  
    // if (v1->v != v2->v)
    //     {
       
    //         return false;
    //     }
    //   if (v1->x != v2->x)
    //   {
    
    //     return false;
    //   }
        

    // if (v1->active != v2->active)
    //     {
       
    //     return false;
    //   }

    // if (strcmp(v1->name, v2->name) != 0)
    //  {
  
    //     return false;
    //   }

    // if (v1->table_size != v2->table_size)
    //     {
     
    //     return false;
    //   }

    // for (int i = 0; i < TABLE_SIZE; i++) {
    //     if (v1->table[i] != v2->table[i])

    //       {
     
    //     return false;
    //   }
    // }

    // return true;


    return memcmp(v1, v2,sizeof(Variable)) == 0;
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



#endif