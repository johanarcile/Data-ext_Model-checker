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
        printf("\t Variables       : v = %d\n", etat->var.v);
        printf("\t Variables       : Active = %d\n", etat->var.active);
        printf("\t Variables       : name = %s\n", etat->var.name);
         printf("\t Variables       : table = [ %d, %d, %d,] \n", etat->var.table[0], etat->var.table[1],etat->var.table[2]);
        


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
        if (s->var.v != goal->v)
            return false;
    }

    if (goal->mask & CHECK_ACTIVE) { // pour tester v il faut 010 et goal.mask !=0 
         printf("\n checking ACTIVE");
        if (s->var.active != goal->active)
            return false;
    }

    if (goal->mask & CHECK_NAME) {// pour tester v il faut 100 et goal.mask !=0 
        printf("\n checking Name");
        if (strcmp(s->var.name, goal->name) != 0)
            return false;
    }

    return true;
}

bool check_p_inf(State* s, GoalCondition* goal, TA* ta) {

    if (goal->mask & CHECK_V) { // pour tester v il faut 001 et goal.mask !=0 
        printf("\n checking v");
        if (s->var.v >= goal->v)
            return false;
    }

    if (goal->mask & CHECK_ACTIVE) { // pour tester v il faut 010 et goal.mask !=0 
         printf("\n checking ACTIVE");
        if (s->var.active != goal->active)
            return false;
    }

    if (goal->mask & CHECK_NAME) {// pour tester v il faut 100 et goal.mask !=0 
        printf("\n checking Name");
        if (strcmp(s->var.name, goal->name) != 0)
            return false;
    }

    return true;
}

