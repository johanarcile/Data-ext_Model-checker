#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include <time.h>
#include <windows.h>
#include <psapi.h>
#include <malloc.h>

// Time limit per execution (the algorithms return -1 when it is exceeded).
// Must be the same value as in the algorithms (see README.md).
#ifndef TIMEOUT_SECONDS
#define TIMEOUT_SECONDS 60
#endif

// Manual declaration of the function that builds the model
void fill_ta_struct(TA* ta);

// Summary line: execution time, or TIME OUT if the execution exceeded the limit
static void print_row(const char *name, double total, int nb_ok) {
    if (nb_ok > 0)
        printf("  %-52s %13f\n", name, total / nb_ok);
    else
        printf("  %-52s %13s\n", name, "TIME OUT");
}



int main() {

    TA ta;

    clock_t debut, fin;
    double temps_ecoule;
    int c;
    State * result;


    State_space_TA state_space_ta;
    fill_ta_struct(&ta);

    double tnmef = 0, tmbef1 = 0, tmbef = 0,  tmbhpef = 0,  tmbhmef = 0, tmlef = 0,  tmfef = 0,  tcee=0;

    // number of runs that finished before the time limit (timed-out runs are ignored)
    int n_nmef = 0, n_mbef1 = 0, n_mbef = 0, n_mbhmef = 0, n_mlef = 0, n_mfef = 0;





     State* init_state = compute_init_state(&ta);
     //print_state(init_state,ta.locations);



printf("\n==============================================================================");
printf("\n  EXPERIMENTS: EF(p) where p is NOT satisfied");
printf("\n  Time limit per execution: %d s (an execution exceeding it is interrupted)", TIMEOUT_SECONDS);
printf("\n==============================================================================\n");

/*--------------------------- building state space ----------------------------*/
// printf("\n------------------------------------------------------------------------------");
// printf("\n  State space construction");
// printf("\n------------------------------------------------------------------------------\n");

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
// PROCESS_MEMORY_COUNTERS_EX m;
// GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&m, sizeof(m));
// printf("Memoire engagee : %.0f Mo | pic RAM : %.0f Mo\n",
//        m.PagefileUsage / 1048576.0,
//        m.PeakWorkingSetSize / 1048576.0);
// printf("Memoire privee : %.0f Mo\n", m.PrivateUsage / 1048576.0);

// for(int i=0; i< 3; i++) {


//   printf("\n------------------------------------------------------------------------------");
//   printf("\n  Run %d of 3", i + 1);
//   printf("\n------------------------------------------------------------------------------\n");


   printf("\n ---------[1] Only Essential States (OES)-------------\n");


    debut = clock();
    c = OES(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp,TIMEOUT_SECONDS);
    fin = clock();            // Stop the timer

    if (c == -1){printf("\n  TIME OUT (> %d s): execution interrupted\n", TIMEOUT_SECONDS);}
    else{
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;

    printf("  \n Execution time           : %f s\n", temps_ecoule);
    printf("  State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }

    tnmef = tnmef + temps_ecoule;
    n_nmef++;
}


   printf("\n ---------[2] All Border States (ABS)-------------\n");

   printf("\n  Variant: 1 hash table\n");
    debut = clock();
    c = ABS_1table(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp,TIMEOUT_SECONDS);
    fin = clock();            // Stop the timer
    if (c == -1){printf("\n  TIME OUT (> %d s): execution interrupted\n", TIMEOUT_SECONDS);}
    else{
     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
     printf("\n  Execution time           : %f s\n", temps_ecoule);
     printf("  State satisfying p found : %s\n", c ? "yes" : "no");

          if (result != NULL){
               printf("  Satisfying state:\n");
               print_state(result, ta.locations);
               free (result);
          }
          tmbef1 = tmbef1+ temps_ecoule;
          n_mbef1++;

    }

   


   printf("\n  Variant: 2 hash tables\n");
    debut = clock();
    c = ABS_2tables(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp,TIMEOUT_SECONDS);
    fin = clock();            // Stop the timer

    if (c == -1){printf("\n  TIME OUT (> %d s): execution interrupted\n", TIMEOUT_SECONDS);}
    else{
          temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
          printf("\n Execution time           : %f s\n", temps_ecoule);
          printf("State satisfying p found : %s\n", c ? "yes" : "no");
          if (result != NULL){
               printf("  Satisfying state:\n");
               print_state(result, ta.locations);
               free (result);
          }
               tmbef = tmbef+ temps_ecoule;
               n_mbef++;
    }

//      GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&m, sizeof(m));
// printf("Memoire engagee : %.0f Mo | pic RAM : %.0f Mo\n",
//        m.PagefileUsage / 1048576.0,
//        m.PeakWorkingSetSize / 1048576.0);
// printf("Memoire privee : %.0f Mo\n", m.PrivateUsage / 1048576.0);




   printf("\n  Variant: hash table and min-heap \n");
    debut = clock();
    c = ABS_table_and_minheap(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp,TIMEOUT_SECONDS);
    fin = clock();            // Stop the timer
    if (c == -1){printf("\n  TIME OUT (> %d s): execution interrupted\n", TIMEOUT_SECONDS);}
    else{
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");

    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }
   tmbhmef = tmbhmef+ temps_ecoule;
   n_mbhmef++;}




   printf("\n ---------[3] All Border States and Current Layer-------------\n");

    debut = clock();
    c = ABS_and_current_layer(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp,TIMEOUT_SECONDS);
    fin = clock();            // Stop the timer
    if (c == -1){printf("\n  TIME OUT (> %d s): execution interrupted\n", TIMEOUT_SECONDS);}
    else{
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }

     tmlef =  tmlef + temps_ecoule;
     n_mlef++;}

 
   printf("\n ---------[4] All States (AS)-------------\n");


    debut = clock();
    c = AS(& ta,init_state->location,init_state->clock_zone, &result, check_true,heuristique_checkp,TIMEOUT_SECONDS);
    fin = clock();            // Stop the timer
    if (c == -1){printf("\n  TIME OUT (> %d s): execution interrupted\n", TIMEOUT_SECONDS);}
    else{
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Execution time           : %f s\n", temps_ecoule);
    printf("State satisfying p found : %s\n", c ? "yes" : "no");
    if (result != NULL){
         printf("  Satisfying state:\n");
         print_state(result, ta.locations);
         free (result);
    }

    tmfef = tmfef + temps_ecoule;
    n_mfef++;}



