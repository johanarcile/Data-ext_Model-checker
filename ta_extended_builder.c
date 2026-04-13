  #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include "uthash.h"
#include "structure_DBM.h"
#include "structure_variable.h"
// --------------------- Hashed state space functions ---------------------

// Ajouter un nouvel état
int ajouter_etat(State_space_TA* ss_ta, StateHash** hash, State state) {//WARNING : the entire structure is copied when function is called, use pointer instead for performance ?

    StateKey key = {state};
    StateHash* s;

    HASH_FIND(hh, *hash, &key, sizeof(StateKey), s);
    if (s) return s->index;

    if (ss_ta->nb_etats == ss_ta->capacite_etats) {
        ss_ta->capacite_etats *= 2;
        ss_ta->etats = realloc(ss_ta->etats, ss_ta->capacite_etats * sizeof(State));
        ss_ta->state_transitions = realloc(ss_ta->state_transitions, ss_ta->capacite_etats * sizeof(State_transition*));
        ss_ta->nb_trans_by_state = realloc(ss_ta->nb_trans_by_state, ss_ta->capacite_etats * sizeof(int));
    }

    int idx = ss_ta->nb_etats;
    ss_ta->etats[idx] = state;
    ss_ta->state_transitions[idx] = NULL;
    ss_ta->nb_trans_by_state[idx] = 0;

    s = malloc(sizeof(StateHash));
    s->key = key;
    s->index = idx;
    HASH_ADD(hh, *hash, key, sizeof(StateKey), s);

    return ss_ta->nb_etats++;
}

// Création des transitions sortantes et états successeurs depuis l'état à indice i
void trans(TA* ta, State_space_TA* ss_ta, StateHash** hash, int i) {
    State source = ss_ta->etats[i];
    int location_out = source.location;
    Variable var = source.var;
    
    int n = ta->nb_trans_par_location[location_out];
    int nb_valides = 0;
    int valid_transitions[n];
    DBM new_clock_zone[n];

    // First loop: Check constraints and store valid transitions and new clock zones
    for (int k = 0; k < n; k++) {
        int action = ta->transitions[location_out][k].label_action;  //get action id
        memcpy(new_clock_zone[k], source.clock_zone, sizeof(DBM));  //copy state clock_zone
        successor_zone(new_clock_zone[k], ta->transitions[location_out][k].guard, ta->transitions[location_out][k].reset, *(ta->invariants[ta->transitions[location_out][k].location_in])); //compute successor_zone
        if (ta->constraints[action](var) && !is_empty(new_clock_zone[k])) {
            valid_transitions[nb_valides++] = k;  // Store the index of valid transitions
        }
    }
    
    if (nb_valides == 0) return;

    ss_ta->state_transitions[i] = malloc(nb_valides * sizeof(State_transition));
    ss_ta->nb_trans_by_state[i] = nb_valides;

    for (int idx = 0; idx < nb_valides; idx++) {
        int k = valid_transitions[idx];  // Retrieve the index of the valid transition
        Transition t = ta->transitions[location_out][k];
        int action = t.label_action;
    
        State new_state;
        new_state.location = t.location_in;
        memcpy(new_state.clock_zone, new_clock_zone[k], sizeof(DBM)); //WARNING : the entire DBM is copied, use pointer instead for performance ?
        new_state.var = ta->update_functions[action](var);
    
        int cible = ajouter_etat(ss_ta, hash, new_state);
        ss_ta->state_transitions[i][idx] = (State_transition){cible, action};
    }
}

// Construction du State space
void build_state_space_ta(TA* ta, State_space_TA* ss_ta) {
    ss_ta->capacite_etats = TAILLE_MAX;
    ss_ta->nb_etats = 0;

    ss_ta->etats = malloc(ss_ta->capacite_etats * sizeof(State));
    ss_ta->state_transitions = malloc(ss_ta->capacite_etats * sizeof(State_transition*));
    ss_ta->nb_trans_by_state = malloc(ss_ta->capacite_etats * sizeof(int));

    StateHash* hash = NULL;
    
    //Etat initial
    DBM clock_zone_init = {0};
    time_elapse_within_invariant(clock_zone_init,*(ta->invariants[0]));
    State init_state;
    init_state.location = 0;
    memcpy(init_state.clock_zone, clock_zone_init, sizeof(DBM));
    init_state.var = ta->variable;
    
    ajouter_etat(ss_ta, &hash, init_state);

    for (int i = 0; i < ss_ta->nb_etats; i++) {
        trans(ta, ss_ta, &hash, i);
    }

    HASH_CLEAR(hh, hash);
}

void print_state_space_ta(State_space_TA* ss_ta, char** locations, char** actions) {
    for (int i = 0; i < ss_ta->nb_etats; i++) {
        printf("Etat etendu ID #%d\n", i);
        printf("\t Location    : %s (ID %d)\n", locations[ss_ta->etats[i].location], ss_ta->etats[i].location);
        printf("\t Clock zone :\n");
        print_dbm(ss_ta->etats[i].clock_zone);
        printf("\t Variables       : v = %d\n", ss_ta->etats[i].var.v);
       printf("\t Variables       : active= %d\n", ss_ta->etats[i].var.active);
        printf("\t Variables       : x= %d\n", ss_ta->etats[i].var.x);
        // printf("\t Variables       : t[0] = %d\n", ss_ta->etats[i].var.table[0]);
        // printf("\t Variables       : t[1] = %d\n", ss_ta->etats[i].var.table[1]);
        // printf("\t Variables       : t[2] = %d\n", ss_ta->etats[i].var.table[2]);
        
        printf("\t Transitions sortantes :\n");

        for (int j = 0; j < ss_ta->nb_trans_by_state[i]; j++) {
            int id_cible = ss_ta->state_transitions[i][j].cible;
            int action_id = ss_ta->state_transitions[i][j].action_id;
            printf("\t\t   [%d] --%s--> [%d] \n", i, actions[action_id], id_cible);
        }

        printf("\n");
    }
}


// --------------------- On-the-fly functions ---------------------


//Return initial state
State* compute_init_state(TA* ta) {
    DBM clock_zone_init = {0};
    time_elapse_within_invariant(clock_zone_init,*(ta->invariants[0]));
    
    State* init_state = malloc(sizeof(State));
    init_state->location = 0;
    memcpy(init_state->clock_zone, clock_zone_init, sizeof(DBM));
    init_state->var = ta->variable;
    
    return init_state;
}

//Return successors of source as an array of State and set num_successors as the size of the array
State* get_successors(TA* ta, State* source, int* num_successors) {
  
    int location_out = source->location;
    Variable var = source->var;
    
    int n = ta->nb_trans_par_location[location_out];
    int nb_valides = 0;
    int valid_transitions[n];
    DBM new_clock_zone[n];

    // First loop: Check constraints and store valid transitions and new clock zones
    for (int k = 0; k < n; k++) {
        int action = ta->transitions[location_out][k].label_action;  //get action id
        memcpy(new_clock_zone[k], source->clock_zone, sizeof(DBM));  //copy state clock_zone
        successor_zone(new_clock_zone[k], ta->transitions[location_out][k].guard, ta->transitions[location_out][k].reset, *(ta->invariants[ta->transitions[location_out][k].location_in])); //compute successor_zone
        if (ta->constraints[action](var) && !is_empty(new_clock_zone[k])) {
            valid_transitions[nb_valides++] = k;  // Store the index of valid transitions
        }
    }

    State* successors = malloc(nb_valides * sizeof(State));
    *num_successors = nb_valides;

    for (int idx = 0; idx < nb_valides; idx++) {
        int k = valid_transitions[idx];  // Retrieve the index of the valid transition
        Transition t = ta->transitions[location_out][k];
        int action = t.label_action;
    
        State new_state;
        new_state.location = t.location_in;
        memcpy(new_state.clock_zone, new_clock_zone[k], sizeof(DBM)); //WARNING : the entire DBM is copied, use pointer instead for performance ?
        new_state.var = ta->update_functions[action](var);
        
        successors[idx] = new_state;  // Add the new state to the list of valid successors
    }
   
    return successors;

}

//Print contents of a state
void print_state(State* etat, char** locations) {
        printf("\t Location    : %s (ID %d)\n", locations[etat->location], etat->location);
        printf("\t Clock zone :\n");
        print_dbm(etat->clock_zone);
    //     printf("\t Variables       : v = %d\n", etat->var.v);
    //    printf("\t Variables       : X = %d\n", etat->var.x);

    //     printf("\t Variables       : Active = %d\n", etat->var.active);
    //     printf("\t Variables       : name = %s\n", etat->var.name);
    //      printf("\t Variables       : table = [ %d, %d, %d,] \n", etat->var.table[0], etat->var.table[1],etat->var.table[2]);
        
         print_variable(&(etat->var));

        printf("\n");
}

// Exploration on-the-fly du state space (currently up to depth 1)
void explore_state_space_ta(TA* ta) {
    State* init_state = compute_init_state(ta); //Etat initial
    print_state(init_state, ta->locations); //Print état initial
    
    int num_successors; // Store length of successors array
    State* successors = get_successors(ta, init_state, &num_successors); //Compute successors of init_state
    for (int i = 0; i < num_successors; i++) {
        print_state(&successors[i], ta->locations); //Print successor at index i
    }
        
}





/*==================properties checking functions ===============================*/



bool check_p(State* s, GoalCondition* goal, TA* ta) {

    if (goal->mask & CHECK_V) { // pour tester v il faut 001 et goal.mask !=0 
       // printf("\n testing v");
        if (s->var.v != goal->v)
            return false;
    }

    if (goal->mask & CHECK_ACTIVE) { // pour tester v il faut 010 et goal.mask !=0 
         //printf("\n checking ACTIVE");
        if (s->var.active != goal->active)
             { //printf("\n pas mmm active");
                return false;}
    }

    if (goal->mask & CHECK_X) { // pour tester v il faut 001 et goal.mask !=0 
          //printf("\n testing x");
        if (s->var.x != goal->x)
            return false;
    }

    if (goal->mask & CHECK_NAME) {// pour tester v il faut 100 et goal.mask !=0 
       // printf("\n checking Name");
        if (strcmp(s->var.name, goal->name) != 0)
            return false;
    }
    // printf("\n prop %d is checked! \n ", goal->v);
    return true;
}

