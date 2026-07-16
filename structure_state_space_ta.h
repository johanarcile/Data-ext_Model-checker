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

// typedef struct {
//     State       key;
//     UT_hash_handle hh;
// } visitState;


// typedef struct StateHash {
//     State key;
//     int index;
//     UT_hash_handle hh;
// } StateHash;

typedef struct {
    Variable            key;    /* clé = state.var */
    State          state;
    int         weight;
    bool        explored;
    UT_hash_handle hh;
} StateWeightExp;

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


typedef struct {
    State * state;
    int weight;
} HeapNodeP;

typedef struct {
    HeapNodeP* data;        
    int        size;
    int        capacity;
} MinHeapP;








void explore_state_space_ta(TA* ta);
void build_state_space_ta(TA* ta, State_space_TA* state_space_ta);
void print_state_space_ta(State_space_TA* state_space_ta, char** locations, char** actions);
void print_state(State* etat, char** locations);
State* compute_init_state(TA* ta);
State* get_successors(TA* ta, State* source, int* num_successors);




/*==============================Fonction d'exploration====================================================*/

//heurstique
 int heuristique_checkp(State* s);
//check
bool check_p(State* s);


/*----------------- Partial memory (memoire que dans les borders) -----------------------------------------------------------------*/

int EF_p(TA* ta, int location, DBM clock, State** result,
          bool (*check)(State* s), int (*heuristique_check)(State* s));

int EF_p_HV(TA* ta, int location, DBM clock, State** result,
          bool (*check)(State* s),  int  (*heuristique_check)(State* s));
int EF_p_HV_M(TA* ta, int location, DBM clock, State** result,
          bool (*check)(State* s),
         int  (*heuristique_check)(State* s));

int EF_p_1table(TA* ta, int location, DBM clock, State** result,
                bool (*check)(State*s),
         int (*heuristique_check)(State*s));

int EG_p_HV_M(TA* ta, int location, DBM clock, 
           bool (*check)(State* s),
          int  (*heuristique_check)(State* s));
int EG_p_2tables(TA* ta, int location, DBM clock, 
                  bool (*check)(State* s),
                 int  (*heuristique_check)(State* s));

int EG_p_1table(TA* ta, int location, DBM clock,
                bool (*check)(State*s),
         int (*heuristique_check)(State*s));


/*-------------tests------------------------------------------------------*/

void print_all_exist(State_space_TA* ss_ta, TA* ta);


/*-------------- No memory ---------------------------------------------*/
int EF_pNO_memory(TA* ta, int location, DBM clock, State** result,
          bool (*check)(State* s),
          int  (*heuristique_check)(State* s));

int EG_p_2tablesNo_memory(TA* ta, int location, DBM clock, 
                  bool (*check)(State* s),
                 int  (*heuristique_check)(State* s));
/*--------------------Memory in layers---------------------------*/
int EF_p_Memory_in_Layer(TA* ta, int location, DBM clock, State** result,
          bool (*check)(State* s),
         int  (*heuristique_check)(State* s));

int EG_p_2tables_Memory_Layer(TA* ta, int location, DBM clock, 
                  bool (*check)(State* s),
                 int  (*heuristique_check)(State* s));

/*-------------------- Full memory ---------------------------*/
int EF_FullMemory(TA* ta, int location, DBM clock, State** result,
          bool (*check)(State* s),
         int  (*heuristique_check)(State* s));

int EG_FullMemory(TA* ta, int location, DBM clock,
         bool (*check)(State* s),
                 int  (*heuristique_check)(State* s));
/*---------------------------test fonction recursive-----------------------------*/


void  visit_add(visit** table, State s);
visit* visit_find(visit** table, State s);
void  visit_destroy(visit** table);

 void sw_add(StateWeight** table, State s, int w);
 StateWeight* sw_find(StateWeight** table, State s);
  void sw_destroy(StateWeight** table);


MinHeap* heap_create(int capacity);
void heap_destroy(MinHeap* h);
void heap_swap(MinHeap* h, int i, int j);
void heap_sift_up(MinHeap* h, int i);
void heap_sift_down(MinHeap* h, int i);
void heap_push(MinHeap* h, State s, int w);
HeapNode heap_pop(MinHeap* h);

MinHeapP* heap_createP(int capacity);
void heap_destroyP(MinHeapP* h);
void heap_swapP(MinHeapP* h, int i, int j);
void heap_sift_upP(MinHeapP* h, int i);
void heap_sift_downP(MinHeapP* h, int i);
void heap_pushP(MinHeapP* h, State* s, int w);
HeapNodeP heap_popP(MinHeapP* h);

void visitState_add(StateHash** table, State s);
StateHash* visitState_find(StateHash** table, State s);
void visitState_destroy(StateHash** table);

  void swe_add(StateWeightExp** table, State s, int w);
  StateWeightExp* swe_find(StateWeightExp** table, State s);
   void swe_destroy(StateWeightExp** table);                  
#endif
