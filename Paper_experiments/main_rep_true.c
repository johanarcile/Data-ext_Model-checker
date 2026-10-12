#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include <time.h>

// Manual declaration of the function that builds the model
void fill_ta_struct(TA* ta);



int main() {

    TA ta;

    clock_t debut, fin;
    double temps_ecoule;
    int c;
    State * result;


    State_space_TA state_space_ta;
    fill_ta_struct(&ta);

    double tnmef = 0, tmbef1 = 0, tmbef = 0,  tmbhpef = 0,  tmbhmef = 0, tmlef = 0,  tmfef = 0,  tcee=0;





     State* init_state = compute_init_state(&ta);
     //print_state(init_state,ta.locations);



printf("\n==============================================================================");
printf("\n  EXPERIMENTS: EF(p) where p is NOT satisfied");
printf("\n==============================================================================\n");

/*--------------------------- building state space ----------------------------*/
printf("\n------------------------------------------------------------------------------");
printf("\n  State space construction");
printf("\n------------------------------------------------------------------------------\n");

// for(int i=0; i< 3; i++) {


//     debut = clock();
//     build_state_space_ta(&ta, &state_space_ta);
//     fin = clock();
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n  Construction time         : %f s", temps_ecoule);
//     printf("\n  Total number of extended states : %d\n", state_space_ta.nb_etats);
//      tcee = tcee + temps_ecoule;


// }


/*--------------------------- EF(p) not satisfied ----------------------------*/

for(int i=0; i< 3; i++) {


  printf("\n------------------------------------------------------------------------------");
  printf("\n  Run %d of 3", i + 1);
  printf("\n------------------------------------------------------------------------------\n");


   printf("\n ---------[1] Only Essential States (OES)-------------\n");


    debut = clock();
    c = OES(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
    fin = clock();            // Stop the timer
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("  \n Execution time           : %f s\n", temps_ecoule);
    printf("  State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }

    tnmef = tnmef + temps_ecoule;


   printf("\n ---------[2] All Border States (ABS)-------------\n");

//    printf("\n  Variant: 1 hash table\n");
//     debut = clock();
//     c = ABS_1table(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
//     fin = clock();            // Stop the timer
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n  Execution time           : %f s\n", temps_ecoule);
//     printf("  State satisfying p found : %s\n", c ? "yes" : "no");

//     if (result != NULL){
//          printf("  Satisfying state:\n");
//          print_state(result, ta.locations);
//          free (result);
//     }
//    tmbef1 = tmbef1+ temps_ecoule;


   printf("\n  Variant: 2 hash tables\n");
    debut = clock();
    c = ABS_2tables(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
    fin = clock();            // Stop the timer
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }
     tmbef = tmbef+ temps_ecoule;




//    printf("\n  Variant: heap and hash table\n");
//      debut = clock();
//     c = EF_p_HV(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
//     fin = clock();            // Stop the timer
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("  \n Execution time           : %f s\n", temps_ecoule);
//     printf("  State satisfying p found : %s\n", c ? "yes" : "no");

//     if (result != NULL){
//          printf("  Satisfying state:\n");
//          print_state(result, ta.locations);
//          free (result);
//     }

//      tmbhpef = tmbhpef+ temps_ecoule;

   printf("\n  Variant: hash table and min-heap (memory allocated on demand)\n");
    debut = clock();
    c = ABS_table_and_minheap(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
    fin = clock();            // Stop the timer
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");

    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }
   tmbhmef = tmbhmef+ temps_ecoule;




   printf("\n ---------[3] All Border States and Current Layer-------------\n");

    debut = clock();
    c = ABS_and_current_layer(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
    fin = clock();            // Stop the timer
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }

     tmlef =  tmlef + temps_ecoule;

   printf("\n ---------[4] All States (AS)-------------\n");


    debut = clock();
    c = AS(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp);
    fin = clock();            // Stop the timer
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }

    tmfef = tmfef + temps_ecoule;


}



printf("\n==============================================================================");
printf("\n  SUMMARY: EF(p) where p is NOT satisfied (average over 3 runs)");
printf("\n==============================================================================\n");
printf("\n  State space construction : %f s (%d states)", tcee / 3, state_space_ta.nb_etats);

printf("\n  %-52s %s\n", "Algorithm", "Avg. time (s)");
printf("  %-52s %s\n", "----------------------------------------------------", "-------------");
printf("  %-52s %13f\n", "Only Essential States (OES)", tnmef / 3);
printf("  %-52s %13f\n", "All Border States - 1 table", tmbef1 / 3);
printf("  %-52s %13f\n", "All Border States - 2 tables", tmbef / 3);
// printf("  %-52s %13f\n", "All Border States - heap and table", tmbhpef / 3);
printf("  %-52s %13f\n", "All Border States - table and min-heap", tmbhmef / 3);
printf("  %-52s %13f\n", "All Border States and Current Layer", tmlef / 3);
printf("  %-52s %13f\n", "All States", tmfef / 3);
printf("\n");

}