bool check_p_inf(State* s, GoalCondition* goal, TA* ta) {

    if (goal->mask & CHECK_V) { // pour tester v il faut 001 et goal.mask !=0 
       // printf("\n checking v");
        if (s->var.v >= goal->v)
            return false;
    }

    if (goal->mask & CHECK_ACTIVE) { // pour tester v il faut 010 et goal.mask !=0 
       //  printf("\n checking ACTIVE");
        if (s->var.active != goal->active)
             {return false;}
    }

    if (goal->mask & CHECK_NAME) {// pour tester v il faut 100 et goal.mask !=0 
       // printf("\n checking Name");
        if (strcmp(s->var.name, goal->name) != 0)
            return false;
    }
  
    return true;
}

bool check_p_sup(State* s, GoalCondition* goal, TA* ta) {

    if (goal->mask & CHECK_V) { // pour tester v il faut 001 et goal.mask !=0 
       // printf("\n checking v");
        if (s->var.v <= goal->v)
            { return false;}
    }

    if (goal->mask & CHECK_ACTIVE) { // pour tester v il faut 010 et goal.mask !=0 
         //printf("\n checking ACTIVE");
        if (s->var.active != goal->active)
           return false;
    }

    if (goal->mask & CHECK_NAME) {// pour tester v il faut 100 et goal.mask !=0 
       // printf("\n checking Name");
        if (strcmp(s->var.name, goal->name) != 0)
            return false;
    }
    return true;
}

// bool check_p_inf(State* s, int goal,TA* ta){
//     //printf("\n i'm checking: \n");
//     //print_state(s, ta->locations);
//     if (s->var.v < goal) {
//         printf("\n value %d found\n", goal);
//         return true;
//     }
//     return false;
// }

// bool check_p_sup(State* s, int goal,TA* ta){
//     //printf("\n i'm checking: \n");
//     //print_state(s, ta->locations);
//     if (s->var.v > goal) {
//         printf("\n value %d found\n", goal);
//         return true;
//     }
//     return false;
// }
/*========================Heuristiques=============================================*/

 int heuristique_checkp(State* s,GoalCondition* goal){
    return abs(s->var.v - goal->v);
 }

 int heuristique_checkp_inf(State* s, GoalCondition* goal){
   
     return  s->var.v ;
 }

 int heuristique_checkp_max(State* s, GoalCondition* goal){
    
    printf("\n ici using heuristique_checkp_max");
    return   - (s->var.v);
 }


/*===============Exploration Alogorithms=============================================*/
/********************************************************************  No memory *******************************************************/


/********************************************************************  partial memory *******************************************************/

State* NextBorder(TA* ta, State state, int location, DBM clock,
                  GoalCondition *goal, int* num_finals, bool* found, bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0;
    int tail = 0;

    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;

    exploring[tail++] = state;

    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;
    *found = false;


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

             /*----check in BFS for EFP---------*/
        if (check(&current, goal, ta)){
             //printf("\nProperty found in NextBorder!");
             *found = true;
              State* result = malloc(sizeof(State));
             *result = current;
              free(exploring);
              free(finals);
              *num_finals = 0;
              return result;

             }


        for (int j = 0; j < num_succ; j++) {

            State* s = &succs[j];
            bool present = false;

            /* ----- Border state ----- */
            if ((s->location == location) &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))
            {
                
               
                /* vérifier doublon */
                for (int k = 0; k < *num_finals; k++) {
                    if (equal_var(&(s->var),&(finals[k].var))) {  //(s->var.v == finals[k].var.v)
                        present = true;
                        break;
                    }
                }
                // Pas necessaire car on garde tout les etats borders en memoire
                if (!present) {
                   
                    if (*num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        // printf("\n capacite augmente:\n");
                        State* tmp =
                            realloc(finals,
                                    capacity_finals * sizeof(State));// finals = realloc ()

                        if (!tmp) {
                            free(finals);
                            free(exploring);
                            free(succs);
                            return NULL;
                        }

                        finals = tmp;
                    }
                  
                    finals[*num_finals] = *s;
                    (*num_finals)++;
                }

            }
            /* ----- Continue BFS ----- */
            else {

                if (tail >= capacity) {
                    capacity *= 2;
                    
                    State* tmp =
                        realloc(exploring,
                                capacity * sizeof(State));

                    if (!tmp) {
                        free(finals);
                        free(exploring);
                        free(succs);
                        printf("\n Erreur: Memoire depasse!!");
                        return NULL;
                    }

                    exploring = tmp;
                    //free (tmp);
                  
                }
                //print_state(s, ta->locations);
                exploring[tail++] = *s;

            }
        }

        free(succs);
    }

    free(exploring);
    return finals;
}




/*---------------------------Next border pour EG ---------------------------------------------------------*/

State* EGNextBorder(TA* ta, State state, int location, DBM clock,
                  GoalCondition *goal, int* num_finals, bool* found, bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int tail = 0;
     int head = 0;
  

    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;

    exploring[tail++] = state;

    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;
    *found = false;


    /* ---------- BFS ---------- */
    while (head < tail) {   // tq y'a des etats a explorer

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

        for (int j = 0; j < num_succ; j++) { // pour chaque successeur

            State* s = &succs[j];
            bool present = false;
            if (check(s,goal, ta)){ // si il satsfat la propriete

                 /* ----- Border state ----- */
            if ((s->location == location) &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))  //si il est un etat border
            {
                
               
                /* vérifier doublon */
                for (int k = 0; k < *num_finals; k++) {
                    if (equal_var(&(s->var),&(finals[k].var))) {  //(s->var.v == finals[k].var.v)
                        present = true;
                        break;
                    }
                }

                if (!present) { // si il n'exite pas déja on l'ajoute à finals
                   
                    if (*num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        // printf("\n capacite augmente:\n");
                        State* tmp =
                            realloc(finals,
                                    capacity_finals * sizeof(State));// finals = realloc ()

                        if (!tmp) {
                            free(finals);
                            free(exploring);
                            free(succs);
                            return NULL;
                        }

                        finals = tmp;
                    }
                  
                    finals[*num_finals] = *s;
                    (*num_finals)++;
                }

            }
            /* ----- Continue BFS ----- */
            else {

                if (tail >= capacity) {
                    capacity *= 2;
                    
                    State* tmp =
                        realloc(exploring,
                                capacity * sizeof(State));

                    if (!tmp) {
                        free(finals);
                        free(exploring);
                        free(succs);
                        printf("\n Erreur: Memoire depasse!!");
                        return NULL;
                    }

                    exploring = tmp;
                    //free (tmp);
                  
                }
                //print_state(s, ta->locations);
                exploring[tail++] = *s;

            }

            }
           
        }

        free(succs);
    }

    free(exploring);
    return finals;
}





State* EFEGNextBorder(TA* ta, State state, int location, DBM clock,
                  GoalCondition *goal, int* num_finals, bool* found, bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0;
    int tail = 0;

    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;

    exploring[tail++] = state;

    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;
    *found = false;


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

             /*----check in BFS for EFP---------*/
        // if (check(&current, goal, ta)){
        //     // printf("\nProperty found in NextBorder!");
        //      *found = true;
        //       State* result = malloc(sizeof(State));
        //      *result = current;
        //       free(exploring);
        //       free(finals);
        //       *num_finals = 0;
        //       return result;

        //      }


        for (int j = 0; j < num_succ; j++) {

            State* s = &succs[j];
            bool present = false;

            /* ----- Border state ----- */
            if ((s->location == location) &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))
            {
                
               
                /* vérifier doublon */
                for (int k = 0; k < *num_finals; k++) {
                    if (equal_var(&(s->var),&(finals[k].var))) {  //(s->var.v == finals[k].var.v)
                        present = true;
                        break;
                    }
                }
                // vu que j'ai visited esq cette verification est necessaire
                if (!present) {
                   
                    if (*num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        // printf("\n capacite augmente:\n");
                        State* tmp =
                            realloc(finals,
                                    capacity_finals * sizeof(State));// finals = realloc ()

                        if (!tmp) {
                            free(finals);
                            free(exploring);
                            free(succs);
                            return NULL;
                        }

                        finals = tmp;
                    }
                  
                    finals[*num_finals] = *s;
                    (*num_finals)++;
                }

            }
            /* ----- Continue BFS ----- */
            else {

                if (tail >= capacity) {
                    capacity *= 2;
                    
                    State* tmp =
                        realloc(exploring,
                                capacity * sizeof(State));

                    if (!tmp) {
                        free(finals);
                        free(exploring);
                        free(succs);
                        printf("\n Erreur: Memoire depasse!!");
                        return NULL;
                    }

                    exploring = tmp;
                    //free (tmp);
                  
                }
                //print_state(s, ta->locations);
                exploring[tail++] = *s;

            }
        }

        free(succs);
    }

    free(exploring);
    return finals;
}






/********************************************************************  partial memory *******************************************************/

/* ==========================================================================================EF(p)====================*/


/* ================================================================== */
/*  Visited hash table (table pour enregistrer les états visités)     */
/* ================================================================== */



