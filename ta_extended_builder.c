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







/* ================================================================================================================================= */
/*                                                      On the fly exploration algorithms                                           */
/* =============================================================================================================================== */



/*******************************************************************memory on borders*******************************************************************************/


//---------------------------------Nex next border avec table de hashage----------------------------------

State* NextBorder(TA* ta, State state, int location, DBM clock,
                 int* num_finals, bool* found,int* num_v, bool (*check)(State*s))
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

    visit* finals_table = NULL;   // table de hashage pour les doublons (border states)


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];
        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

             /*----check in BFS for EFP---------*/
        if (check(&current)){ // Retourner l etat satisfaisant la propriété dès qu'on le trouve
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

            /* ----- Border state ----- */
            if ((s->location == location) &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))
            {
                  /* vérifier doublon via table de hashage */
                if (visit_find(&finals_table, *s) == NULL) {

                         if (*num_finals >= capacity_finals) {
                                 capacity_finals *= 2;
                                 State* tmp = realloc(finals, capacity_finals * sizeof(State));

                                 if (!tmp) {
                                         free(finals);
                                         free(exploring);
                                        free(succs);
                                        visit_destroy(&finals_table);
                                         return NULL;
                                         printf("\n error memoire");
                                         }

                                finals = tmp;
                         }

                        finals[*num_finals] = *s;
                        (*num_finals)++;

                        visit_add(&finals_table, *s);  // marquer comme vu
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
                   // free (tmp);
                  
                }
                exploring[tail++] = *s;

            }
        }
         (*num_v)++;
        free(succs);
    }

    free(exploring);
    return finals;
}




/*---------------------------Next border pour EG ---------------------------------------------------------*/

State* EGNextBorder(TA* ta, State state, int location, DBM clock,
                 int* num_finals, int* num_visited, bool (*check)(State*s))
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

    visit* finals_table = NULL;   // table de hashage pour les doublons (border states)

    /* ---------- BFS ---------- */
    while (head < tail) {   // tq y'a des etats a explorer

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

        for (int j = 0; j < num_succ; j++) { // pour chaque successeur

            State* s = &succs[j];

            if (check(s)){ // si il satsfaIt la propriete 

                 /* ----- Border state ----- */
            if ((s->location == location) &&
                clock_zones_equal(s->clock_zone, clock, DBM_DIM))  //si il est un etat border
            {
     
                 if (visit_find(&finals_table, *s) == NULL) { // si il n'exite pas déja on l'ajoute à finals
                   
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
                    visit_add(&finals_table, *s);  // marquer comme vu
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
                    free (tmp);
                  
                }
                //print_state(s, ta->locations);
                exploring[tail++] = *s;

            }

            }
           
        }
         (*num_visited)++;
        free(succs);

    }
  
    free(exploring);
    return finals;
}









/* ==========================================================================================EF(p)====================*/


 /*==========================================   2 tabeles de hashage   ===================*/

int EF_p(TA* ta, int location, DBM clock,State** result,
         bool (*check)(State*s),
         int (*heuristique_check)(State*s)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    int num_visited = 0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    //if (!init_state) return 0;
    *result= NULL;
   
   if (check(init_state)) {  
    
                        *result = init_state; 
                         printf("\n nombre d etats vistes: %d", nbr_border_state ++);

                        return 1;
                    }
    StateWeight* visiting = NULL;  /* frontier  */
    visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state);
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
                                        &num_succ, &found,&num_visited, check);
        if (found) {
              *result = malloc(sizeof(State));
              **result = *successors;
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
             printf("\n nombre d etats vistes: %d", num_visited);
            return 1;
        }

        if (!successors) continue;

       // bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);
       // if (!boucle) {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
               
                /* Skip if already seen */
                if (visit_find(&visited, *s) != NULL)
                    { 
                        continue;}

                if (check(s)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    sw_destroy(&visiting);
                    visit_destroy(&visited);
                     printf("\n nombre d etats vistes: %d", num_visited);
                    return 1;
                }



                int w = heuristique_check(s);
                sw_add(&visiting, *s, w);
                visit_add(&visited,  *s);  /* mark as seen immediately */
                 nbr_border_state ++;

               
            }
       // }

        free(successors);
    }

    sw_destroy(&visiting);
    visit_destroy(&visited);
    printf("\n nombre d'états dans les borders: %d",nbr_border_state );
    printf("\n nombre d'états dans visite durant le parcours: %d",num_visited );

    return 0;
}



                                               
/* ============================ EF_p avec min-heap et visited ====================================== */


