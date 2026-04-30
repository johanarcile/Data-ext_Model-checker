#ifndef PARSE_H
#define PARSE_H

#include <limits.h>
#include <stdbool.h>

#define infty INT_MAX 

typedef int** DBM;
typedef int* clocks;

typedef struct Transition {
    int location_in;
    int label_action;
    DBM guard;
    clocks reset;
} Transition;

typedef struct ParseInfos {
    int nb_actions;
    char** actions;
    int nb_clocks;
    char** names_clocks;
    int nb_locations;
    char** locations;
    DBM* invariants;
    Transition*** transitions;
    int* nb_transitions_locations;
} ParseInfos;

char* read_model_json(const char* filename);
void parse_model_json(const char* json_donnees);
void fill_parseInfos_struct(ParseInfos* parseInfos);

#endif