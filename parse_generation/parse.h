#ifndef PARSE_H
#define PARSE_H

#include <limits.h>
#include <stdbool.h>

#define infty INT_MAX //Renaming of INT_MAX to symobolize the infinity
#define limit 1000000000 //Definition of a maximum limit for integers declarated in clocks constraints and guards

typedef int** DBM; //Definition of a matrix type for the clocks contraints and the guards
typedef int* clocks; //Definition of a type for the resets
typedef char* line; //Definition of a type to simplify the dimensions of def_variables_define and def_variables_typedef

typedef struct Transition {
    int location_in; //Index of the location_in in the array locations
    int label_action; //Index of the action in the array actions
    DBM guard; 
    clocks reset;
} Transition;

typedef struct ParseInfos {
    int nb_actions; //Number of actions
    char** actions; //Array of actions's names
    int nb_clocks; //Number of clocks
    char** names_clocks; //Array of clocks's names
    int nb_locations; //Number of locations
    char** locations; //Array of locations's names
    DBM* invariants; //Matrix of clocks constraints
    Transition*** transitions; //Array of transitions
    int* nb_transitions_locations; //Number of  going out off for every location

    int nb_define; //Number of code lines for declaration of symbolic constants
    line* def_variables_define; //Declarations lines of symbolic constants for structure_variable.h file
    int** nb_clines_typedef; //Array saving the number of code lines for each typedef
    int nb_typedef_struct; //Number of object typedef types
    int nb_typedef_primitive; //Number of primitive typedef types
    char*** label_typedef; //Array of names of each typedef
    line*** def_variables_typedef; //Declarations lines of typedef types including which of Variable
    int*** dim_elements_typedef_variables; //Array memorizing the dimension of each field of a typedef type
    int nb_clines_init_variables; //Number of code lines for init_variables function
    line* init_variables_function; //Lines of init_variables function
    int* nb_clines_updatef; //Numer of code lines for update functions
    line** update_functions; //Array of code lines of update functions for each action
    int* nb_clines_constraints; //Number of code lines for contraints functions
    line** constraints_functions; //Array of code lines of contraint functions for each action
} ParseInfos;

char* read_model_json(const char* filename);
void parse_model_json(const char* json_donnees);
void fill_parseInfos_struct(ParseInfos* parseInfos);

#endif