int EF_p_HV(TA* ta, int location, DBM clock,State** result,
         bool (*check)(State*s),
         int  (*heuristique_check)(State*s)) 
{

    //if (!ta) return 0;
    int nbr_border_state =0;
    int n = 0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
   // if (!init_state) return 0;//Vérifiecation
   *result= NULL;
   if (check(init_state)) { 
                       *result= init_state; 
                        return 1;
                    }
    MinHeap*     heap    = heap_create(64);
    visit* visited = NULL;

    int init_weight = heuristique_check(init_state);
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
                                        &num_succ, &found,&n, check);

        if (found) {
            *result = malloc(sizeof(State));
            **result = *successors;
            free(successors);
            heap_destroy(heap);
            visit_destroy(&visited);
            return 1;
        }

        if (!successors) continue;

        // bool boucle = (num_succ == 1) &&
        //                equal_var(&current.var, &successors[0].var);

        // if (!boucle) {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];

                if (visit_find(&visited, *s) != NULL)
                    continue;

                if (check(s)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    heap_destroy(heap);
                    visit_destroy(&visited);
                    return 1;
                }

                int w = heuristique_check(s);
                heap_push(heap, *s, w);
                visit_add(&visited, *s);
                nbr_border_state++;
            }
       // }

        free(successors);
    }

    heap_destroy(heap);
    visit_destroy(&visited);
    printf("\n nombre d'états dans les borders: %d",nbr_border_state );
    printf("\n nombre d'états dans visite durant le parcours: %d",n );

    return 0;
}

/*============================EFP miheap state avec pointeur===================================*/

int EF_p_HV_M(TA* ta, int location, DBM clock,State** result,
         bool (*check)(State*s),
         int  (*heuristique_check)(State*s))
{
    bool found = false;
    int n = 0;
    *result= NULL;
    State* init_state = compute_init_state(ta);
      if (check(init_state)) { 
                        *result = init_state;  
                        return 1;
                    }

    MinHeapP* heap = heap_createP(64);  
    visit* visited = NULL;

    int init_weight = heuristique_check(init_state);

    heap_pushP(heap, init_state, init_weight);
    visit_add(&visited, *init_state);

     int num_succ = 0;
    while (heap->size > 0) {

        HeapNodeP best = heap_popP(heap);  
        State* current = best.state;

     
        State* successors = NextBorder(ta, *current, location, clock,
                                        &num_succ, &found,&n, check);

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

            // bool boucle = (num_succ == 1) &&
            //               equal_var(&current->var, &successors[0].var);

          //  if (!boucle) {

                for (int i = 0; i < num_succ; i++) {

                    State* temp = &successors[i];

                    if (visit_find(&visited, *temp) != NULL)
                        continue;

                    if (check(temp)) {
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

                    int w = heuristique_check(new_state);

                    heap_pushP(heap, new_state, w);
                    visit_add(&visited, *new_state);
                }
          //  }

            free(successors);
        }

        free(current);
    }

    heap_destroyP(heap);
    visit_destroy(&visited);

    printf("\n nombre d'états dans visite durant le parcours: %d",n );

    return 0;
}


/* ==========================================================================================EG(p)====================*/


/*============================EGP miheap state avec pointeur===================================*/

