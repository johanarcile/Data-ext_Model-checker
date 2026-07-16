#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include"nested_queries.h"
#include <time.h>

// Déclarations manuelles de la fonction de construction du modèle
void fill_ta_struct(TA* ta);



bool check_prop0(State* s) {
   //  if(s->var.v == 0) printf("\n prop0 verifie");
    return s->var.v == 0;
   
}

bool check_prop1(State* s) {
    // if(s->var.v == 9900000) printf("\n prop1 verifie");
    return s->var.v <= 9900000;
     
}

CheckFunc checks[] = { check_prop0, check_prop1 };

int heuristique_prop0(State* s) {
    return abs(s->var.v - 0);
}

int heuristique_prop1(State* s) {
    return abs(s->var.v - 9900000);
}

HeuristicFunc heuristics[] = { heuristique_prop0, heuristique_prop1 };

int main() {
    TA ta;
    clock_t debut, fin;
    double temps_ecoule;
    int c;
    State * result;
    State_space_TA state_space_ta;
    fill_ta_struct(&ta);
    
    double tnmef = 0,tmbef = 0,  tmbhpef = 0,  tmbhmef = 0, tmlef = 0,  tmfef = 0, 
           tnmeg = 0,tmbeg = 0 ,   tmfeg = 0,  tmleg = 0, tmbhmeg = 0, tmbhpeg = 0, tcee=0;
    int   nbr_visit_efnom = 0,nbr_visit_efmb =0,nbr_visit_efmbhp= 0, nbr_visit_efmbhm = 0,nbr_visit_efml = 0,nbr_visit_efmf =0,  
    nbr_visit_egnom =0 ,  nbr_visit_egmb = 0,  nbr_visit_egmbhp = 0, nbr_visit_egmbhm = 0,nbr_visit_egml=0, nbr_visit_egmf = 0;
  




     State* init_state = compute_init_state(&ta);
//     //print_state(init_state,ta.locations);
 


 
 /*--------------------------- calculer nbr etats ----------------------------*/

//     debut = clock(); 
//     build_state_space_ta(&ta, &state_space_ta);
//     fin = clock();
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
//     printf("\n Temps d execution : %f secondes\n", temps_ecoule);




printf("\n ======================================== Les tests =====================================");

//  for(int i=0; i< 3; i++) {

 

//  printf("\n ---------------- Test n: %d ----------------------------------------------------------",i);




//     debut = clock(); 
//     build_state_space_ta(&ta, &state_space_ta);
//     fin = clock();
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n temps de constructiond d espace d etats : %f", temps_ecoule);
//     printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
//      tcee = tcee + temps_ecoule;
      /*--------------------------- EF(p) ----------------------------*/



 /*--------------------------- memory in borders-------------------*/
  printf("\n \n ------------------EF:-------------------------- \n ");
   printf("\n EF *********Memory on the borders only*************: \n ");
    debut = clock(); 
    c = EF_p(& ta,init_state->location,init_state->clock_zone, &result, check_p,heuristique_checkp);
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
    c = EF_p_HV(& ta,init_state->location,init_state->clock_zone, &result, check_p,heuristique_checkp);
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
    c = EF_p_HV_M(& ta,init_state->location,init_state->clock_zone, &result, check_p,heuristique_checkp);
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
    c = EF_p_Memory_in_Layer(& ta,init_state->location,init_state->clock_zone, &result, check_p,heuristique_checkp);
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
    c = EF_FullMemory(& ta,init_state->location,init_state->clock_zone, &result, check_p,heuristique_checkp);
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


 printf("\n \n ****************EF No memory:****************** \n ");
 
    debut = clock(); 
    c = EF_pNO_memory(& ta,init_state->location,init_state->clock_zone, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables No emory? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }

    tnmef = tnmef + temps_ecoule;



  

//    //   printf("\n \n ------------------EG:-------------------------- \n ");
//      printf("\n EG *********Memory on the borders only*************: \n ");
//     debut = clock(); 
//     c = EG_p_2tables(& ta,init_state->location,init_state->clock_zone,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
//     tmbeg = tmbeg + temps_ecoule;



 

//      debut = clock(); 
//     c = EG_p_HV_M(& ta,init_state->location,init_state->clock_zone,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP heap pool juse maloc au besoins: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
//     tmbhmeg = tmbhmeg + temps_ecoule;

//   printf("\n \n **************EG memory in layers:******************* \n ");

//     debut = clock(); 
//     c = EG_p_2tables_Memory_Layer(& ta,init_state->location,init_state->clock_zone,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");

//     tmleg = tmleg + temps_ecoule;

//   printf("\n \n **********************EG Full memory************************: \n ");
   
//     debut = clock(); 
//     c = EG_FullMemory(& ta,init_state->location,init_state->clock_zone,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables full memory: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
//     tmfeg = tmfeg + temps_ecoule;

    
//      printf("\n \n ****************EG No memory:****************** \n ");

//     debut = clock(); 
//     c = EG_p_2tablesNo_memory(& ta,init_state->location,init_state->clock_zone,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
//     tnmeg = tnmeg + temps_ecoule;






    
//}


printf("\n ======================================== sumurry =====================================");
printf("\n space state construction :  temps :  %f  nbr visite: %d", tcee /3,state_space_ta.nb_etats );

printf("\n ***************************Resultats pour EF(p) :************************************** ");
printf("\n No memory :  temps :  %f  nbr visite: %d", tnmef /3,nbr_visit_efnom );
printf("\n memory in borders only  2 tables :  temps %f  nbr visite : %d", tmbef /3,nbr_visit_efmb );
printf("\n memory in borders only  HEAP ET TABLE :  temps %f  nbr visite: %d", tmbhpef/3,nbr_visit_efmbhp );
printf("\n memory in borders only  HEAP ET TABLE juse maloc au besoins :  temps %f  nbr visite: %d", tmbhmef/3,nbr_visit_efmbhm );
printf("\n memory in borders + current layer :  temps %f  nbr visite: %d", tmlef/3,nbr_visit_efml );
printf("\n memory in full memory :  temps %f  nbr visite: %d", tmfef/3,nbr_visit_efmf );

printf("\n ***************************Resultats pour EG(p) :************************************** ");
printf("\n No memory :  temps :  %f  nbr visite: %d", tnmeg/3,nbr_visit_egnom );
printf("\n memory in borders only  2 tables :  temps %f  nbr visite: %d", tmbeg/3,nbr_visit_egmb );
printf("\n memory in borders only  HEAP ET TABLE juse maloc au besoins :  temps %f  nbr visite: %d", tmbhmeg/3,nbr_visit_egmbhm );
printf("\n memory in borders + current layer :  temps %f  nbr visite: %d", tmleg/3,nbr_visit_egml );
printf("\n memory in full memory :  temps %f  nbr visite: %d", tmfeg/3,nbr_visit_egmf );






 /*--------------------------- Requete imbriques EF(EG(p)) ----------------------------*/
//    printf(" \n EFEG:\n");
//    debut = clock(); 
//     c =  EGEF_p_2tables(& ta,init_state->location,init_state->clock_zone,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFEGP 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver ? : %s ", c? "true" : "false  \n");


/*------------------------------------------------------------------------------------*/





// printf(" \n EFEFn:\n");

//  c = EFEF_pn_2tables(&ta, init_state->location, init_state->clock_zone, 2,
//                          checks, heuristics);

//   printf("\n trouver ? : %s ", c? "true" : "false  \n");


//   printf(" \n EFEGn:\n");

//  c =  EFEG_pn(&ta, init_state->location, init_state->clock_zone,
//                          checks, heuristics);

//   printf("\n trouver ? : %s ", c? "true" : "false  \n");


 /*--------------------------- Exploration espace etats ----------------------------*/
//    explore_state_space_ta(&ta);
//     build_state_space_ta(&ta, &state_space_ta);
//     print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
//     printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);

//     return 0;
    
}

