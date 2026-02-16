#ifndef STRUCTURE_STATE_SPACE_TA_H
#define STRUCTURE_STATE_SPACE_TA_H

#include "structure_ta.h"
#include "uthash.h"

#define TAILLE_MAX 1000

typedef struct State {
    int location;
    DBM clock_zone;
    Variable var;
} State;

//typedef struct StateKey {
//    int location;
    //DBM clock_zone;
//    Variable var;
//} StateKey;

typedef struct StateKey { //WARNING : Whole state is key, might cause failure to find state because of "padding" ?
    State key;
} StateKey;

typedef struct StateHash {
    StateKey key;
    int index;
    UT_hash_handle hh;
} StateHash;

typedef struct State_transition {
    int cible;
    int action_id;
} State_transition;

typedef struct {
    State* etats;   //Dynamic array of States (this is hashed)
    int nb_etats;   //Total number of States added to etats
    int capacite_etats;   //Current capacity of etats (starts at 1000, doubled when reached)
    State_transition** state_transitions;   //Dynamic array of array of State_transitions (first dimension is the source State (size given by nb_etats), second dimension if the transition id in the outgoing transitions from source (size given by nb_trans_by_state))
    int* nb_trans_by_state;   //Dynamic array of int giving the number of outgoing transition of state at index
} State_space_TA;

void explore_state_space_ta(TA* ta);
void build_state_space_ta(TA* ta, State_space_TA* state_space_ta);
void print_state_space_ta(State_space_TA* state_space_ta, char** locations, char** actions);
State* compute_init_state(TA* ta);
int EF_p(TA* ta,State* init_state, int goal, bool (*check)(State* s, int goal,TA* ta));
bool check_p(State* s, int goal,TA* ta);
bool check_p_sup(State* s, int goal,TA* ta);
bool check_p_inf(State* s, int goal,TA* ta);
#endif
