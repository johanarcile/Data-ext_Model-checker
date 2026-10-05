#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "structure_state_space_ta.h"
#include "uthash.h"

/*======================== ready to compile pour resultats papier=============================================*/


bool check_true(State* s) {
    
    return check_p_var_true(&s->var);
}

bool check_false(State* s) {
    
    return check_p_var_false(&s->var);
}


/*======================== Check_p=============================================*/

bool check_p(State* s) {
    
    return check_p_var(&s->var);
}



/*========================Heuristiques=============================================*/

 int heuristique_checkp(State* s){
    return heuristique_checkp_var(&s->var);
 }