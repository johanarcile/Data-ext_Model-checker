#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include <time.h>

// Déclarations manuelles de la fonction de construction du modèle
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
 


printf("\n ======================================== Les tests =====================================");

/*--------------------------- building state space ----------------------------*/
// printf("\n ======================================== building state space=====================================");

// for(int i=0; i< 3; i++) {

     
//     debut = clock(); 
//     build_state_space_ta(&ta, &state_space_ta);
//     fin = clock();
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n temps de constructiond d espace d etats : %f", temps_ecoule);
//     printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
//      tcee = tcee + temps_ecoule;


// }

     
/*--------------------------- EF(p) true----------------------------*/

printf("\n ======================================== EF(false) =====================================");

for(int i=0; i< 3; i++) {

 
  printf("\n ---------------- Test n: %d ----------------------------------------------------------",i);


//    printf("\n \n ****************EF No memory:****************** \n ");
 
//     debut = clock(); 
//     c = EF_pNO_memory(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
//     printf("\n trouver Avec  EFP 2 tables No emory? : %s ", c? "true" : "false \n");
//     if (result != NULL){
//          printf("\n Le state qui verifie\n");
//          print_state(result, ta.locations);
//          free (result);
//     }

//     tnmef = tnmef + temps_ecoule;


//    printf("\n EF *********Memory on the borders only*************: \n ");

//   debut = clock(); 
//     c = EF_p_1table(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFP 1 table de hashage: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  1 table de hashage? : %s ", c? "true" : "false  \n");
    
//     if (result != NULL){
//          printf("\n Le state qui verifie\n");
//          print_state(result, ta.locations);
//          free (result);
//     }
//    tmbef1 = tmbef1+ temps_ecoule;



    debut = clock(); 
    c = EF_p(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }
     tmbef = tmbef+ temps_ecoule;




     debut = clock(); 
    c = EF_p_HV(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP HEAP ET TABLE : %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
   
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }

     tmbhpef = tmbhpef+ temps_ecoule;

    debut = clock(); 
    c = EF_p_HV_M(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP heap pool juse maloc au besoins: %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
    
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }
   tmbhmef = tmbhmef+ temps_ecoule;

   

    
 printf("\n \n **************EF memory in layers:******************* \n ");
 
    debut = clock(); 
    c = EF_p_Memory_in_Layer(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables Memory in layers ? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }

     tmlef =  tmlef + temps_ecoule; 


   printf("\n \n **********************EF Full memory************************: \n ");
 
    debut = clock(); 
    c = EF_FullMemory(& ta,init_state->location,init_state->clock_zone, &result, check_false,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables full emory? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }

    tmfef = tmfef + temps_ecoule;


}


printf("\n ======================================== sumurry =====================================");
//printf("\n space state construction :  temps :  %f  nbr visite: %d", tcee /3,state_space_ta.nb_etats );

printf("\n ***************************Resultats pour EF(p) true :************************************** ");
printf("\n Only Essential States (OES) :  temps :  %f ", tnmef /3 );
printf("\n All Border States 1 table :  temps %f ", tmbef1 /3 );
printf("\n All Border States 2 tables :  temps %f ", tmbef /3 );
printf("\n All Border States HEAP ET TABLE :  temps %f ", tmbhpef/3);
printf("\n All Border States  HEAP ET TABLE juse maloc au besoins :  temps %f ", tmbhmef/3 );
printf("\n All Border States and Current Layer :  temps %f ", tmlef/3 );
printf("\n  All States :  temps %f  ", tmfef/3 );
    
}