int EG_p_HV_M(TA* ta, int location, DBM clock,
          bool (*check)(State*s),
          int  (*heuristique_check)(State*s))
{
    State* init_state = compute_init_state(ta);
    //State * last;
    /* L'état initial doit satisfaire la propriété */
    if (!check(init_state)) {
        free(init_state);
        return 0;
    }

    MinHeapP* heap   = heap_createP(64);
    visit*   visited = NULL;

    int init_weight = heuristique_check(init_state);
    heap_pushP(heap, init_state, init_weight);
    visit_add(&visited, *init_state);
     int  num_succ = 0;
    while (heap->size > 0) {

        HeapNodeP best    = heap_popP(heap);
        State*    current = best.state;

        int found = 0;
        
        State* successors = EGNextBorder(ta, *current, location, clock,
                                        &num_succ, &found, check);

        

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
            int w = heuristique_check(new_state);
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

int EG_p_2tables(TA* ta, int location, DBM clock,
                 bool (*check)(State*s),
                 int  (*heuristique_check)(State*s))
{

    State* init_state = compute_init_state(ta);
    int nbr_border = 0;
    int num_visited = 0; 

    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init_state)) {
        free(init_state);
        printf("propriete non verifier dans init state");
        return 0;
    }

    int    init_weight = heuristique_check(init_state);

    StateWeight* visiting   = NULL;   /* à explorer  (frontier) */
    visit*       visited = NULL;   /* déjà expansés           */

    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);
    nbr_border++;
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
                                        &num_succ, &num_visited, check);
       
        

        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);


        if (boucle) {
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            // printf("\n boucle");
             printf("\n nombre d etats vistes: %d", num_visited);
            return 1;
        }
      
        /* Cas général : on n'ajoute que les successeurs pas encore visités */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
  
            /* un successor déja visité */
            if (visit_find(&visited, *s) != NULL)
                  continue;
          
            int w = heuristique_check(s);
           
            sw_add(&visiting, *s, w);
            visit_add(&visited, *s);
            nbr_border++;
            
        }

        free(successors);
    }

    
   printf("\n nombre d etats vistes: %d", num_visited);
   printf("\n nombre de border states: %d", nbr_border);

    /* open vide : aucun chemin infini satisfaisant trouvé */
    sw_destroy(&visiting);
    visit_destroy(&visited);

     return 0 ;
}




/************************************************************************** Memory inside the layers ******************************************************************/




State* NextBorderMemory(TA* ta, State state, int location, DBM clock,
                 int* num_finals, bool* found,int * num_visited, bool (*check)(State*s))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0;
    int tail = 0;
    int idx = 0;
    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;
   StateHash* visited  = NULL;  /* une table de hashage pour suavegarder les etats visités avec clé de hashage tout le state */
    exploring[tail++] = state;
    visitState_add(&visited, state);
    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
   
    *num_finals = 0;
    *found = false;


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];

        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);

             /*----check in BFS for EFP---------*/
        if (check(&current)){
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
            if (visitState_find (&visited, *s)) continue; // si un successuer est déja trouver skip
            else{
                //bool present = false;

                /* ----- Border state ----- */
                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
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
                visitState_add(&visited, *s); // Successeur est ajouté soit a border soit à exloring => ajouter à visited
                
            }

        }
       // print_state(&current, ta->locations);
         (*num_visited)++;

        free(succs);
    }

    free(exploring);
    free(visited);
    return finals;
}

