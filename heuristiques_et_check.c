#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "structure_state_space_ta.h"
#include "uthash.h"


bool check_p(State* s) {

   
     
    
    return check_p_var(&s->var);
}



/*========================Heuristiques=============================================*/

 int heuristique_checkp(State* s){
    return heuristique_checkp_var(&s->var);
 }