void visit_add(visit** table, State s) {
    
        visit* e = NULL;
      
        HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
        if (e == NULL) {
        e = malloc(sizeof(visit));
        e->key    = s.var;
        //e->explored  = true;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);}
 
}

visit* visit_find(visit** table, State s) {
    visit* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

void visit_destroy(visit** table) {
    visit *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}

/* ------------------------------------------------------------------ */
/*  Structure stateweight pour table visiting (état + weight)                          */
/* ------------------------------------------------------------------ */

static void sw_add(StateWeight** table, State s, int w) {
   
    StateWeight* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    if (e == NULL) {
        e = malloc(sizeof(StateWeight));
        e->key    = s.var;
        e->state  = s;
        e->weight = w;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);
    }
}

static StateWeight* sw_find(StateWeight** table, State s) {
    StateWeight* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

static void sw_destroy(StateWeight** table) {
    StateWeight *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}


/* ================================================================================= */
/*  Min-Heap (priority queue)   pour sauvegarder les états ordonés selon le weight   */
/* ================================================================================ */



 MinHeap* heap_create(int capacity) {
    MinHeap* h = malloc(sizeof(MinHeap));
    h->data     = malloc(sizeof(HeapNode) * capacity);
    h->size     = 0;
    h->capacity = capacity;
    return h;
}

 void heap_destroy(MinHeap* h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

 void heap_swap(MinHeap* h, int i, int j) {
    HeapNode tmp  = h->data[i];
    h->data[i]    = h->data[j];
    h->data[j]    = tmp;
}

 void heap_sift_up(MinHeap* h, int i) { // pour insérer nouveau élémnt dans sa place
    while (i > 0) { // jusqu'au premier élément
        int parent = (i - 1) / 2; // rend la partie décimal parent de 4 =>3/2=1
        if (h->data[parent].weight <= h->data[i].weight) break; // c ordonné
        heap_swap(h, parent, i);// swap si pas pas ordonné
        i = parent; // passer à l'élément au dessus
    }
}

void heap_sift_down(MinHeap* h, int i) { // pour réordonner après pop du premier élément 
    while (1) {
        int smallest = i;
        int left     = 2 * i + 1;
        int right    = 2 * i + 2;

        if (left  < h->size && h->data[left].weight  < h->data[smallest].weight) // si le fils gauche existe et est plus petit que la parent 
            smallest = left;
        if (right < h->size && h->data[right].weight < h->data[smallest].weight)
            smallest = right;

        if (smallest == i) break; // i est donc plus petit que ses deux fils donc c cordonné
        heap_swap(h, i, smallest);
        i = smallest; // continuer avec l'element prochain
    }
}

void heap_push(MinHeap* h, State s, int w) {
    /* Grow if needed */
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->data = realloc(h->data, sizeof(HeapNode) * h->capacity);
    }
    h->data[h->size].state  = s;
    h->data[h->size].weight = w;
    heap_sift_up(h, h->size); // on le place comme dernier élément et on monte
    h->size++;
}

 HeapNode heap_pop(MinHeap* h) {
    HeapNode best  = h->data[0];// on retourne le premier élément

    h->data[0] = h->data[h->size - 1];// on met le dernier element comme premier 
    h->size--;
    if (h->size > 0)
        heap_sift_down(h, 0);//réordonner
    return best;
}

/* ================================================================================= */
/*  Min-Heap avec pointeur vers le state                                            */
/* =============================================================================== */


typedef struct {
    State * state;
    int weight;
} HeapNodeP;

typedef struct {
    HeapNodeP* data;        
    int        size;
    int        capacity;
} MinHeapP;


MinHeapP* heap_createP(int capacity) {
    MinHeapP* h = malloc(sizeof(MinHeapP));
    h->data     = malloc(sizeof(HeapNodeP) * capacity);
    h->size     = 0;
    h->capacity = capacity;
    return h;
}

void heap_destroyP(MinHeapP* h) {
    for (int i = 0; i < h->size; i++) {
        free(h->data[i].state);    // libère chaque State pointé
    }
    free(h->data);
    free(h);
}

void heap_swapP(MinHeapP* h, int i, int j) {
    HeapNodeP tmp = h->data[i];    //  FIX #2 : était HeapNode, doit être HeapNodeP
    h->data[i]    = h->data[j];
    h->data[j]    = tmp;
}

void heap_sift_upP(MinHeapP* h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].weight <= h->data[i].weight) break;
        heap_swapP(h, parent, i);
        i = parent;
    }
}

void heap_sift_downP(MinHeapP* h, int i) {
    while (1) {
        int smallest = i;
        int left     = 2 * i + 1;
        int right    = 2 * i + 2;

        if (left  < h->size && h->data[left].weight  < h->data[smallest].weight)
            smallest = left;
        if (right < h->size && h->data[right].weight < h->data[smallest].weight)
            smallest = right;

        if (smallest == i) break;
        heap_swapP(h, i, smallest);
        i = smallest;
    }
}

void heap_pushP(MinHeapP* h, State* s, int w) {
     if (s == NULL) {
        printf(" heap_pushP : state NULL poussé !\n");
        return;
    }
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->data = realloc(h->data, sizeof(HeapNodeP) * h->capacity); 
    }
    h->data[h->size].state  = s;   
    h->data[h->size].weight = w;
    h->size++;
    heap_sift_upP(h, h->size-1);
    
}

HeapNodeP heap_popP(MinHeapP* h) { 
    HeapNodeP best = h->data[0];    
    h->data[0] = h->data[h->size - 1];
    h->size--;
    if (h->size > 0)
        heap_sift_downP(h, 0);
    
    return best;
}

 /*==========================================   2 tabeles de hashage   ===================*/

int EF_p(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int (*heuristique_check)(State* s, GoalCondition* goal)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    //if (!init_state) return 0;
    *result= NULL;
   if (check(init_state, goal, ta)) {  
                        *result = init_state; 
                        return 1;
                    }
    StateWeight* visiting = NULL;  /* frontier  */
    visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state, goal);
    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);
   nbr_border_state ++;

    //free(init_state);

    while (HASH_COUNT(visiting) > 0) {

        /* --- Extract state with lowest weight --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

        /* --- Compute successors --- */
        int    num_succ  = 0;
        State* successors = NextBorder(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
        if (found) {
              *result = malloc(sizeof(State));
              **result = *successors;
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            return 1;
        }

        if (!successors) continue;

        bool boucle = (num_succ == 1) &&
                       equal_var(&current.var, &successors[0].var);
        if (!boucle) {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
               
                /* Skip if already seen */
                if (visit_find(&visited, *s) != NULL)
                    { 
                        continue;}

                if (check(s, goal, ta)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    sw_destroy(&visiting);
                    visit_destroy(&visited);
                    return 1;
                }



                int w = heuristique_check(s, goal);
                sw_add(&visiting, *s, w);
                visit_add(&visited,  *s);  /* mark as seen immediately */
                 nbr_border_state ++;

               
            }
        }

        free(successors);
    }

    sw_destroy(&visiting);
    visit_destroy(&visited);
    printf("\n nombre d'états dans les borders: %d",nbr_border_state );

    return 0;
}



                                               
/* ============================ EF_p avec min-heap ====================================== */

int EF_p_HV(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int  (*heuristique_check)(State* s, GoalCondition* goal)) 
{

    //if (!ta) return 0;
    int nbr_border_state =0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
   // if (!init_state) return 0;//Vérifiecation
   *result= NULL;
   if (check(init_state, goal, ta)) { 
                       *result= init_state;  
                        return 1;
                    }
    MinHeap*     heap    = heap_create(64);
    visit* visited = NULL;

    int init_weight = heuristique_check(init_state, goal);
    heap_push(heap, *init_state, init_weight);
    visit_add(&visited, *init_state);                
    nbr_border_state ++;

   // free(init_state);
    int    num_succ  = 0;
    HeapNode best; 
    State    current;
    while (heap->size > 0) {// tq y'a encore des éléments 

        /* O(log n) extraction of best state */
       best    = heap_pop(heap);
       current = best.state;

        /* Compute successors */
       
        State* successors = NextBorder(ta, current, location, clock,
                                       goal, &num_succ, &found, check);

        if (found) {
            *result = malloc(sizeof(State));
            **result = *successors;
            free(successors);
            heap_destroy(heap);
            visit_destroy(&visited);
            return 1;
        }

        if (!successors) continue;

        bool boucle = (num_succ == 1) &&
                       equal_var(&current.var, &successors[0].var);

        if (!boucle) {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];

                if (visit_find(&visited, *s) != NULL)
                    continue;

                if (check(s, goal, ta)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    heap_destroy(heap);
                    visit_destroy(&visited);
                    return 1;
                }

                int w = heuristique_check(s, goal);
                heap_push(heap, *s, w);
                visit_add(&visited, *s);
                nbr_border_state++;
            }
        }

        free(successors);
    }

    heap_destroy(heap);
    visit_destroy(&visited);
    printf("\n nombre d'états dans les borders: %d",nbr_border_state );
    return 0;
}

/*============================EFP miheap state avec pointeur===================================*/




int EF_p_HV_M(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int  (*heuristique_check)(State* s, GoalCondition* goal))
{
    bool found = false;
    *result= NULL;
    State* init_state = compute_init_state(ta);
      if (check(init_state, goal, ta)) { 
                        *result = init_state;  
                        return 1;
                    }

    MinHeapP* heap = heap_createP(64);  
    visit* visited = NULL;

    int init_weight = heuristique_check(init_state, goal);

    heap_pushP(heap, init_state, init_weight);
    visit_add(&visited, *init_state);

     int num_succ = 0;
    while (heap->size > 0) {

        HeapNodeP best = heap_popP(heap);  
        State* current = best.state;

     
        State* successors = NextBorder(ta, *current, location, clock,
                                       goal, &num_succ, &found, check);

        if (found) {
            *result = malloc(sizeof(State));
            **result = *successors;
            free(current);
            free(successors);
            heap_destroyP(heap);
            visit_destroy(&visited);
            return 1;
        }

        if (successors) {

            bool boucle = (num_succ == 1) &&
                          equal_var(&current->var, &successors[0].var);

            if (!boucle) {

                for (int i = 0; i < num_succ; i++) {

                    State* temp = &successors[i];

                    if (visit_find(&visited, *temp) != NULL)
                        continue;

                    if (check(temp, goal, ta)) {
                         *result = malloc(sizeof(State));
                        **result = *temp;
                        free(current);
                        free(successors);
                        heap_destroyP(heap);
                        visit_destroy(&visited);
                        return 1;
                    }

                    State* new_state = malloc(sizeof(State));
                    if (!new_state) {
                        perror("malloc failed");
                        exit(EXIT_FAILURE);
                    }

                    *new_state = *temp;

                    int w = heuristique_check(new_state, goal);

                    heap_pushP(heap, new_state, w);
                    visit_add(&visited, *new_state);
                }
            }

            free(successors);
        }

        free(current);
    }

    heap_destroyP(heap);
    visit_destroy(&visited);
    return 0;
}


