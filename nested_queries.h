#ifndef NESTED_QUERIES_H
#define NESTED_QUERIES_H

#include "structure_ta.h"
#include "uthash.h"
#include "structure_state_space_ta.h"

//======================== Pour les requetes imbriqués ===================================================

typedef bool (*CheckFunc)(State* s);
typedef int  (*HeuristicFunc)(State* s);


typedef struct {
    Variable       key;
   // State          state;
    int            mark;
    UT_hash_handle hh;
} mark;

 void mark_add(mark** table, State s, int w); 
 mark* mark_find(mark** table, State s);
 void mark_destroy(mark** table);

/*------------- fonctions imbriquees ------------------------------------------------------*/
/*

Les arguments de EGEF_p_2tables:
            ta : timed automata
            location : location de border state
            clock : horloge de border state
            check : la fonction check définissant la propriété p à satisfaire
            heuristique_check: l'heuristique utilisée pour guider l'exploration.

Les arguments de EFEF_pn_2tables et EFEG_pn:
            ta : timed automata
            location : location de border state
            clock : horloge de border state
            nbr_prop: nbr de propriétés recherchés
            check : liste des fonctions check définissant les propriétés p_i à satisfaire
            heuristique_check: liste d'heuristiques utilisées pour guider l'exploration ( chaque p_i a sa propre heuristique).

Note :
    Rappelle EFEG_pn verifie:  EF(P0 && EG(P1 && EG(P2 .......&&EG(Pn))))
    EFEG_pn a que deux propriétés, il faut trouver une execution : check[0] -> check[1].....check[1].ou:
                                    check[O] = P0 &&....Pn
                                et  check[1] = P1 &&....Pn

*/

int EGEF_p_2tables(TA* ta, int location, DBM clock,
                 bool (*check)(State* s),
                 int  (*heuristique_check)(State* s));

int EFEF_pn_2tables(TA* ta, int location, DBM clock, int nbr_prop,
                    CheckFunc*     check,              /* check[i] teste la propriete i */
                    HeuristicFunc* heuristique_check);

int EFEG_pn(TA* ta, int location, DBM clock,
            CheckFunc*     check,              /* check[0], check[1] */
            HeuristicFunc* heuristique_check) ;  /* heuristique_check[0], [1] */
                   
                   
#endif