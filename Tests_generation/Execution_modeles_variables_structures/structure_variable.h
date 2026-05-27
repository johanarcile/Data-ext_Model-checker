#ifndef STRUCTURE_VAR_H
#define STRUCTURE_VAR_H

#include <stdbool.h>

// --------------------- Def de variables -------------------------

#define TABLE_SIZE 3
#define NAME_SIZE 50

typedef struct Name {
    int nom_size;
    int nom_size_1;
    char** nom;
} Name;

typedef struct Variable {
    int v;
    int x;
    int table[TABLE_SIZE];   // tableau d'int
    char name[NAME_SIZE];    // string
    bool active;             // booléen
    int n_size;
    Name* n;
} Variable;


bool equal_var(Variable* v1, Variable* v2);
void print_variable(Variable* v);

#endif
