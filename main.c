#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include <time.h>

// Déclarations manuelles de la fonction de construction du modèle


void fill_ta_struct(TA* ta);


int main() {
    TA ta;
    fill_ta_struct(&ta);
    State* init_state = compute_init_state(&ta);
    print_state(init_state,ta.locations);
 

    GoalCondition g;
   // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask = CHECK_V; //| CHECK_ACTIVE;
    g.active = true;
    g.v = 501;
    strcpy(  g.name, "transition b");

    clock_t debut, fin;
     double temps_ecoule;

//     State_space_TA state_space_ta;
//    debut = clock(); 
//     build_state_space_ta(&ta, &state_space_ta);
//      fin = clock();
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
//     printf("\n Temps d execution : %f secondes\n", temps_ecoule);


     
    debut = clock(); 
    int c = EF_p(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables? : %s ", c? "true" : "false \n");


     debut = clock(); 
    c = EF_p_HV(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP HEAP ET TABLE : %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");

     debut = clock(); 
    c = EF_p_HV_Pool(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP heap pool: %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
  
   




    // explore_state_space_ta(&ta);
    

    // State_space_TA state_space_ta;
    // build_state_space_ta(&ta, &state_space_ta);
    // print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
    // printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);
    // return 0;
    
}