int EF_p_Memory_in_Layer(TA* ta, int location, DBM clock,State** result,
         bool (*check)(State*s),
         int (*heuristique_check)(State*s)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    int num_visited = 0;
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    *result= NULL;
   if (check(init_state)) {  
                        *result = init_state;
                         printf("\n nombre d etats vistes: %d", 1); 
                        return 1;
                    }
    StateWeight* visiting = NULL;  /* frontier  */
    visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state);
    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);
    nbr_border_state++;


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
                                        &num_succ, &found,&num_visited, check);   // Appel à nextborder memory
        if (found) {
              *result = malloc(sizeof(State));
              **result = *successors;
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
             printf("\n nombre d etats vistes: %d", num_visited);
            return 1;
        }

        if (!successors) continue;

       
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
               
                /* Skip if already seen */
                if (visit_find(&visited, *s) != NULL)
                    { 
                        continue;}

                if (check(s)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    sw_destroy(&visiting);
                    visit_destroy(&visited);
                     printf("\n nombre d etats vistes: %d", num_visited);
                    return 1;
                }



                int w = heuristique_check(s);
                sw_add(&visiting, *s, w);
                visit_add(&visited,  *s);  /* mark as seen immediately */
                nbr_border_state ++;

               
            }
     
        free(successors);
    }

    sw_destroy(&visiting);
    visit_destroy(&visited);
    printf("\n nombre d'états dans les borders: %d",nbr_border_state );
    printf("\n nombre d etats vivites : %d",num_visited );


    return 0;
}
/*--------------------EG(p)------------------------------------------------*/
State* NextBorderMemoryEG(TA* ta, State state, int location, DBM clock,
                 int* num_finals, int* num_visited, bool (*check)(State*s))
{
    /* ---------- Queue BFS ---------- */
    int capacity = 32;
    int head = 0;
    int tail = 0;

    State* exploring = malloc(capacity * sizeof(State));// trouver une optimisation sans le malloc
    if (!exploring) return NULL;
    StateHash* visited  = NULL;  /* une table de hashage pour suavegarder les etats visités avec clé de hashage tout le state */
    exploring[tail++] = state;

    /* ---------- Finals ---------- */
    int capacity_finals = 32;
    State* finals = malloc(capacity_finals * sizeof(State));// trouver une optimisation sans le malloc
    if (!finals) {
        free(exploring);
        return NULL;
    }

    *num_finals = 0;


    /* ---------- BFS ---------- */
    while (head < tail) {

        State current = exploring[head++];
        int num_succ = 0;
        State* succs = get_successors(ta, &current, &num_succ);
       
       //if (visitState_find (&visited, current)) continue;
        for (int j = 0; j < num_succ; j++) {
           

            State* s = &succs[j];
             if (check(s))
        {
            if (visitState_find (&visited, *s)) continue; // skip si le successeur exite déja 
         
            {
                bool present = false;

                /* ----- Border state ----- */
                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
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
                     
                   
                     (*num_finals)++;

                        //printf("\n num finals = %d", *num_finals);
                    
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

                visitState_add(&visited, *s); // Ajouter à visited une fois ajouté dans exploring ou borders

             
            }

           
        }
        }

        (*num_visited)++;
        free(succs);
    }

    free(exploring);
    free(visited);
    return finals;
}

int EG_p_2tables_Memory_Layer(TA* ta, int location, DBM clock,
                 bool (*check)(State*s),
                 int  (*heuristique_check)(State*s))
{

    State* init_state = compute_init_state(ta);
        int num_borders = 0;
        int num_visited = 0;


    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init_state)) {
        free(init_state);
        printf("propriete non verifier dans init state");
        return 0;
    }

    int    init_weight = heuristique_check(init_state);

    StateWeight* visiting   = NULL;   /* à explorer  (frontier) */
    visit*       visited = NULL;   /* déjà expansés           */

    sw_add(&visiting, *init_state, init_weight);
    visit_add(&visited,  *init_state);
    //num_visited++;
    //num_borders++;
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
                                        &num_succ, &num_visited, check); // Appel a nexborcer memory
       
        
      
        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);

      
        if (boucle) {
            free(successors);
            sw_destroy(&visiting);
            visit_destroy(&visited);
            // printf("\n boucle");
             printf("\n nombre d etats vistes: %d", num_visited);
            return 1;
        }
      
        /* Cas général : on n'ajoute que les successeurs qui satisfont check */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
    
            /* un successor déja visité */
            if (visit_find(&visited, *s) != NULL)
                  continue;
          
           

            int w = heuristique_check(s);
           
            sw_add(&visiting, *s, w);
            visit_add(&visited, *s);
            num_borders ++;
            
        }

        free(successors);
    }

    /* open vide : aucun chemin infini satisfaisant trouvé */
    sw_destroy(&visiting);
    visit_destroy(&visited);

    printf("\n nombre d etats vistes: %d", num_visited);
    printf("\n nombre de border states = %d", num_borders);
   


     return 0 ;
}