/* ==========================================================================================EG(p)====================*/

/*============================EGP miheap state avec pointeur===================================*/

int EG_p_HV_M(TA* ta, int location, DBM clock, GoalCondition* goal,
          bool (*check)(State* s, GoalCondition* goal, TA* ta),
          int  (*heuristique_check)(State* s, GoalCondition* goal))
{
    State* init_state = compute_init_state(ta);
    //State * last;
    /* L'état initial doit satisfaire la propriété */
    if (!check(init_state, goal, ta)) {
        free(init_state);
        return 0;
    }

    MinHeapP* heap   = heap_createP(64);
    visit*   visited = NULL;

    int init_weight = heuristique_check(init_state, goal);
    heap_pushP(heap, init_state, init_weight);
    visit_add(&visited, *init_state);
     int  num_succ = 0;
    while (heap->size > 0) {

        HeapNodeP best    = heap_popP(heap);
        State*    current = best.state;

        bool found = false;
        
        State* successors = EGNextBorder(ta, *current, location, clock,
                                       goal, &num_succ, &found, check);
        //printf("\n Num successors : %d", num_succ);

        

        /* Cas 2 : boucle sur soi-même → chemin infini où check est vrai */
         bool boucle = (num_succ == 1) &&
                      equal_var(&current->var, &successors[0].var);

        if (boucle) { 

            free(current);
            free(successors);
            heap_destroyP(heap);
            visit_destroy(&visited);
            //printf("\n boucle");
            return 1;
        }

      


        for (int i = 0; i < num_succ; i++) {

            State* temp = &successors[i];

            /* EG : inutile d'explorer un état qui viole la propriété */
            // if (!check(temp, goal, ta))
            //     continue;
             
         
            /* Déjà visité */
            if (visit_find(&visited, *temp) != NULL)
                continue;

            State* new_state = malloc(sizeof(State)); //Allouer espace dans la memoire dés qu'ontrouve un nouveau état
            if (!new_state) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
            *new_state = *temp;
             //print_state(new_state,ta->locations);
            int w = heuristique_check(new_state, goal);
            heap_pushP(heap, new_state, w);
            visit_add(&visited, *new_state);
        }

        free(successors);
        free(current);
    }

    /* Heap vide : aucun chemin infini satisfaisant trouvé */
    heap_destroyP(heap);
    visit_destroy(&visited);
    return 0;
    
}

 /*==========================================  EG(p) avec 2 tabeles de hashage   ===================*/

int EG_p_2tables(TA* ta, int location, DBM clock, GoalCondition* goal,
                 bool (*check)(State* s, GoalCondition* goal, TA* ta),
                 int  (*heuristique_check)(State* s, GoalCondition* goal))
{

    State* init_state = compute_init_state(ta);


    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init_state, goal, ta)) {
        free(init_state);
        printf("propriete non verifier dans init state");
        return 0;
    }

    int    init_weight = heuristique_check(init_state, goal);

    StateWeight* visiting   = NULL;   /* à explorer  (frontier) */
    visit*       visited = NULL;   /* déjà expansés           */

    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);

    free(init_state);
     int    num_succ = 0;
    while (HASH_COUNT(visiting) > 0) {

        /* --- Extraire le meilleur état de visiting --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

       

        /* --- Calculer les successeurs --- */
        bool   found    = false;
      
        State* successors = EGNextBorder(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
       
        

        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);


        if (boucle) {
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            // printf("\n boucle");
            return 1;
        }
      
        /* Cas général : on n'ajoute que les successeurs qui satisfont check */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
           

            /* EG : inutile d'explorer un état qui viole la propriété */
            // if (!check(s, goal, ta))
            //     continue;

           
           
            /* un successor déja visité */
            if (visit_find(&visited, *s) != NULL)
                  continue;
          
           

            int w = heuristique_check(s, goal);
           
            sw_add(&visiting, *s, w);
            visit_add(&visited, *s);
            
        }

        free(successors);
    }

    /* open vide : aucun chemin infini satisfaisant trouvé */
    sw_destroy(&visiting);
    visit_destroy(&visited);

     return 0 ;
}


 /*==========================================  Fonction pour test   ===================*/

void print_all_exist(State_space_TA* ss_ta, TA* ta, GoalCondition* goal) {
    printf("\n les etats satisfaisants g:\n");
    for (int i = 0; i < ss_ta->nb_etats; i++) {
        if(check_p(&(ss_ta->etats[i]), goal, ta)){
               print_state(&(ss_ta->etats[i]),ta->locations);
                printf("Etat etendu ID #%d\n", i);
        }
       // printf("\n");
    }

}

/*========================= Requetes imbriquees========================================================================================================*/

 /*==========================================  EFEG(p) avec 2 tabeles de hashage   ===================*/

int EGEF_p_2tables(TA* ta, int location, DBM clock, GoalCondition* goal,
                 bool (*check)(State* s, GoalCondition* goal, TA* ta),
                 int  (*heuristique_check)(State* s, GoalCondition* goal))
{

    State* init_state = compute_init_state(ta);


    int    init_weight = heuristique_check(init_state, goal);

    StateWeight* visiting   = NULL;   /* à explorer  (frontier) */
    visit*       visited = NULL;   /* déjà explorés          */

    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);

    free(init_state);
     int    num_succ = 0;
    while (HASH_COUNT(visiting) > 0) {

        /* --- Extraire le meilleur état de visiting --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

       

        /* --- Calculer les successeurs --- */
        bool   found    = false;
       // Next border de EF sans le check dedans
        State* successors = EFEGNextBorder(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
       
        
        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);


        if (boucle) {  

            if  (check(&current, goal, ta)){ // si la propriete est verifié

                free(successors);
                sw_destroy(&visiting);
                visit_destroy(&visited);
              //  printf("\n boucle");
                return 1;

            }
            
        }
      
        /* Cas général : on n'ajoute que les successeurs qui satisfont check */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
           

            /* EG : inutile d'explorer un état qui viole la propriété */
            // if (!check(s, goal, ta))
            //     continue;

           
           
            /* un successor déja visité */
            if (visit_find(&visited, *s) != NULL)
            continue;
           

            int w = heuristique_check(s, goal);
            visit_add(&visited, *s);
            sw_add(&visiting, *s, w);
            
        }

        free(successors);
    }


    sw_destroy(&visiting);
    visit_destroy(&visited);

     return 0 ;
}


/* ================Test de EF(P1 && EF (......&&EF(Pn)))======================================*/

State* EFPnNextBorder(TA* ta, State state, int location, DBM clock,
                      GoalCondition* goal, int nbr_prop,
                      int* num_finals, int current_prop,
                      int** border_props_out,
                      bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    int capacity = 32, head = 0, tail = 0;
    State* exploring       = malloc(capacity * sizeof(State));
    int*   exploring_props = malloc(capacity * sizeof(int));   /* parallèle à exploring */
    State* finals          = malloc(capacity * sizeof(State));
    int*   finals_props    = malloc(capacity * sizeof(int));   /* parallèle à finals */
    *num_finals = 0;

    if (!exploring || !exploring_props || !finals || !finals_props) {
        free(exploring); free(exploring_props);
        free(finals);    free(finals_props);
        printf("\n Erreur malloc!!!");
        return NULL;
    }
    
    /* Prop de l'état de départ */
    int start_prop = current_prop;
   
    while (start_prop < nbr_prop && check(&state, &goal[start_prop], ta))
        start_prop++;

    exploring[tail]       = state;
    exploring_props[tail] = start_prop;
    tail++;

    while (head < tail) {
        State current  = exploring[head];
        int   cur_prop = exploring_props[head];
        head++;

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

        for (int j = 0; j < num_succ; j++) {
            State* s = &succs[j];
            /* Avancer le prop avec ce successeur */
            int s_prop = cur_prop;
            while (s_prop < nbr_prop && check(s, &goal[s_prop], ta))
                s_prop++;
            
            /* ----- Border state ----- */
            if (s->location == location &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))
            {
                /* Doublon : garder le meilleur prop */
                bool present = false;
                for (int k = 0; k < *num_finals; k++) {
                    if (equal_var(&s->var, &finals[k].var)) {
                        if (s_prop > finals_props[k])
                            finals_props[k] = s_prop;
                        present = true;
                        break;
                    }
                }
                if (present) continue;

                if (*num_finals >= capacity) {
                    capacity *= 2;
                    State* tmp_f = realloc(finals,       capacity * sizeof(State));
                    int*   tmp_p = realloc(finals_props, capacity * sizeof(int));
                    if (!tmp_f || !tmp_p) {
                        free(finals); free(finals_props);
                        free(exploring); free(exploring_props);
                        free(succs);
                        return NULL;
                    }
                    finals       = tmp_f;
                    finals_props = tmp_p;
                }

                finals[*num_finals]       = *s;
                finals_props[*num_finals] = s_prop;
                (*num_finals)++;
            }
            /* ----- Continue BFS ----- */
            else {
                if (tail >= capacity) {
                    capacity *= 2;
                    State* tmp_e = realloc(exploring,       capacity * sizeof(State));
                    int*   tmp_p = realloc(exploring_props, capacity * sizeof(int));
                    if (!tmp_e || !tmp_p) {
                        free(finals); free(finals_props);
                        free(exploring); free(exploring_props);
                        free(succs);
                        printf("\nErreur: Memoire depasse!!");
                        return NULL;
                    }
                    exploring       = tmp_e;
                    exploring_props = tmp_p;
                }
                exploring[tail]       = *s;
                exploring_props[tail] = s_prop;
                tail++;
            }
        }
        free(succs);
    }

    free(exploring);
    free(exploring_props);
    *border_props_out = finals_props;
    
    return finals;
}