//}


// _heapmin();
// Sleep(5000);
// GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&m, sizeof(m));
// printf("Memoire engagee : %.0f Mo | pic RAM : %.0f Mo\n",
//        m.PagefileUsage / 1048576.0,
//        m.PeakWorkingSetSize / 1048576.0);
// printf("Memoire privee : %.0f Mo\n", m.PrivateUsage / 1048576.0);

printf("\n==============================================================================");
printf("\n  SUMMARY: EF(p) where p is NOT satisfied");
printf("\n  (execution time, time limit: %d s)", TIMEOUT_SECONDS);
printf("\n==============================================================================\n");
//printf("\n  State space construction : %f s (%d states)", tcee / 3, state_space_ta.nb_etats);

printf("\n  %-52s %s\n", "Algorithm", "Time (s)");
printf("  %-52s %s\n", "----------------------------------------------------", "-------------");
print_row("Only Essential States (OES)", tnmef, n_nmef);
print_row("All Border States - 1 table", tmbef1, n_mbef1);
print_row("All Border States - 2 tables", tmbef, n_mbef);
// print_row("All Border States - heap and table", tmbhpef, n_mbhpef);
print_row("All Border States - table and min-heap", tmbhmef, n_mbhmef);
print_row("All Border States and Current Layer", tmlef, n_mlef);
print_row("All States", tmfef, n_mfef);
printf("\n");

}