bool check_p_sup(State* s, GoalCondition* goal, TA* ta) {

    if (goal->mask & CHECK_V) { // pour tester v il faut 001 et goal.mask !=0 
        printf("\n checking v");
        if (s->var.v <= goal->v)
            return false;
    }

    if (goal->mask & CHECK_ACTIVE) { // pour tester v il faut 010 et goal.mask !=0 
         printf("\n checking ACTIVE");
        if (s->var.active != goal->active)
            return false;
    }

    if (goal->mask & CHECK_NAME) {// pour tester v il faut 100 et goal.mask !=0 
        printf("\n checking Name");
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

             /*----check in BFS---------*/
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


/* essayer de paralleliser la boucle des successor dans next border:   */

// State* NextBorder(TA* ta, State state, int location, DBM clock,
//                   GoalCondition *goal, int* num_finals, bool* found, bool (*check)(State* s, GoalCondition* goal, TA* ta))
// {
//     /* ---------- Queue BFS ---------- */
//     int capacity = 32;
//     int head = 0;
//     int tail = 0;

//     State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
//     if (!exploring) return NULL;

//     exploring[tail++] = state;

//     /* ---------- Finals ---------- */
//     int capacity_finals = 32;
//     State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
//     if (!finals) {
//         free(exploring);
//         return NULL;
//     }

//     *num_finals = 0;
//     *found = false;


//     /* ---------- BFS ---------- */
//     while (head < tail) {

//         State current = exploring[head++];

//         int num_succ = 0;
//         State* succs = get_successors(ta, &current, &num_succ);

//              /*----check in BFS---------*/
//         if (check(&current, goal, ta)){
//              printf("\nProperty found in NextBorder!");
//              *found = true;
//               State* result = malloc(sizeof(State));
//              *result = current;
//               free(exploring);
//               free(finals);
//               *num_finals = 0;
//               return result;

//              }


//        #pragma omp parallel
// {
//     State* local_finals = malloc(16 * sizeof(State));
//     int local_num_finals = 0;
//     int local_cap_finals = 16;

//     State* local_exploring = malloc(16 * sizeof(State));
//     int local_tail = 0;
//     int local_cap = 16;

//     #pragma omp for nowait
//     for (int j = 0; j < num_succ; j++) {

//         State* s = &succs[j];
//         bool present = false;

//         if ((s->location == location) &&
//             clock_zones_equal(s->clock_zone, clock, DBM_DIM))
//         {
//             for (int k = 0; k < *num_finals; k++) {
//                 if (equal_var(&(s->var), &(finals[k].var))) {
//                     present = true;
//                     break;
//                 }
//             }

//             if (!present) {

//                 if (local_num_finals >= local_cap_finals) {
//                     local_cap_finals *= 2;
//                     local_finals = realloc(local_finals,
//                                            local_cap_finals * sizeof(State));
//                 }

//                 local_finals[local_num_finals++] = *s;
//             }
//         }
//         else {

//             if (local_tail >= local_cap) {
//                 local_cap *= 2;
//                 local_exploring = realloc(local_exploring,
//                                           local_cap * sizeof(State));
//             }

//             local_exploring[local_tail++] = *s;
//         }
//     }

//     /* Merge global */
//     #pragma omp critical
//     {
//         for (int i = 0; i < local_num_finals; i++) {

//             bool present = false;
//             for (int k = 0; k < *num_finals; k++) {
//                 if (equal_var(&(local_finals[i].var),
//                               &(finals[k].var))) {
//                     present = true;
//                     break;
//                 }
//             }

//             if (!present) {

//                 if (*num_finals >= capacity_finals) {
//                     capacity_finals *= 2;
//                     finals = realloc(finals,
//                                      capacity_finals * sizeof(State));
//                 }

//                 finals[*num_finals] = local_finals[i];
//                 (*num_finals)++;
//             }
//         }

//         for (int i = 0; i < local_tail; i++) {

//             if (tail >= capacity) {
//                 capacity *= 2;
//                 exploring = realloc(exploring,
//                                     capacity * sizeof(State));
//             }

//             exploring[tail++] = local_exploring[i];
//         }
//     }

//     free(local_finals);
//     free(local_exploring);
// }

//         free(succs);
//     }

//     free(exploring);
//     return finals;
// }
/* ==========================================================================================EF(p)====================*/


/* ================================================================== */
/*  Visited hash table (table pour enregistrer les états visités)     */
/* ================================================================== */



void visit_add(visit** table, State s) {
    
        visit* e = NULL;
        e = malloc(sizeof(visit));
        e->key    = s.var;
        //e->explored  = true;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);
 
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






// // // /*==========================================tester tabele hashage (2 tables comme prmeière approche avec list et table) ===================*/


/* ------------------------------------------------------------------ */
/*  Structure 1 : frontière  (état + weight)                          */
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

int EF_p(TA* ta, int location, DBM clock, GoalCondition* goal,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int (*heuristique_check)(State* s, GoalCondition* goal)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    //if (!init_state) return 0;

    StateWeight* visiting = NULL;  /* frontier  */
    visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state, goal);
    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);
   nbr_border_state ++;

    free(init_state);

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
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            return 1;
        }

        //if (!successors) continue;

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

/* ================================================================== */
/*  EF_p avec min-heap                                                */
/* ================================================================== */

int EF_p_HV(TA* ta, int location, DBM clock, GoalCondition* goal,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int  (*heuristique_check)(State* s, GoalCondition* goal)) 
{

    //if (!ta) return 0;
    int nbr_border_state =0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
   // if (!init_state) return 0;//Vérifiecation

    MinHeap*     heap    = heap_create(64);
    visit* visited = NULL;

    int init_weight = heuristique_check(init_state, goal);
    heap_push(heap, *init_state, init_weight);
    visit_add(&visited, *init_state);                
    nbr_border_state ++;

    free(init_state);
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

/*============================test de pool===================================*/

typedef struct {
    State * state;
    int weight;
} HeapNodeP;

typedef struct {
    HeapNodeP* data;        //  FIX #1 : était HeapNode*, doit être HeapNodeP*
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
        h->data = realloc(h->data, sizeof(HeapNodeP) * h->capacity); //  FIX #3 : était sizeof(HeapNode)
    }
    h->data[h->size].state  = s;   //  FIX #4 : était -> (pointeur), data[] est un tableau de structs donc .
    h->data[h->size].weight = w;
    h->size++;
    heap_sift_upP(h, h->size-1);
    
}

HeapNodeP heap_popP(MinHeapP* h) {  //  FIX #5 : retournait HeapNode, doit retourner HeapNodeP
    HeapNodeP best = h->data[0];    //  FIX #6 : même correction ici
    h->data[0] = h->data[h->size - 1];
    h->size--;
    if (h->size > 0)
        heap_sift_downP(h, 0);
    
    return best;
}

// typedef struct {
//     State* buffer;    // la grande zone mémoire pré-allouée
//     int    capacity;
//     int    used;      // prochain index disponible
// } StatePool;

// StatePool* pool_create(int capacity) {
//     StatePool* p  = malloc(sizeof(StatePool));
//     p->buffer     = malloc(sizeof(State) * capacity);  // UN seul malloc
//     p->capacity   = capacity;
//     p->used       = 0;
//      printf("\n pool created");
//     return p;
// }

// // "Allouer" un State = juste prendre le prochain slot
// State* pool_alloc(StatePool* p) {
//     if (p->used == p->capacity) {
//         p->capacity *= 2;
//         p->buffer = realloc(p->buffer, sizeof(State) * p->capacity);

//         State* tmp = realloc(p->buffer, sizeof(State) * p->capacity);// finals = realloc ()

//                         if (!tmp) {
//                            printf("\n probleme realloc poool");
//                             return NULL;
//                         }
//                         p->buffer= tmp;

//     }
//     return &p->buffer[p->used++];   // ← quasi gratuit, juste un incrément
// }

// // Libérer TOUT le pool d'un coup
// void pool_destroy(StatePool* p) {
//     free(p->buffer);   // UN seul free pour tous les states
//     free(p);
// }


typedef struct PoolBlock {
    State*        buffer;
    int           capacity;
    int           used;
    struct PoolBlock* next;   // bloc suivant si celui-ci est plein
} PoolBlock;

typedef struct {
    PoolBlock* head;   // bloc courant
} StatePool;

StatePool* pool_create(int capacity) {
    StatePool*  p     = malloc(sizeof(StatePool));
    PoolBlock*  block = malloc(sizeof(PoolBlock));
    block->buffer     = malloc(sizeof(State) * capacity);
    block->capacity   = capacity;
    block->used       = 0;
    block->next       = NULL;
    p->head           = block;
    return p;
}

State* pool_alloc(StatePool* p) {
    PoolBlock* block = p->head;

    if (block->used == block->capacity) {
        // Créer un nouveau bloc — l'ancien reste en place !
        PoolBlock* new_block = malloc(sizeof(PoolBlock));
        new_block->capacity  = block->capacity * 2;
        new_block->buffer    = malloc(sizeof(State) * new_block->capacity);
        new_block->used      = 0;
        new_block->next      = block;  // chaîner l'ancien
        p->head              = new_block;
        block                = new_block;
    }
    return &block->buffer[block->used++];  // adresse STABLE
}

void pool_destroy(StatePool* p) {
    PoolBlock* block = p->head;
    while (block) {
        PoolBlock* next = block->next;
        free(block->buffer);
        free(block);
        block = next;
    }
    free(p);}

int EF_p_HV_Pool(TA* ta, int location, DBM clock, GoalCondition* goal,
         bool (*check)(State* s, GoalCondition* goal, TA* ta),
         int  (*heuristique_check)(State* s, GoalCondition* goal))
{
    bool found = false;
    StatePool* pool = pool_create(64);

    State* init_state = compute_init_state(ta);
    State* init_slot  = pool_alloc(pool);   //  slot dans le pool
    *init_slot        = *init_state;        //  copier dedans
    free(init_state);                       //  libérer l'original

    MinHeapP* heap  = heap_createP(64);
    visit* visited  = NULL;

    heap_pushP(heap, init_slot, heuristique_check(init_slot, goal)); //  pousser init_slot
    visit_add(&visited, *init_slot);

    while (heap->size > 0) {

        HeapNodeP best   = heap_popP(heap);
        State*   current = best.state;

        //print_state(current, ta->locations);

        int    num_succ  = 0;
        State* successors = NextBorder(ta, *current, location, clock,
                                       goal, &num_succ, &found, check);

        if (found) {
            free(successors);
            heap_destroyP(heap);
            visit_destroy(&visited);
            pool_destroy(pool);    // libère tout les states d'un coup
            return 1;
        }

        if (successors) {
            bool boucle = (num_succ == 1) &&
                          equal_var(&current->var, &successors[0].var);

            if (!boucle) {
                for (int i = 0; i < num_succ; i++) {
                    State* temp = &successors[i];

                    if (visit_find(&visited, *temp) != NULL) continue;

                    if (check(temp, goal, ta)) {
                        free(successors);
                        heap_destroyP(heap);
                        visit_destroy(&visited);
                        pool_destroy(pool);   
                        return 1;
                    }

                    State* new_state = pool_alloc(pool);  //  plus de malloc
                    *new_state = *temp;

                    heap_pushP(heap, new_state, heuristique_check(new_state, goal));
                    visit_add(&visited, *new_state);
                }
            }
            free(successors);
        }
        //  pas de free(current) — le pool s'en occupe
    }

    heap_destroyP(heap);
    visit_destroy(&visited);
    pool_destroy(pool);   //  UN seul appel libère tous les states
    return 0;
}


