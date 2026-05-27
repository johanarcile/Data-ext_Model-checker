#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>
#define TABLE_SIZE 3
#define NAME_SIZE 50
// --------------------- Def de variables -------------------------

typedef struct Variable {
    int v;
    int x;
    int table[TABLE_SIZE];   // tableau d'int
    int table_size;          // nombre d’éléments utilisés

    char name[NAME_SIZE];    // "string"
    bool active;             // booléen
} Variable;


// --------------Fonctions ----------------------------
bool equal_var(Variable* v1, Variable* v2);
void print_variable(Variable * v);
#endif
