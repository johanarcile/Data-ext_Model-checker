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
int EGEF_p_2tables(TA* ta, int location, DBM clock,
                 bool (*check)(State* s),
                 int  (*heuristique_check)(State* s));

int EFEF_pn_2tables(TA* ta, int location, DBM clock, int nbr_prop,
                    CheckFunc*     check,              /* check[i] teste la propriete i */
                    HeuristicFunc* heuristique_check);

int EFEG_pn(TA* ta, int location, DBM clock,
            CheckFunc*     check,              /* check[0], check[1] */
            HeuristicFunc* heuristique_check) ;  /* heuristique_check[0], [1] */

            
//GoalCondition* build_EFEG_goals(GoalCondition* props, int nbr_prop);


// int EFPn_recursive(TA* ta, int location, DBM clock,
//                     int nbr_prop,
//                     bool (*check)(State* s),
//                    int  (*heuristique_check)(State* s));
                   
                   
                   
#endif