/***********************************************************************************  No memory *************************************************************************************/
int EF_pNO_memory(TA* ta, int location, DBM clock,State** result,
         bool (*check)(State*s),
         int (*heuristique_check)(State*s)) {

    //if (!ta) return 0;
    int nbr_border_state =0;
    int num_found = 0; 
    bool   found      = false;
    State* init_state = compute_init_state(ta);
    //if (!init_state) return 0;
    *result= NULL;
   if (check(init_state)) {  
                        *result = init_state; 
                         printf("\n nombre d etats vistes: %d", 1);
                        return 1;
                    }
    StateWeight* visiting = NULL;  /* frontier  */
   // visit* visited  = NULL;  /* seen set  */

    int init_weight = heuristique_check(init_state);
    sw_add(&visiting, *init_state, init_weight);
 

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
                                        &num_succ, &found,&num_found, check);
        if (found) {
              *result = malloc(sizeof(State));
              **result = *successors;
            free(successors);
            sw_destroy(&visiting);
            printf("\n nombre d etats vistes: %d", num_found);
            return 1;
        }

        if (!successors) continue;
        // si on tombe sur le mm border state c une boucle il faut arréter
        bool boucle = (num_succ == 1) &&
                       equal_var(&current.var, &successors[0].var);
        if (boucle){
            if (check(&successors[0])) {
                    *result = malloc(sizeof(State));
                    **result = successors[0];
                
                    free(successors);

                    sw_destroy(&visiting);
                     printf("\n nombre d etats vistes: %d", num_found);
                    return 1;
                }
            else continue;
        }
        else 
        {
            for (int i = 0; i < num_succ; i++) {
                State* s = &successors[i];
               
                /* Skip if already seen */
               

                if (check(s)) {
                    *result = malloc(sizeof(State));
                    **result = *s;
                    free(successors);
                    sw_destroy(&visiting);
                    printf("\n nombre d etats vistes: %d", num_found);
                    return 1;
                }



                int w = heuristique_check(s);
                sw_add(&visiting, *s, w);
              
               

               
            }
        }

        free(successors);
    }

    sw_destroy(&visiting);
    printf("\n nombre d'états trouves dans le parcours : %d",num_found );

    return 0;
}

int EG_p_2tablesNo_memory(TA* ta, int location, DBM clock,
                 bool (*check)(State*s),
                 int  (*heuristique_check)(State*s))
{

    State* init_state = compute_init_state(ta);
    int num_visited = 0; 

    /* EG : l'état initial doit satisfaire la propriété */
    if (!check(init_state)) {
        free(init_state);
        printf("propriete non verifier dans init state");
        return 0;
    }

    int    init_weight = heuristique_check(init_state);

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
       
      
        State* successors = EGNextBorder(ta, current, location, clock,
                                        &num_succ, &num_visited, check);
       
        

        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);


        if (boucle) {
            free(successors);
            sw_destroy(&visiting);
           // visit_destroy(&visited);
           //  printf("\n boucle");
            printf("\n nombre d etats vistes: %d", num_visited);
            return 1;
        }
      
        /* Cas général : on n'ajoute que les successeurs qui satisfont check */
        for (int i = 0; i < num_succ; i++) {
            State* s = &successors[i];
            /* EG : inutile d'explorer un état qui viole la propriété */
            //  if (!check(s))
            //      continue;
            int w = heuristique_check(s);
            sw_add(&visiting, *s, w);    
        }
       
        free(successors);
    }

    /* open vide : aucun chemin infini satisfaisant trouvé */
   sw_destroy(&visiting);
   printf("\n nombre d etats vistes: %d", num_visited);

     return 0 ;
}

/***********************************************************************************  full memory *****************************************************************************/


