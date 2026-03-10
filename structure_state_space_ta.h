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

typedef struct {
    Variable       key;
    UT_hash_handle hh;
} visit;

typedef struct {
    Variable            key;    /* clé = state.var */
    State          state;
    int         weight;
    UT_hash_handle hh;
} StateWeight;


typedef struct {
    State state;
    int weight;
} HeapNode;

typedef struct {
    HeapNode* data;
    int       size;// nbr d'elément actuel dans le heap
    int       capacity;// nbr d'element qu'on peut stocker
} MinHeap;



void explore_state_space_ta(TA* ta);
void build_state_space_ta(TA* ta, State_space_TA* state_space_ta);
void print_state_space_ta(State_space_TA* state_space_ta, char** locations, char** actions);
void print_state(State* etat, char** locations);
State* compute_init_state(TA* ta);



bool check_p(State* s, GoalCondition* goal, TA* ta);
bool check_p_sup(State* s, GoalCondition* goal,TA* ta);
bool check_p_inf(State* s, GoalCondition* goal,TA* ta);

int heuristique_checkp(State* s,GoalCondition* goal);
int heuristique_checkp_inf(State* s, GoalCondition* goal);
int heuristique_checkp_max(State* s, GoalCondition* goal);

int EF_p(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta), int (*heuristique_check)(State* s,GoalCondition* goal));

int EF_p_HV(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta), int  (*heuristique_check)(State* s, GoalCondition* goal));
int EF_p_HV_M(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int  (*heuristique_check)(State* s, GoalCondition* goal));


int EG_p_HV_M(TA* ta, int location, DBM clock, GoalCondition* goal,
          bool (*check)(State* s, GoalCondition* goal, TA* ta),
          int  (*heuristique_check)(State* s, GoalCondition* goal));
int EG_p_2tables(TA* ta, int location, DBM clock, GoalCondition* goal,
                 bool (*check)(State* s, GoalCondition* goal, TA* ta),
                 int  (*heuristique_check)(State* s, GoalCondition* goal));


void print_all_exist(State_space_TA* ss_ta, TA* ta, GoalCondition* goal);

#endif
