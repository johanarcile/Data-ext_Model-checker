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

/* ---------goal variable -----------------  */
#define CHECK_V      1   // 001
#define CHECK_ACTIVE 2   // 010
#define CHECK_NAME   4   // 100
#define CHECK_X      8   // 1000

//void fill_ta_struct(TA* ta);
typedef struct {
    int mask;      // indique quoi vérifier

    int v;
    bool active;
    char name[50];
    int x;

} GoalCondition;
/* ------------------------------------  */
// --------------Fonctions de comparaison ----------------------------
bool equal_var(Variable* v1, Variable* v2);
bool v1_inf_v2(Variable* v1, Variable* v2);
bool v1_sup_v2(Variable* v1, Variable* v2);
#endif