typedef struct {
    Variable       key;
   // State          state;
    int            mark;
    UT_hash_handle hh;
} mark;

static void mark_add(mark** table, State s, int w) {
    mark* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    if (e == NULL) {
        e = malloc(sizeof(mark));
        e->key   = s.var;
       // e->state = s;
        e->mark  = w;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);
    }
}

static mark* mark_find(mark** table, State s) {
    mark* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

static void mark_destroy(mark** table) {
    mark *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}


int EFEF_pn_2tables(TA* ta, int location, DBM clock, GoalCondition* goal, int nbr_prop,
                    bool (*check)(State* s, GoalCondition* goal, TA* ta),
                    int  (*heuristique_check)(State* s, GoalCondition* goal))
{
    State* init_state = compute_init_state(ta);

    /* Calculer jusqu'à quelle propriété l'état initial satisfait goal[0..nbr_prop-1] */
    int init_prop = 0;
    while (init_prop < nbr_prop && check(init_state, &goal[init_prop], ta))
        init_prop++;
// si init satisfais pO et pas p1 :  init_prop = 1, si elle ne satisfait pas p0 : init prop = 0
    if (init_prop == nbr_prop) {
        printf("\n Toutes les proprietes sont verifiees ! \n");
        free(init_state);
        return 1;
    }

    StateWeight* visiting = NULL;
    mark*        marked   = NULL;

    int init_weight = heuristique_check(init_state, &goal[init_prop]);
    sw_add(&visiting, *init_state, init_weight);
    mark_add(&marked, *init_state, init_prop); // la prochaine propriete a tester pour les successor
    free(init_state);

    int num_succ = 0;

    while (HASH_COUNT(visiting) > 0) {
       //  printf ("\n element de visiting : %d",HASH_COUNT(visiting));
        /* --- 1. Trouver le mark maximal parmi les états dans visiting --- */
        StateWeight *best = NULL, *cur, *tmp;
        int best_mark = -1;

        HASH_ITER(hh, visiting, cur, tmp) {
            mark* m = mark_find(&marked, cur->state);
            if (m && m->mark > best_mark)
                best_mark = m->mark;
        }

        /* --- 2. Parmi les états avec ce mark max, prendre le plus petit weight --- */
        HASH_ITER(hh, visiting, cur, tmp) {
            mark* m = mark_find(&marked, cur->state);
            if (m && m->mark == best_mark) {
                if (best == NULL || cur->weight < best->weight)
                    best = cur;
            }
        }

        State current     = best->state;
        int   current_prop = best_mark;
        HASH_DEL(visiting, best);
        free(best);

        /* --- Appel NextBorder --- */
        int*   border_props = NULL;
        State* successors   = EFPnNextBorder(ta, current, location, clock,
                                             goal, nbr_prop, &num_succ,
                                             current_prop, &border_props, check);


        /* --- Cas self-loop --- */
        bool boucle = (num_succ == 1) &&
                      equal_var(&current.var, &successors[0].var);

        // if (boucle) {
        //     int prop = border_props[0];
        //     if (prop == nbr_prop) {
        //         printf("\n Toutes les proprietes sont verifiees! \n");
        //         free(successors); free(border_props);
        //         sw_destroy(&visiting);
        //         mark_destroy(&marked);
        //         return 1;
        //     }
        //     free(successors); free(border_props);
        //     continue;
        // }

        /* --- Cas général --- */
        for (int i = 0; i < num_succ; i++) {
            State* s    = &successors[i];
            int    prop = border_props[i];

            if (prop == nbr_prop) {
                printf("\nToutes les proprietes sont verifiees!\n");
                free(successors); free(border_props);
                sw_destroy(&visiting);
                mark_destroy(&marked);
                return 1;
            }

            /* Ne pas re-ajouter un état déjà marqué avec un prop >= */
            mark* existing = mark_find(&marked, *s);
            if (existing != NULL && existing->mark >= prop)
                continue;

            mark_add(&marked, *s, prop);
            int w = heuristique_check(s, &goal[prop]);
            sw_add(&visiting, *s, w);
        }

        free(successors);
        free(border_props);
    }

    sw_destroy(&visiting);
    mark_destroy(&marked);
    return 0;
}

/*=====================EF(p1 && EG (p2 && EG (p3 &&  EG (..... && pn)) )==================================*/
State* EFEG_pnNextBorder(TA* ta, State state, int location, DBM clock,
                         GoalCondition* goal, int nbr_prop,
                         int* num_finals, int current_prop,
                         int** border_props_out,
                         bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    int capacity = 32, head = 0, tail = 0;

    State* exploring      = malloc(capacity * sizeof(State));
    int*   exploring_props = malloc(capacity * sizeof(int));
    State* finals         = malloc(capacity * sizeof(State));
    int*   finals_props   = malloc(capacity * sizeof(int));
    *num_finals = 0;

    if (!exploring || !exploring_props || !finals || !finals_props) {
        free(exploring); free(exploring_props);
        free(finals);    free(finals_props);
        printf("\n Erreur malloc!!!");
        return NULL;
    }

    /* --- Push initial (un seul tail++) --- */
    int s_prop = current_prop;

    if (check(&state, &goal[0], ta))
        s_prop = 0;
    else if ((current_prop == 0 || current_prop == 1) && check(&state, &goal[1], ta))
        s_prop = 1;

    exploring[tail]       = state;
    exploring_props[tail] = s_prop;
    tail++;

    while (head < tail) {

        /* --- Dépiler : lire état ET prop au même index, puis head++ une seule fois --- */
        State current  = exploring[head];
        int   cur_prop = exploring_props[head];
        head++;

        /* Debug : utiliser cur_prop, PAS exploring_props[head] --- */
        // if (cur_prop == 0) printf("\n here prop 0");
        // if (cur_prop == 1) printf("\n here prop 1");

        int    num_succ = 0;
        State* succs    = get_successors(ta, &current, &num_succ);

        for (int j = 0; j < num_succ; j++) {
            State* s = &succs[j];

            /* Calculer s_prop depuis cur_prop, jamais depuis exploring_props[head] */
            if (check(s, &goal[0], ta))
                s_prop = 0;
            else if ((cur_prop == 0 || cur_prop == 1) && check(s, &goal[1], ta))
                s_prop = 1;
            else
                s_prop = 2;

            /* ----- Border state ----- */
            if (s->location == location &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))
            {
                bool present = false;
                for (int k = 0; k < *num_finals; k++) {
                    if (equal_var(&s->var, &finals[k].var)
                     && s->location == finals[k].location)
                    {
                        if (s_prop < finals_props[k])
                            finals_props[k] = s_prop;
                        present = true;
                        break;
                    }
                }
                if (present) continue;

                if (*num_finals >= capacity) {
                    capacity *= 2;
                    State* tmp_f = realloc(finals,       capacity * sizeof(State));
                    int*   tmp_p = realloc(finals_props, capacity * sizeof(int));
                    if (!tmp_f || !tmp_p) {
                        free(finals); free(finals_props);
                        free(exploring); free(exploring_props);
                        free(succs);
                        return NULL;
                    }
                    finals       = tmp_f;
                    finals_props = tmp_p;
                }

                finals[*num_finals]       = *s;
                finals_props[*num_finals] = s_prop;
                (*num_finals)++;
            }
            /* ----- Continue BFS (un seul tail++) ----- */
            else {
                if (tail >= capacity) {
                    capacity *= 2;
                    State* tmp_e  = realloc(exploring,       capacity * sizeof(State));
                    int*   tmp_ep = realloc(exploring_props, capacity * sizeof(int));
                    if (!tmp_e || !tmp_ep) {
                        free(finals); free(finals_props);
                        free(exploring); free(exploring_props);
                        free(succs);
                        return NULL;
                    }
                    exploring       = tmp_e;
                    exploring_props = tmp_ep;
                }

                exploring[tail]       = *s;
                exploring_props[tail] = s_prop;
                tail++;                          /* un seul incrément */
            }
        }
        free(succs);
    }

    free(exploring);
    free(exploring_props);
    *border_props_out = finals_props;
    return finals;
}

int EFEG_pn(TA* ta, int location, DBM clock, GoalCondition* goal, 
            bool (*check)(State* s, GoalCondition* goal, TA* ta),
            int  (*heuristique_check)(State* s, GoalCondition* goal))
{
    int  nbr_border_state = 0;
 

    State* init_state = compute_init_state(ta);
    int current_prop = 2;
    if (check(init_state, &goal[0], ta)) {

    current_prop = 0;
    }

    StateWeight* visiting = NULL;
    mark*        marked   = NULL;
    

    int init_weight = heuristique_check(init_state, &goal[0]);
    sw_add(&visiting, *init_state, init_weight);
    mark_add(&marked, *init_state, current_prop);   // &marked ici aussi
    nbr_border_state++;
    free(init_state);

    while (HASH_COUNT(visiting) > 0) {

        /* Extraire le meilleur état */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        /* Extraire la propriete qu'il satisfait*/
        mark* m = mark_find(&marked, best->state);
        current_prop =  m->mark;

        HASH_DEL(visiting, best);
        free(best);

        int*   border_props = NULL;
        int    num_succ     = 0;
        State* successors   = EFEG_pnNextBorder(ta, current, location, clock,
                                                goal, 2, &num_succ,
                                                current_prop, &border_props, check);
        if (!successors) continue;

        bool boucle = (num_succ == 1)
                   && equal_var(&current.var, &successors[0].var);
                  

        if (boucle) {
            if ( current_prop == 0 ||  current_prop == 1) {
               // printf("\n boucle");
                //printf("\n current_prop: %d", current_prop);
                free(border_props);
                free(successors);
                sw_destroy(&visiting);
                mark_destroy(&marked);
                return 1;
            }
            free(border_props);   
            free(successors);
            continue;
        }

        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];

           mark* existing = mark_find(&marked, *s);
            if (existing != NULL) {
                if (existing->mark == 0) continue;          // déjà au mieux
                if (border_props[i] < existing->mark)
                    existing->mark = border_props[i];        // mise à jour
                // re-ajouter dans visiting avec nouveau poids
                int w = heuristique_check(s, &goal[border_props[i]]);
                sw_add(&visiting, *s, w);
                continue;
            }

            // Nouvel état
            mark_add(&marked, *s, border_props[i]);
            int w = heuristique_check(s, &goal[border_props[i] < 2 ? border_props[i] : 0]);
            sw_add(&visiting, *s, w);
            nbr_border_state++;
                    }

        free(border_props);   
        free(successors);
    }

    sw_destroy(&visiting);
    mark_destroy(&marked);
    printf("\n nombre d'états dans les borders: %d", nbr_border_state);
    return 0;
}




