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
    printf("\n location: %d",init_state->location);
    printf("\n horloge: %d",init_state->clock_zone);

    print_state(init_state,ta.locations);
 

    GoalCondition g;
   // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask = CHECK_V; //| CHECK_ACTIVE;
    g.active = true;
    g.v = 101;
    strcpy(  g.name, "transition b");


     clock_t debut, fin;
     double temps_ecoule;
    debut = clock(); 
    int c = EF_p(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
 temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps d execution : %f secondes\n", temps_ecoule);
    

   printf("Temps d execution : %f secondes\n", temps_ecoule); 
   State_space_TA state_space_ta;
   debut = clock(); 
    build_state_space_ta(&ta, &state_space_ta);
     fin = clock();
       temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);
     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Temps d execution : %f secondes\n", temps_ecoule);
    printf("\n trouver? : %s ", c? "true" : "false");





    // explore_state_space_ta(&ta);
    

    // State_space_TA state_space_ta;
    // build_state_space_ta(&ta, &state_space_ta);
    // print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
    // printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);
    return 0;
    
}

