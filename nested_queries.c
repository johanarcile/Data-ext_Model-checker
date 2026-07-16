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
#include"nested_queries.h"



State* EFEGNextBorder(TA* ta, State state, int location, DBM clock,
                   int* num_finals, bool* found, bool (*check)(State* s))
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








/*========================= Requetes imbriquees========================================================================================================*/

 /*==========================================  EFEG(p) avec 2 tabeles de hashage   ===================*/

int EGEF_p_2tables(TA* ta, int location, DBM clock,
                 bool (*check)(State* s),
                 int  (*heuristique_check)(State* s))
{

    State* init_state = compute_init_state(ta);


    int    init_weight = heuristique_check(init_state);

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
                                        &num_succ, &found, check);
       
        
        /*  boucle sur soi-même → chemin infini trouvé */
         bool boucle = (num_succ == 1) && equal_var(&current.var, &successors[0].var);


        if (boucle) {  

            if  (check(&current)){ // si la propriete est verifié

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
            // if (!check(s, goal))
            //     continue;

           
           
            /* un successor déja visité */
            if (visit_find(&visited, *s) != NULL)
            continue;
           

            int w = heuristique_check(s);
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
                      CheckFunc* props, int nbr_prop,
                      int* num_finals, int current_prop,
                      int** border_props_out)
{

    int capacity = 32, head = 0, tail = 0;
    State* exploring       = malloc(capacity * sizeof(State));
    int*   exploring_props = malloc(capacity * sizeof(int));
    State* finals          = malloc(capacity * sizeof(State));
    int*   finals_props    = malloc(capacity * sizeof(int));
    *num_finals = 0;

    if (!exploring || !exploring_props || !finals || !finals_props) {
        free(exploring); free(exploring_props);
        free(finals);    free(finals_props);
        printf("\n Erreur malloc!!!");
        return NULL;
    }

    int start_prop = current_prop;
    while (start_prop < nbr_prop && props[start_prop](&state))
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
            int s_prop = cur_prop;
            while (s_prop < nbr_prop && props[s_prop](s))
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





int EFEF_pn_2tables(TA* ta, int location, DBM clock, int nbr_prop,
                    CheckFunc*     check,              /* check[i] teste la propriete i */
                    HeuristicFunc* heuristique_check)  /* heuristique_check[i] pour la propriete i */
{
    State* init_state = compute_init_state(ta);

    /* Calculer jusqu'à quelle propriété l'état initial satisfait check[0..nbr_prop-1] */
    int init_prop = 0;
    while (init_prop < nbr_prop && check[init_prop](init_state))
        init_prop++;

    if (init_prop == nbr_prop) {
        printf("\n Toutes les proprietes sont verifiees ! \n");
        free(init_state);
        return 1;
    }

    StateWeight* visiting = NULL;
    mark*        marked   = NULL;

    int init_weight = heuristique_check[init_prop](init_state);
    sw_add(&visiting, *init_state, init_weight);
    mark_add(&marked, *init_state, init_prop);
    free(init_state);

    int num_succ = 0;

    while (HASH_COUNT(visiting) > 0) {
        StateWeight *best = NULL, *cur, *tmp;
        int best_mark = -1;

        HASH_ITER(hh, visiting, cur, tmp) {
            mark* m = mark_find(&marked, cur->state);
            if (m && m->mark > best_mark)
                best_mark = m->mark;
        }

        HASH_ITER(hh, visiting, cur, tmp) {
            mark* m = mark_find(&marked, cur->state);
            if (m && m->mark == best_mark) {
                if (best == NULL || cur->weight < best->weight)
                    best = cur;
            }
        }

        State current      = best->state;
        int   current_prop  = best_mark;
        HASH_DEL(visiting, best);
        free(best);

        /* --- Appel NextBorder --- */
        int*   border_props = NULL;
        State* successors   = EFPnNextBorder(ta, current, location, clock,
                                             check, nbr_prop, &num_succ,
                                             current_prop, &border_props);

        /* --- Cas self-loop --- */
        // bool boucle = (num_succ == 1) &&
        //               equal_var(&current.var, &successors[0].var);

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

            mark* existing = mark_find(&marked, *s);
            if (existing != NULL && existing->mark >= prop)
                continue;

            mark_add(&marked, *s, prop);
            int w = heuristique_check[prop](s);
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
                         CheckFunc* check, int nbr_prop,
                         int* num_finals, int current_prop,
                         int** border_props_out)
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

    if (check[0](&state))
        s_prop = 0;
    else if ((current_prop == 0 || current_prop == 1) && check[1](&state))
        s_prop = 1;

    exploring[tail]       = state;
    exploring_props[tail] = s_prop;
    tail++;
    

    while (head < tail) {

        State current  = exploring[head];
        int   cur_prop = exploring_props[head];
        head++;

        int    num_succ = 0;
        State* succs    = get_successors(ta, &current, &num_succ);

        for (int j = 0; j < num_succ; j++) {
            State* s = &succs[j];

            if (check[0](s))
                s_prop = 0;
            else if ((cur_prop == 0 || cur_prop == 1) && check[1](s))
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

int EFEG_pn(TA* ta, int location, DBM clock,
            CheckFunc*     check,              /* check[0], check[1] */
            HeuristicFunc* heuristique_check)   /* heuristique_check[0], [1] */
{
    int  nbr_border_state = 0;

    State* init_state = compute_init_state(ta);
    int current_prop = 2;
    if (check[0](init_state)) {
        current_prop = 0;
    }

    StateWeight* visiting = NULL;
    mark*        marked   = NULL;

    int init_weight = heuristique_check[current_prop < 2 ? current_prop : 0](init_state);
    sw_add(&visiting, *init_state, init_weight);
    mark_add(&marked, *init_state, current_prop);
    nbr_border_state++;
    free(init_state);

    while (HASH_COUNT(visiting) > 0) {

        StateWeight *best = NULL, *cur, *tmp;
        HASH_ITER(hh, visiting, cur, tmp) {
            if (best == NULL || cur->weight < best->weight)
                best = cur;
        }

        State current = best->state;
        mark* m = mark_find(&marked, best->state);
        current_prop = m->mark;

        HASH_DEL(visiting, best);
        free(best);

        int*   border_props = NULL;
        int    num_succ     = 0;
        State* successors   = EFEG_pnNextBorder(ta, current, location, clock,
                                                check, 2, &num_succ,
                                                current_prop, &border_props);
        if (!successors) continue;

        bool boucle = (num_succ == 1)
                   && equal_var(&current.var, &successors[0].var);

        if (boucle) {
            if (current_prop == 0 || current_prop == 1) {
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
                if (existing->mark == 0) continue;
                if (border_props[i] < existing->mark)
                    existing->mark = border_props[i];
                int w = heuristique_check[border_props[i] < 2 ? border_props[i] : 0](s);
                sw_add(&visiting, *s, w);
                continue;
            }

            mark_add(&marked, *s, border_props[i]);
            int w = heuristique_check[border_props[i] < 2 ? border_props[i] : 0](s);
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