int EF_FullMemory(TA* ta, int location, DBM clock, State** result,
         bool (*check)(State*s),
         int (*heuristique_check)(State*s))
{
    *result = NULL;
    int nbr_border =0;
    int num_visited = 0;
    State *init = compute_init_state(ta);
    if (!init) return 0;

    if (check(init)) {
        *result = init;
         printf("\n nombre d etats vistes: %d", 1);
        return 1;
    }

   // visitState* layer_visited = NULL;  /* table d'états visités */
    StateHash* layer_visited = NULL;  /* table d'états visités */
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
    exploring[tail++] = *init; // Ajouter l'état initial a exploring
   
     visitState_add(&layer_visited, *init); // Ajouter l'état initial a visited
     nbr_border++;
   //  num_visited++;
    free(init);

    while (true) { // cette boucle pour explorer les borders il s'arrete quad y'a pls de borders à explorer

        /* ---------- BFS depuis l'état courant ---------- */
        while (head < tail) { // meme boucle que next border

            State current = exploring[head++]; 

            if (check(&current)) {
                *result = malloc(sizeof(State));
                **result = current;
                free(exploring);
                free(finals);
                visitState_destroy(&layer_visited);
                 printf("\n nombre d etats vistes: %d", num_visited);
                return 1;
            }
            int num_succ = 0;
            State* succs = get_successors(ta, &current, &num_succ);
            for (int j = 0; j < num_succ; j++) {
                State* s = &succs[j];
                if (visitState_find(&layer_visited, *s)) continue; // skip si déja trouver
                  visitState_add(&layer_visited, *s); // Ajouter à visited une fois ajouté dans exploring ou finals
                  
 
                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
                {
                    /* ----- Border state ----- */
                  
                    nbr_border++;

                    if (num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        StateWeight* tmp = realloc(finals, capacity_finals * sizeof(StateWeight));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            visitState_destroy(&layer_visited);
                            return 0;
                        }
                        finals = tmp;
                    }
                    finals[num_finals].state  = *s;
                    finals[num_finals].weight = heuristique_check(s);
                    num_finals++;
                }
                else {
                    /* ----- État intermédiaire ----- */
                   

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
            num_visited++;

        } // Endwhile interieur (plus d'etats dans exploring)

        /* ---------- BFS épuisé : choisir le meilleur border state ---------- */
        if (num_finals == 0) break; // Si y'a plus d'états à explorer (l'exploration termine)


        /*------- Choisir le prochain border state à explorer selon le weight ----------- */
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
    
    }

    printf("\nnombre d'états dans les borders: %d", nbr_border);
    printf("\nnombre d'états visites: %d", num_visited);


    free(exploring);
    free(finals);
    visitState_destroy(&layer_visited);
    return 0;
}

int EG_FullMemory(TA* ta, int location, DBM clock,
         bool (*check)(State*s),
         int (*heuristique_check)(State*s))
{

  
    int nbr_border = 0;
    State *init = compute_init_state(ta);
    int num_visited = 0;
    if (!init) return 0;

    if (!check(init)) {
        printf( "actif de init state : %d",init->var.active);
        free(init);
        printf("\n propriete non verifiee dans init state");
        return 0;
    }

    // visitState* layer_visited  = NULL;  /* états intermédiaires — réinitialisé à chaque restart */
    // visitState* border_visited = NULL;  /* border states uniquement — jamais effacé             */

    StateHash* layer_visited  = NULL;  /* états intermédiaires — réinitialisé à chaque restart */
    StateHash* border_visited = NULL;  /* border states uniquement — jamais effacé             */

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
    visitState_add(&layer_visited, *init);

    /* Si l'état initial est lui-même un border state */
    if ((init->location == location) &&
        clock_zones_equal(init->clock_zone, clock, DBM_DIM))
    {
        visitState_add(&border_visited, *init);
        nbr_border++;
    }
    State next_state = exploring[0];
    free(init);
    bool last_layer = false;
    bool loop = false;
    while (true) {

        last_layer =true;
        /* ---------- BFS depuis l'état courant ---------- */
        while (head < tail) {

            State current = exploring[head++];
            if (!check(&current)) continue;

            int num_succ = 0;
            State* succs = get_successors(ta, &current, &num_succ);
            for (int j = 0; j < num_succ; j++) {
                 State* s = &succs[j];
                if (!equal_var( &next_state.var, &s->var)){ 
                   // printf("\n im not in last layer!");
                    //print_state(s,ta->locations);
                    last_layer= false;
                }
             
               
               
                if ((!check(s))) continue;
                

                if ((s->location == location) &&
                    clock_zones_equal(s->clock_zone, clock, DBM_DIM))
                {

                      /*-- verrifier si c une boucle*/
                    if (equal_var( &next_state.var, &s->var)){ 
                    printf("\n boucle");
                    printf("\n variable v= %d, active = %d, x= %d ", s->var.v, s->var.active, s->var.x);

                    if (check(s))
                    {
                        free(finals); free(exploring); free(succs);
                          
                          visitState_destroy(&layer_visited);
                          visitState_destroy(&border_visited);
                           printf("\n nombre d etats vistes: %d", num_visited);
                        return 1;

                    }else continue;
                   
                  }


                    /* ----- Border state ----- */
                    
                    if (visitState_find(&border_visited, *s) && !loop) continue;

                    visitState_add(&border_visited, *s);
                    //visitState_add(&layer_visited, *s);
                    nbr_border++;

                    if (num_finals >= capacity_finals) {
                        capacity_finals *= 2;
                        StateWeight* tmp = realloc(finals, capacity_finals * sizeof(StateWeight));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            visitState_destroy(&layer_visited);
                            visitState_destroy(&border_visited);
                            return 0;
                        }
                        finals = tmp;
                    }
                    finals[num_finals].state  = *s;
                    finals[num_finals].weight = heuristique_check(s);
                    num_finals++;
                }
                else {
                    /* ----- État intermédiaire ----- */
                    if (visitState_find(&layer_visited, *s) && !loop) continue;
                    visitState_add(&layer_visited, *s);

                    if (tail >= capacity) {
                        capacity *= 2;
                        State* tmp = realloc(exploring, capacity * sizeof(State));
                        if (!tmp) {
                            free(finals); free(exploring); free(succs);
                            visitState_destroy(&layer_visited);
                            visitState_destroy(&border_visited);
                            printf("\nErreur: Memoire depasse!!");
                            return 0;
                        }
                        exploring = tmp;
                    }
                    exploring[tail++] = *s;
                }
            }
            num_visited++;
            free(succs);
        }
        if (last_layer) {
             loop = true;
          // print_state(&next_state, ta->locations);

        } 


        /* ---------- BFS épuisé : choisir le meilleur border state ---------- */
        if (num_finals == 0){
        if (loop){
            //printf("\n loooop!!");
            //print_state(&next_state, ta->locations);
        }else
              break;

        }
      if (!loop){
        
        int min_idx = 0;
        for (int i = 1; i < num_finals; i++) {
            if (finals[i].weight < finals[min_idx].weight)
                min_idx = i;
        }

        next_state = finals[min_idx].state;
        finals[min_idx] = finals[num_finals - 1];
        num_finals--;

      }

        head = 0; tail = 0;
        exploring[tail++] = next_state;
        //last_layer = false;
    }

   printf("\n nombre d etats vistes: %d", num_visited);
   printf("\n nombre de border states: %d", nbr_border);
 
    free(exploring);
    free(finals);
    visitState_destroy(&layer_visited);
    visitState_destroy(&border_visited);
    return 0;
}

 /*==========================================  Fonction pour test   ===================*/

void print_all_exist(State_space_TA* ss_ta, TA* ta) {
    printf("\n les etats satisfaisants g:\n");
    for (int i = 0; i < ss_ta->nb_etats; i++) {
        if(check_p(&(ss_ta->etats[i]))){
               print_state(&(ss_ta->etats[i]),ta->locations);
                printf("Etat etendu ID #%d\n", i);
        }
       // printf("\n");
    }

}




