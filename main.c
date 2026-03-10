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



    State* init_state = compute_init_state(&ta);
   // print_state(init_state,ta.locations);
 
 /*--------------------------- Goal cherche ----------------------------*/
    GoalCondition g;
   // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask = CHECK_V  ; 
    g.active = true;
    g.v = 0;

    strcpy(  g.name, "transition b");

 
 /*--------------------------- calculer nbr etats ----------------------------*/

    // debut = clock(); 
    // build_state_space_ta(&ta, &state_space_ta);
    // fin = clock();
    // temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    // printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
    // printf("\n Temps d execution : %f secondes\n", temps_ecoule);

 /*--------------------------- EF(p) ----------------------------*/
     
    debut = clock(); 
    c = EF_p(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }
  

     debut = clock(); 
    c = EF_p_HV(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP HEAP ET TABLE : %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
   
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }


    debut = clock(); 
    c = EF_p_HV_M(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP heap pool juse maloc au besoins: %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
    
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }
    
 /*--------------------------- EG(p) ----------------------------*/

    debut = clock(); 
    c = EG_p_2tables(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");


     debut = clock(); 
    c = EG_p_HV_M(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP heap pool juse maloc au besoins: %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");

   
    
 /*--------------------------- Exploration espace etats ----------------------------*/
//    // explore_state_space_ta(&ta);
//     build_state_space_ta(&ta, &state_space_ta);
//    // print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
//     printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);



 /*--------------------------- test ----------------------------*/
//    // explore_state_space_ta(&ta);
    // build_state_space_ta(&ta, &state_space_ta);
    // print_all_exist(&state_space_ta, &ta, &g);



    return 0;
    
}

