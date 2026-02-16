#include <stdlib.h>
#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include "uthash.h"
#include "structure_DBM.h"
#include "hashTable.h"
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

bool check_p(State* s, int goal,TA* ta){
    //printf("\n i'm checking: \n");
    //print_state(s, ta->locations);
    if (s->var.v == goal) {
        printf("\n value %d found\n", goal);
        return true;
    }
    return false;
}

/*===============Exploration Alogorithms=============================================*/

State* NextBorder(TA* ta, State state, State* init_state,
                  int goal, int* num_finals, bool* found)
{
    /* ---------- Queue BFS ---------- */
    int capacity = 10;
    int head = 0;
    int tail = 0;

    State* exploring = malloc(capacity * sizeof(State));
    if (!exploring) return NULL;

    exploring[tail++] = state;

    /* ---------- Finals ---------- */
    int capacity_finals = 4;
    State* finals = malloc(capacity_finals * sizeof(State));
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;
    *found = false;

    printf("\n starting exploration with nextborder\n");

    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

             /*----check in BFS---------*/
        if (check_p(&current, goal, ta)){
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
            if ((s->location == init_state->location) &&
                clock_zones_equal(s->clock_zone,
                                  init_state->clock_zone,
                                  DBM_DIM))
            {
                /* vérifier doublon */
                for (int k = 0; k < *num_finals; k++) {
                    if (s->var.v == finals[k].var.v) {
                        present = true;
                        break;
                    }
                }

                if (!present) {

                    if (*num_finals >= capacity_finals) {
                        capacity_finals *= 2;

                        State* tmp =
                            realloc(finals,
                                    capacity_finals * sizeof(State));

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
                        return NULL;
                    }

                    exploring = tmp;
                }

                exploring[tail++] = *s;
            }
        }

        free(succs);
    }

    free(exploring);
    return finals;
}




typedef struct ListNode {
    State state;
    struct ListNode* next;
} ListNode;


void list_add(ListNode** list, State s) {
    ListNode* node = malloc(sizeof(ListNode));
    node->state = s;
    node->next = *list;
    *list = node;
}

State list_extract_best(ListNode** list) {
    ListNode* prev_best = NULL;
    ListNode* best = *list;
    ListNode* prev = NULL;
    ListNode* cur = *list;

    while (cur != NULL) {
        if (cur->state.var.v > best->state.var.v) { // HEURISTIQUE
            best = cur;
            prev_best = prev;
        }
        prev = cur;
        cur = cur->next;
    }

    // retirer de la liste
    if (prev_best == NULL)
        *list = best->next;
    else
        prev_best->next = best->next;

    State s = best->state;
    free(best);

    return s;
}

int EF_p(TA* ta, State* init_state, int goal, bool (*check)(State* s, int goal, TA* ta)) {
    bool found = false;
    ListNode* exploring = NULL;
    
    // Table de hachage pour suivre les états déjà visités/explorés
    HashTable* visited = hash_table_create();
    
    list_add(&exploring, *init_state);
    hash_table_add(visited, *init_state);  // Marquer l'état initial
    
    while (exploring != NULL) {
        State current = list_extract_best(&exploring);
        printf("\nvaleur de v %d", current.var.v);
        
        int num_succ = 0;
        State* successors = NextBorder(ta, current, init_state, goal, &num_succ, &found);
        
        if (found) {
            printf("\nProperty FOUND in NextBorder! Returning 1\n");
            free(successors);
            hash_table_destroy(visited);
            return 1;
        }
        
        printf("\nnbr de sucessors de current %d est : %d\n", current.var.v, num_succ);
        
        if (!((num_succ == 1) && (current.var.v == successors[0].var.v))) {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
                
                // Vérifier si l'état n'a pas déjà été visité/exploré
                if (!hash_table_contains(visited, *s)) {
                    print_state(s, ta->locations);
                    
                    if (check_p(s, goal, ta)) {
                        free(successors);
                        hash_table_destroy(visited);
                        printf("\nProperty FOUND in border! Returning 1\n");
                        return 1;
                    }
                    
                    list_add(&exploring, *s);
                    hash_table_add(visited, *s);  // Marquer comme visité
                }
            }
            
            if (num_succ > 0) {
                free(successors);
            }
        }
    }
    
    hash_table_destroy(visited);
    printf("\nvaleur pas trouvee\n");
    return 0;
}


// int EF_p(TA* ta,State* init_state, int goal, bool (*check)(State* s, int goal,TA* ta)) {
  
//     bool found = false;

//    ListNode* exploring = NULL;
//    list_add(&exploring, *init_state);

//     while (exploring != NULL) {

//         State current = list_extract_best(&exploring);

//         int num_succ = 0;
//         printf("\n valeur de v %d", current.var.v);
        
     
        
//             printf("\nici depth avec valeur %d", current.var.v);
//            State* successors = NextBorder(ta, current, init_state, goal, &num_succ, &found);

//             // Vérifier si la propriété a été trouvée dans NextBorder
//             if (found) {
//                 printf("\n Property FOUND in NextBorder! Returning 1\n");
//                 free(successors);
//                 return 1;   // TRUE
//             }
//             printf("\n  nbr de sucessors de current %d  est : %d \n", current.var.v,num_succ);
//             // arreter si num_scc =1 et successor[0]=current 
//             if(!((num_succ == 1) &&(current.var.v == successors[0].var.v)))
//             {
//             for (int i = 0; i < num_succ; i++) {
//                 State* s = &successors[i];
//                 print_state(s,ta->locations);
//                 // Vérifier aussi les états frontières
//                 if (check_p(s, goal,ta)){
//                     free(successors);
//                     printf("\n Property FOUND in border! Returning 1\n");
//                     return 1;   // TRUE
//                 }
               
//                 // Continuer l'exploration
//                list_add(&exploring, *s);
//             }
            
//             if (num_succ > 0) {
//                 free(successors);
//             }
//         }
        
//     }
    
//     printf("\n valeur pas trouvee\n");
//     return 0;  // FALSE
// }