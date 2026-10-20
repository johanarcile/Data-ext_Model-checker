#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include "structure_variable.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"


// ----------------------Comparaison des variable----------------------------------
bool equal_var( Variable *v1,  Variable *v2) {

  
  
    if (v1->v != v2->v)
        {
       
            return false;
        }

  
        

    if (v1->active != v2->active)
        {
       
        return false;
      }

    
   

    return true;


    //return memcmp(v1, v2,sizeof(Variable)) == 0;
}

int compare_var(const Variable *v1, const Variable *v2) {

    if (v1->v < v2->v) return -1;
    if (v1->v > v2->v) return 1;
    // si ici donc v1.v == v2.V
    
        // si ici donc v1.v == v2.V et v1.x == v2.x
    if (v1->active < v2->active) return -1;
    if (v1->active > v2->active) return 1;
     // si ici donc v1.v == v2.V et v1.x == v2.x et  v1.active = v2.active 
    return 0;
}

// bool v1_inf_v2(Variable* v1, Variable* v2){
//      return (compare_var(v1, v2) < 0);
// }
// bool v1_sup_v2(Variable* v1, Variable* v2){
//     return compare_var(v1, v2)>0;
// }


void print_variable(Variable * v){

        printf("\t Variables       : v = %d\n", v->v);
        printf("\t Variables       : Active = %d\n", v->active);

       
       
}


/*==================properties checking functions pour resultats papier ===============================*/

bool check_p_var_true(Variable* V) {

   //9900000
      if ((V->v == 20000 )&&(V->active == true)) 
      return true;
    
      else return false;
}

bool check_p_var_false(Variable* V) {

   //9900000
      if ((V->v > 20000 )&&(V->active == false)) 
      return true;
    
      else return false;
}






/*==================properties checking functions ===============================*/



bool check_p_var(Variable* V) {

   //9900000
      if ((V->v == 20000 )&&(V->active == false)) 
      return true;
    
      else return false;
}



/*========================Heuristiques=============================================*/

 int heuristique_checkp_var(Variable* V){
    return abs(V->v - 20000);
 }