/* Retourne un tableau [2] de GoalCondition :
 *   result[0] = props[0] AND props[1] AND ... AND props[nbr_prop-1]
 *   result[1] = props[1] AND props[2] AND ... AND props[nbr_prop-1]
 * Retourne NULL si contradiction détectée sur une même variable.
 */
GoalCondition* build_EFEG_goals(GoalCondition* props, int nbr_prop) {
    if (nbr_prop < 1) return NULL;

    GoalCondition* result = calloc(2, sizeof(GoalCondition));
    if (!result) return NULL;

    for (int g = 0; g < 2; g++) {
        int start = g ;
        result[g].mask = 0;

        for (int i = start; i < nbr_prop; i++) {
            GoalCondition* p = &props[i];

            /* --- CHECK_V --- */
            if (p->mask & CHECK_V) {
                if (result[g].mask & CHECK_V) {
                    if (result[g].v != p->v) {
                        printf("[CONTRADICTION] v=%d vs v=%d entre prop 0..%d et prop %d\n",
                               result[g].v, p->v, i-1, i);
                        free(result);
                        return NULL;
                    }
                } else {
                    result[g].mask |= CHECK_V;
                    result[g].v = p->v;
                }
            }

            /* --- CHECK_ACTIVE --- */
            if (p->mask & CHECK_ACTIVE) {
                if (result[g].mask & CHECK_ACTIVE) {
                    if (result[g].active != p->active) {
                        printf("[CONTRADICTION] active=%d vs active=%d entre prop 0..%d et prop %d\n",
                               result[g].active, p->active, i-1, i);
                        free(result);
                        return NULL;
                    }
                } else {
                    result[g].mask |= CHECK_ACTIVE;
                    result[g].active = p->active;
                }
            }

            /* --- CHECK_X --- */
            if (p->mask & CHECK_X) {
                if (result[g].mask & CHECK_X) {
                    if (result[g].x != p->x) {
                        printf("[CONTRADICTION] x=%d vs x=%d entre prop 0..%d et prop %d\n",
                               result[g].x, p->x, i-1, i);
                        free(result);
                        return NULL;
                    }
                } else {
                    result[g].mask |= CHECK_X;
                    result[g].x = p->x;
                }
            }

            /* --- CHECK_NAME --- */
            if (p->mask & CHECK_NAME) {
                if (result[g].mask & CHECK_NAME) {
                    if (strcmp(result[g].name, p->name) != 0) {
                        printf("[CONTRADICTION] name='%s' vs name='%s' entre prop 0..%d et prop %d\n",
                               result[g].name, p->name, i-1, i);
                        free(result);
                        return NULL;
                    }
                } else {
                    result[g].mask |= CHECK_NAME;
                    strcpy(result[g].name, p->name);
                }
            }
        }
    }

    return result;
}

/************************************Memory inside next border ************************************************************************* */

void visitState_add(visitState** table, State s) {
    visitState* entry = malloc(sizeof(visitState));
    entry->key = s;
    HASH_ADD_KEYPTR(hh, *table, &entry->key, sizeof(State), entry);
}

visitState* visitState_find(visitState** table, State s) {
    visitState* entry = NULL;
    HASH_FIND(hh, *table, &s, sizeof(State), entry);
    return entry;
}

void visitState_destroy(visitState** table) {
    visitState *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}

State* NextBorderMemory(TA* ta, State state, int location, DBM clock,
                  GoalCondition *goal, int* num_finals, bool* found, bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0;
    int tail = 0;

    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;
    visitState* visited  = NULL;  /* seen set  */
    exploring[tail++] = state;
    visitState_add(&visited, state);
    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;
    *found = false;


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

             /*----check in BFS for EFP---------*/
        if (check(&current, goal, ta)){
             printf("\nProperty found in NextBorder!");
             *found = true;
              State* result = malloc(sizeof(State));
             *result = current;
              free(exploring);
              free(finals);
              *num_finals = 0;
              return result;

             }

      
        for (int j = 0; j < num_succ; j++) {

            State* s = &succs[j];
            if (visitState_find (&visited, *s)) continue;
            else{
                //bool present = false;

                /* ----- Border state ----- */
                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
                {
                    
                
                    /* vérifier doublon */
                    // for (int k = 0; k < *num_finals; k++) {
                    //     if (equal_var(&(s->var),&(finals[k].var))) {  //(s->var.v == finals[k].var.v)
                    //         present = true;
                    //         break;
                    //     }
                    // }
                    // Pas necessaire car on garde tout les etats borders en memoire
                    
                    
                        if (*num_finals >= capacity_finals) {
                            capacity_finals *= 2;
                            // printf("\n capacite augmente:\n");
                            State* tmp =
                                realloc(finals,
                                        capacity_finals * sizeof(State));// finals = realloc ()

                            if (!tmp) {
                                free(finals);
                                free(exploring);
                                free(succs);
                                return NULL;
                            }

                            finals = tmp;
                        }
                    
                        finals[*num_finals] = *s;
                        (*num_finals)++;
                     
                    

                }
                /* ----- Continue BFS ----- */
                else {

                    if (tail >= capacity) {
                        capacity *= 2;
                        
                        State* tmp =
                            realloc(exploring,
                                    capacity * sizeof(State));

                        if (!tmp) {
                            free(finals);
                            free(exploring);
                            free(succs);
                            printf("\n Erreur: Memoire depasse!!");
                            return NULL;
                        }

                        exploring = tmp;
                        //free (tmp);
                    
                    }
                    //print_state(s, ta->locations);
                    exploring[tail++] = *s;
                   
                }
                visitState_add(&visited, *s);
                
        }
        }

        free(succs);
    }

    free(exploring);
    free(visited);
    return finals;
}

int EF_p_Memory_in_Layer(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int (*heuristique_check)(State* s, GoalCondition* goal)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    //if (!init_state) return 0;
    *result= NULL;
   if (check(init_state, goal, ta)) {  
                        *result = init_state; 
                        return 1;
                    }
    StateWeight* visiting = NULL;  /* frontier  */
    visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state, goal);
    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);
   nbr_border_state ++;

    //free(init_state);

    while (HASH_COUNT(visiting) > 0) {

        /* --- Extract state with lowest weight --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

        /* --- Compute successors --- */
        int    num_succ  = 0;
        State* successors = NextBorderMemory(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
        if (found) {
              *result = malloc(sizeof(State));
              **result = *successors;
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            return 1;
        }

        if (!successors) continue;

        bool boucle = (num_succ == 1) &&
                       equal_var(&current.var, &successors[0].var);
        if (!boucle) {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
               
                /* Skip if already seen */
                if (visit_find(&visited, *s) != NULL)
                    { 
                        continue;}

                if (check(s, goal, ta)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    sw_destroy(&visiting);
                    visit_destroy(&visited);
                    return 1;
                }



                int w = heuristique_check(s, goal);
                sw_add(&visiting, *s, w);
                visit_add(&visited,  *s);  /* mark as seen immediately */
                 nbr_border_state ++;

               
            }
        }

        free(successors);
    }

    sw_destroy(&visiting);
    visit_destroy(&visited);
    printf("\n nombre d'états dans les borders: %d",nbr_border_state );

    return 0;
}
/*--------------------EG(p)------------------------------------------------*/
State* NextBorderMemoryEG(TA* ta, State state, int location, DBM clock,
                  GoalCondition *goal, int* num_finals, bool* found, bool (*check)(State* s, GoalCondition* goal, TA* ta))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0;
    int tail = 0;

    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;
    visitState* visited  = NULL;  /* seen set  */
    exploring[tail++] = state;
    //visitState_add(&visited, state); //il faut pas ajouter l'etat d'ou on commence pour pouvoir le retrouver dans l'autoboucle
    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;
    *found = false;


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];
        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

       //if (visitState_find (&visited, current)) continue;
        for (int j = 0; j < num_succ; j++) {
           

            State* s = &succs[j];
             if (check(s, goal, ta))
        {
            if (visitState_find (&visited, *s)) continue;
         
            {
                bool present = false;

                /* ----- Border state ----- */
                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
                {
                    
                
                    /* vérifier doublon */
                    for (int k = 0; k < *num_finals; k++) {
                        if (equal_var(&(s->var),&(finals[k].var))) {  //(s->var.v == finals[k].var.v)
                            present = true;
                            break;
                        }
                    }
                   // Pas necessaire car on garde tout les etats borders en memoire
                    
                    if (!present) 
                    {
                        if (*num_finals >= capacity_finals) {
                            capacity_finals *= 2;
                            // printf("\n capacite augmente:\n");
                            State* tmp =
                                realloc(finals,
                                        capacity_finals * sizeof(State));// finals = realloc ()

                            if (!tmp) {
                                free(finals);
                                free(exploring);
                                free(succs);
                                return NULL;
                            }

                            finals = tmp;
                        }
                    
                        finals[*num_finals] = *s;
                     
                    //   if (equal_var(&state.var, &finals[*num_finals].var)){
                        
                    //     // printf("\n boucle_next_border");
                    //     // printf("\n num finals = %d", *num_finals);
                    
                    // }
                     (*num_finals)++;

                        //printf("\n num finals = %d", *num_finals);
                    } 
                   //  printf("\n num finals apres present = %d", *num_finals);

                }
                /* ----- Continue BFS ----- */
                else {

                    if (tail >= capacity) {
                        capacity *= 2;
                        
                        State* tmp =
                            realloc(exploring,
                                    capacity * sizeof(State));

                        if (!tmp) {
                            free(finals);
                            free(exploring);
                            free(succs);
                            printf("\n Erreur: Memoire depasse!!");
                            return NULL;
                        }

                        exploring = tmp;
                        //free (tmp);
                    
                    }
                    //print_state(s, ta->locations);
                    exploring[tail++] = *s;
                    
                   
                }

                visitState_add(&visited, *s);

             
            }

           
        }
        }
        // visitState_add(&visited, current);
                    // printf("\n num finals apres for = %d", *num_finals);

        //   if (equal_var(&state.var, &finals[0].var)){
        //             printf("\n boucle_next_border heeeereee");
        //             (*num_finals)++;
        //             printf("\n num finals = %d", *num_finals);
        //             free(exploring);
        //             free(visited);
        //             return finals;
        //           }

        free(succs);
    }

    free(exploring);
    free(visited);
    return finals;
}

int EG_p_2tables_Memory_Layer(TA* ta, int location, DBM clock, GoalCondition* goal,
                 bool (*check)(State* s, GoalCondition* goal, TA* ta),
                 int  (*heuristique_check)(State* s, GoalCondition* goal))
{

    State* init_state = compute_init_state(ta);
        int num_borders = 0;


    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init_state, goal, ta)) {
        free(init_state);
        printf("propriete non verifier dans init state");
        return 0;
    }

    int    init_weight = heuristique_check(init_state, goal);

    StateWeight* visiting   = NULL;   /* à explorer  (frontier) */
    visit*       visited = NULL;   /* déjà expansés           */

    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);

    free(init_state);
     int    num_succ = 0;
    while (HASH_COUNT(visiting) > 0) {

        /* --- Extraire le meilleur état de visiting --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

       

        /* --- Calculer les successeurs --- */
        bool   found    = false;
      
        State* successors = NextBorderMemoryEG(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
       
        
        // State* successors = EGNextBorder(ta, current, location, clock,
        //                                goal, &num_succ, &found, check);
        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);

        // if (equal_var(&current.var, &successors[0].var)){

        //    /// printf("\n equal var!!");

        //      if(num_succ == 1){
        //         printf("\n here true!!!");
        //      }
        // }
        if (boucle) {
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            // printf("\n boucle");
            return 1;
        }
      
        /* Cas général : on n'ajoute que les successeurs qui satisfont check */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
           

            /* EG : inutile d'explorer un état qui viole la propriété */
            // if (!check(s, goal, ta))
            //     continue;

           
           
            /* un successor déja visité */
            if (visit_find(&visited, *s) != NULL)
                  continue;
          
           

            int w = heuristique_check(s, goal);
           
            sw_add(&visiting, *s, w);
            visit_add(&visited, *s);
            num_borders ++;
            
        }

        free(successors);
    }

    /* open vide : aucun chemin infini satisfaisant trouvé */
    sw_destroy(&visiting);
    visit_destroy(&visited);

    printf("num_borders = %d", num_borders);

     return 0 ;
}


/********************************************************************  No memory *******************************************************/
int EF_pNO_memory(TA* ta, int location, DBM clock, GoalCondition* goal,State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int (*heuristique_check)(State* s, GoalCondition* goal)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    //if (!init_state) return 0;
    *result= NULL;
   if (check(init_state, goal, ta)) {  
                        *result = init_state; 
                        return 1;
                    }
    StateWeight* visiting = NULL;  /* frontier  */
   // visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state, goal);
    sw_add(&visiting, *init_state, init_weight);
   // visit_add(&visited,  *init_state);
   nbr_border_state ++;

    //free(init_state);

    while (HASH_COUNT(visiting) > 0) {

        /* --- Extract state with lowest weight --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

        /* --- Compute successors --- */
        int    num_succ  = 0;
        State* successors = NextBorder(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
        if (found) {
              *result = malloc(sizeof(State));
              **result = *successors;
            free(successors);
            sw_destroy(&visiting);
           // visit_destroy(&visited);
            return 1;
        }

        if (!successors) continue;

        bool boucle = (num_succ == 1) &&
                       equal_var(&current.var, &successors[0].var);
        if (boucle){
            if (check(&successors[0], goal, ta)) {
                    *result = malloc(sizeof(State));
                    **result = successors[0];
                    free(successors);
                    sw_destroy(&visiting);
                    //visit_destroy(&visited);
                    return 1;
                }
            else continue;
        }
        else {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
               
                /* Skip if already seen */
                // if (visit_find(&visited, *s) != NULL)
                //     { 
                //         continue;}

                if (check(s, goal, ta)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    sw_destroy(&visiting);
                   // visit_destroy(&visited);
                    return 1;
                }



                int w = heuristique_check(s, goal);
                sw_add(&visiting, *s, w);
                //visit_add(&visited,  *s);  /* mark as seen immediately */
                 nbr_border_state ++;

               
            }
        }

        free(successors);
    }

    sw_destroy(&visiting);
   // visit_destroy(&visited);
   printf("\n nombre d'états dans les borders: %d",nbr_border_state );

    return 0;
}

int EG_p_2tablesNo_memory(TA* ta, int location, DBM clock, GoalCondition* goal,
                 bool (*check)(State* s, GoalCondition* goal, TA* ta),
                 int  (*heuristique_check)(State* s, GoalCondition* goal))
{

    State* init_state = compute_init_state(ta);

    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init_state, goal, ta)) {
        free(init_state);
        printf("propriete non verifier dans init state");
        return 0;
    }

    int    init_weight = heuristique_check(init_state, goal);

    StateWeight* visiting   = NULL;   /* à explorer  (frontier) */
    //visit*       visited = NULL;   /* déjà expansés           */

    sw_add(&visiting, *init_state, init_weight);
    //visit_add(&visited,  *init_state);

    free(init_state);
     int    num_succ = 0;
    while (HASH_COUNT(visiting) > 0) {

        /* --- Extraire le meilleur état de visiting --- */
        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        HASH_DEL(visiting, best);
        free(best);

       

        /* --- Calculer les successeurs --- */
        bool   found    = false;
      
        State* successors = EGNextBorder(ta, current, location, clock,
                                       goal, &num_succ, &found, check);
       
        

        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);


        if (boucle) {
            free(successors);
            sw_destroy(&visiting);
           // visit_destroy(&visited);
           //  printf("\n boucle");
            return 1;
        }
      
        /* Cas général : on n'ajoute que les successeurs qui satisfont check */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
           

            /* EG : inutile d'explorer un état qui viole la propriété */
            // if (!check(s, goal, ta))
            //     continue;

           
           
            /* un successor déja visité */
            // if (visit_find(&visited, *s) != NULL)
            //       continue;
          
           

            int w = heuristique_check(s, goal);
           
            sw_add(&visiting, *s, w);
           // visit_add(&visited, *s);
            
        }

        free(successors);
    }

    /* open vide : aucun chemin infini satisfaisant trouvé */
    sw_destroy(&visiting);
    //visit_destroy(&visited);

     return 0 ;
}

/********************************************************************  full memory *******************************************************/


int EF_FullMemory(TA* ta, int location, DBM clock, GoalCondition* goal, State** result,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int (*heuristique_check)(State* s, GoalCondition* goal))
{
    *result = NULL;
    int nbr_border =0;

    State *init = compute_init_state(ta);
    if (!init) return 0;

    if (check(init, goal, ta)) {
        *result = init;
        return 1;
    }

    visitState* layer_visited = NULL;  /* états non-border visités globalement */
   // visit*      border_visited = NULL; /* états border visités globalement     */

    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0, tail = 0;
    State* exploring = malloc(capacity * sizeof(State));
    if (!exploring) { free(init); return 0; }

    /* ---------- Finals (border states en attente) ---------- */
    int capacity_finals = 32;
    int num_finals = 0;
    StateWeight* finals = malloc(capacity_finals * sizeof(StateWeight));
    if (!finals) { free(exploring); free(init); return 0; }

    /* Ajouter l'état initial */
    exploring[tail++] = *init;
    if ((init->location == location) &&
                    clock_zones_equal(init->clock_zone, clock, DBM_DIM))
                    {
                        visitState_add(&layer_visited,*init);
                        //visit_add(&border_visited,*init);
                        nbr_border++;
                    }
    else{
           visitState_add(&layer_visited, *init);
    }
   
    free(init);

    while (true) {

        /* ---------- BFS depuis l'état courant ---------- */
        while (head < tail) {

            State current = exploring[head++];

            if (check(&current, goal, ta)) {
                *result = malloc(sizeof(State));
                **result = current;
                free(exploring);
                free(finals);
                //visit_destroy(&border_visited);
                visitState_destroy(&layer_visited);
                return 1;
            }

            int num_succ = 0;
            State* succs = get_successors(ta, &current, &num_succ);

            for (int j = 0; j < num_succ; j++) {
                State* s = &succs[j];
                if (visitState_find(&layer_visited, *s)) continue;

                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
                {
                    /* ----- Border state ----- */
                    // if (visitState_find(&layer_visited, *s)) continue;
                   // if (visit_find(&border_visited, *s)) continue;
                   // visit_add(&border_visited, *s);
                    visitState_add(&layer_visited, *s);
                    nbr_border++;

                    if (num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        StateWeight* tmp = realloc(finals, capacity_finals * sizeof(StateWeight));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            //visit_destroy(&border_visited);
                            visitState_destroy(&layer_visited);
                            return 0;
                        }
                        finals = tmp;
                    }
                    finals[num_finals].state  = *s;
                    finals[num_finals].weight = heuristique_check(s, goal);
                    num_finals++;
                }
                else {
                    /* ----- État intermédiaire ----- */
                    if (visitState_find(&layer_visited, *s)) continue;
                    visitState_add(&layer_visited, *s);

                    if (tail >= capacity) {
                        capacity *= 2;
                        State* tmp = realloc(exploring, capacity * sizeof(State));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            //visit_destroy(&border_visited);
                            visitState_destroy(&layer_visited);
                            printf("\nErreur: Memoire depasse!!");
                            return 0;
                        }
                        exploring = tmp;
                    }
                    exploring[tail++] = *s;
                }
            }
            free(succs);
        }

        /* ---------- BFS épuisé : choisir le meilleur border state ---------- */
        if (num_finals == 0) break;

        int min_idx = 0;
        for (int i = 1; i < num_finals; i++) {
            if (finals[i].weight < finals[min_idx].weight)
                min_idx = i;
        }

        State next_state = finals[min_idx].state;

        /* Retirer le min du tableau (swap avec le dernier) */
        finals[min_idx] = finals[num_finals - 1];
        num_finals--;

        /* Relancer le BFS depuis ce border state */
        head = 0;
        tail = 0;
        exploring[tail++] = next_state;
        /* Pas besoin de re-ajouter dans layer_visited : déjà dans border_visited */
    }

    printf("\nnombre d'états dans les borders: %d", nbr_border);
    free(exploring);
    free(finals);
    //visit_destroy(&border_visited);
    visitState_destroy(&layer_visited);
    return 0;
}




int EG_FullMemory(TA* ta, int location, DBM clock, GoalCondition* goal, 
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int (*heuristique_check)(State* s, GoalCondition* goal))
{
   // *result = NULL;
    int nbr_border =0;
     int num_finals = 0;
    State *init = compute_init_state(ta);
    if (!init) return 0;

    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init, goal, ta)) {
        free(init);
        printf("propriete non verifier dans init state");
        return 0;
    }

    visitState* layer_visited = NULL;  /* états non-border visités globalement */
   // visit*      border_visited = NULL; /* états border visités globalement     */

    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0, tail = 0;
    State* exploring = malloc(capacity * sizeof(State));
    if (!exploring) { free(init); return 0; }

    /* ---------- Finals (border states en attente) ---------- */
    int capacity_finals = 32;
  
    StateWeight* finals = malloc(capacity_finals * sizeof(StateWeight));
   // if (!finals) { free(exploring); free(init); return 0; }

    /* Ajouter l'état initial */
    exploring[tail++] = *init;
 
    visitState_add(&layer_visited, *init);

   State last_border = *init;
    free(init);

    while (true) {

        /* ---------- BFS depuis l'état courant ---------- */
        
        while (head < tail) {

            State current = exploring[head++];
            if (check(&current, goal, ta)) {

                int num_succ = 0;
            State* succs = get_successors(ta, &current, &num_succ);
       
            
            for (int j = 0; j < num_succ; j++) {
                State* s = &succs[j];
                
                if (!check(s, goal, ta))continue;
              

                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
                {
                    /* ----- Border state ----- */
                    
                    /*-- verrifier si c une boucle*/
                    if (equal_var( &last_border.var, &s->var)){
                    //printf("\n boucle");
                    free(exploring);
                    free(finals);
                    return 1;
                  }

                if (visitState_find(&layer_visited, *s)) continue;
                  //printf("\n new border!!");
                    if (num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        StateWeight* tmp = realloc(finals, capacity_finals * sizeof(StateWeight));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            //visit_destroy(&border_visited);
                            visitState_destroy(&layer_visited);
                            return 0;
                        }
                        finals = tmp;
                    }
                    finals[num_finals].state  = *s;
                    finals[num_finals].weight = heuristique_check(s, goal);
                     nbr_border++;
                    num_finals++;
                }
                else {
                    /* ----- État intermédiaire ----- */
                 
                    if (visitState_find(&layer_visited, *s)) continue;
                    if (tail >= capacity) {
                        capacity *= 2;
                        State* tmp = realloc(exploring, capacity * sizeof(State));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            //visit_destroy(&border_visited);
                            visitState_destroy(&layer_visited);
                            printf("\nErreur: Memoire depasse!!");
                            return 0;
                        }
                        exploring = tmp;
                    }
                    exploring[tail++] = *s;
                }

                visitState_add(&layer_visited, *s);
                  
            }

               free(succs);
            }

            
           //visitState_add(&layer_visited, current);
        }

        /* ---------- BFS épuisé : choisir le meilleur border state ---------- */
        if (num_finals == 0) break;

        int min_idx = 0;
        for (int i = 1; i < num_finals; i++) {
            if (finals[i].weight < finals[min_idx].weight)
                min_idx = i;
        }

        State next_state = finals[min_idx].state;

        /* Retirer le min du tableau (swap avec le dernier) */
        finals[min_idx] = finals[num_finals - 1];
        num_finals--;

        /* Relancer le BFS depuis ce border state */
        head = 0; tail = 0;
        exploring[tail++] = next_state;
        last_border = next_state;

        // Vider layer_visited SAUF les border states déjà visités
        visitState_destroy(&layer_visited);
        layer_visited = NULL;
        // Ré-ajouter uniquement les borders connus pour garder la détection de cycle
        // (ils sont dans finals + next_state)
        visitState_add(&layer_visited, next_state);
        for (int i = 0; i < num_finals; i++) {
            visitState_add(&layer_visited, finals[i].state);
}
       
    }

    printf("\n nombre d'états dans les borders: %d", nbr_border);
    free(exploring);
    free(finals);
    //visit_destroy(&border_visited);
    visitState_destroy(&layer_visited);
    return 0;
}

/*======================================test======================================================*/
// int EF_p_recursive(TA* ta, State* s, int location, DBM clock,
//                    GoalCondition* goal, State** result,
//                    bool (*check)(State* s, GoalCondition* goal, TA* ta),
//                    int (*heuristique_check)(State* s, GoalCondition* goal),
//                    visit** visited)
// {
//     *result = NULL;

//     if (check(s, goal, ta)) {
//         *result = malloc(sizeof(State));
//         **result = *s;
//         return 1;
//     }

//     if (visit_find(visited, *s))
//         return 0;   /* ce point de départ déjà épuisé */

//     StateWeight* visiting = NULL;
//     sw_add(&visiting, *s, heuristique_check(s, goal));
//     visit_add(visited, *s);

//     while (HASH_COUNT(visiting) > 0) {

//         StateWeight *best = NULL, *cur, *tmp;
//         HASH_ITER(hh, visiting, cur, tmp) {
//             if (!best || cur->weight < best->weight)
//                 best = cur;
//         }
//         State current = best->state;
//         HASH_DEL(visiting, best);
//         free(best);

//         bool found    = false;
//         int  num_succ = 0;
//         State* successors = NextBorder(ta, current, location, clock,
//                                        goal, &num_succ, &found, check);
//         if (found) {
//             *result = malloc(sizeof(State));
//             **result = *successors;
//             free(successors);
//             sw_destroy(&visiting);
//             return 1;
//         }

//         if (!successors) continue;

//         bool boucle = (num_succ == 1) &&
//                       equal_var(&current.var, &successors[0].var);

//         if (!boucle) {
//             for (int i = 0; i < num_succ; i++) {
//                 State* ns = &successors[i];
//                 if (visit_find(visited, *ns)) continue;

//                 if (check(ns, goal, ta)) {
//                     *result = malloc(sizeof(State));
//                     **result = *ns;
//                     free(successors);
//                     sw_destroy(&visiting);
//                     return 1;
//                 }

//                 sw_add(&visiting, *ns, heuristique_check(ns, goal));
//                 visit_add(visited, *ns);
//             }
//         }
//         free(successors);
//     }

//     sw_destroy(&visiting);
//     return 0;
// }


// int EFPn_recursive(TA* ta, int location, DBM clock,
//                    GoalCondition* goal, int nbr_prop,
//                    bool (*check)(State* s, GoalCondition* goal, TA* ta),
//                    int (*heuristique_check)(State* s, GoalCondition* goal))
// {
//     visit**  visitedg    = calloc(nbr_prop, sizeof(visit*));
//     State**  start_states = calloc(nbr_prop, sizeof(State*));
//     State**  found_states = calloc(nbr_prop, sizeof(State*));

//     if (!visitedg || !start_states || !found_states) {
//         free(visitedg); free(start_states); free(found_states);
//         return 0;
//     }

//     start_states[0] = compute_init_state(ta);
//     int i = 0;

//     while (i < nbr_prop) {

//         //printf("\n prop = %d", i);

//         State* result = NULL;
//         int ok = EF_p_recursive(ta, start_states[i], location, clock,
//                                  &goal[i], &result,
//                                  check, heuristique_check,
//                                  &visitedg[i]);
//         if (ok) {
//             if (found_states[i]) free(found_states[i]);
//             found_states[i] = result;

//             if (i + 1 < nbr_prop) {
//                 if (start_states[i + 1]) free(start_states[i + 1]);
//                 start_states[i + 1] = malloc(sizeof(State));
//                 *start_states[i + 1] = *result;
//             }
//             i++;

//         } else {
//             free(result);

//             if (i == 0) {
//                 printf("\nEFPn: propriété non satisfaite.\n");
//                 goto cleanup;
//             }

//             /* Détruire le niveau i pour repartir proprement */
//             visit_destroy(&visitedg[i]);
//             visitedg[i] = NULL;

//             i--;

//             /* ← FIX : marquer l'état trouvé au niveau i comme épuisé
//                pour que EF_p_recursive en trouve un autre au prochain appel */
//             if (found_states[i]) {
//                 visit_add(&visitedg[i], *found_states[i]);
//                 free(found_states[i]);
//                 found_states[i] = NULL;
//             }
//         }
//     }

//     printf("\nEFPn: toutes les propriétés satisfaites.\n");

// cleanup:
//     for (int k = 0; k < nbr_prop; k++) {
//         visit_destroy(&visitedg[k]);
//         free(found_states[k]);
//         free(start_states[k]);
//     }
//     free(visitedg);
//     free(start_states);
//     free(found_states);
//     return (i == nbr_prop) ? 1 : 0;
// }