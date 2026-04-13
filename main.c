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
    //print_state(init_state,ta.locations);
 
 /*--------------------------- Goal cherche ----------------------------*/
    GoalCondition g;
   // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask =  CHECK_V; 
    g.active = false;
    g.v = 501;
    g.x = 500;
    strcpy(  g.name, "transition b");

 
 /*--------------------------- calculer nbr etats ----------------------------*/

    debut = clock(); 
    build_state_space_ta(&ta, &state_space_ta);
    fin = clock();
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
    printf("\n Temps d execution : %f secondes\n", temps_ecoule);

 /*--------------------------- EF(p) ----------------------------*/



 /*--------------------------- memory in borders-------------------*/
  printf("\n \n ------------------EF:-------------------------- \n ");
   printf("\n EF *********Memory on the borders only*************: \n ");
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
    
 printf("\n \n **************EF memory in layers:******************* \n ");
 
    debut = clock(); 
    c = EF_p_Memory_in_Layer(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables Memory in layers ? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }

 printf("\n \n ****************EF No memory:****************** \n ");
 
    debut = clock(); 
    c = EF_pNO_memory(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables No emory? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }


  printf("\n \n **********************EF Full memory************************: \n ");
 
    debut = clock(); 
    c = EF_FullMemory(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
    printf("\n trouver Avec  EFP 2 tables full emory? : %s ", c? "true" : "false \n");
    if (result != NULL){
         printf("\n Le state qui verifie\n");
         print_state(result, ta.locations);
         free (result);
    }




//  /*--------------------------- EG(p) ----------------------------*/

 // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask =  CHECK_ACTIVE; 
    g.active = false;
    g.v = 501;
    g.x = 500;
    strcpy(  g.name, "transition b");

  printf("\n \n ------------------EG:-------------------------- \n ");
     printf("\n EF *********Memory on the borders only*************: \n ");
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


  printf("\n \n **************EG memory in layers:******************* \n ");

    debut = clock(); 
    c = EG_p_2tables_Memory_Layer(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");

    
     printf("\n \n ****************EG No memory:****************** \n ");

    debut = clock(); 
    c = EG_p_2tablesNo_memory(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");




    printf("\n \n **********************EG Full memory************************: \n ");
   
    debut = clock(); 
    c = EG_FullMemory(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables full memory: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");



//  /*--------------------------- Requete imbriques EF(EG(p)) ----------------------------*/
//    printf("\n \n EFEG:\n");
//    debut = clock(); 
//     c = EGEF_p_2tables(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFEGP 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver ? : %s ", c? "true" : "false  \n");
//  /*--------------------------- Exploration espace etats ----------------------------*/
   // explore_state_space_ta(&ta);
   //  build_state_space_ta(&ta, &state_space_ta);
   //  print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
   //  printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);



 /*--------------------------- test ----------------------------*/
//    // explore_state_space_ta(&ta);
   //  build_state_space_ta(&ta, &state_space_ta);
   //  print_all_exist(&state_space_ta, &ta, &g);




 /*--------------------------- Test de Requete imbriques EF(p1 && EF(.......EF(pn)..) ----------------------------*/
    
 
 

// GoalCondition Pops [3];
//    Pops[0].mask =  CHECK_V; 
//      Pops[0].active = false;
//      Pops[0].v = 2;
//     Pops[0].x = 10;
// //   /*------------------------------*/
//    Pops[1].mask = CHECK_V  ; 
//      Pops[1].active = true;
//      Pops[1].v = 501;
//     Pops[1].x = 1;
//   /*------------------------------*/
//    Pops[2].mask = CHECK_V   ; 
//      Pops[2].active = false;
//      Pops[2].v = 500;
//     Pops[2].x = 1;
 
 
//     /*------------------------------*/
//    printf("\n \n ===============EFEF Pn: ====================\n");
//    debut = clock(); 
//     c = EFEF_pn_2tables(& ta,init_state->location,init_state->clock_zone,Pops,3,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFEF Pn 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver ? : %s ", c? "true" : "false  \n");




//   GoalCondition props[3];
//   props[0].mask = CHECK_X | CHECK_V;
//   props[0].v = 5; props[0].x = 5;

//   props[1].mask = CHECK_V;
//   props[1].v = 5;

//   props[2].mask = CHECK_ACTIVE;
//   props[2].active = true;

//     GoalCondition* P; 
//     P = build_EFEG_goals(props,3);
//     printf("\n les props recherches: \n");
//     printf("\n P0 :     mask : %d , v = %d, x= %d ,  Active = %d \n", P[0].mask, P[0].v, P[0].x, P[0].active );
//     printf("\n P1 :     mask : %d , v = %d, x= %d ,  Active = %d \n", P[1].mask, P[1].v, P[1].x, P[1].active );
//      printf("\n \n========================== EFEG_pn:=============================================\n");
//     debut = clock(); 
//     c = EFEG_pn(& ta,init_state->location,init_state->clock_zone,P,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFEG_pn 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver ? : %s ", c? "true" : "false  \n");



/* ---------------------------------No memory------------------------------------*/
//  printf("\n \n EF No memory: \n ");
 
//     debut = clock(); 
//     c = EF_pNO_memory(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
//     printf("\n trouver Avec  EFP 2 tables No emory? : %s ", c? "true" : "false \n");
//     if (result != NULL){
//          printf("\n Le state qui verifie\n");
//          print_state(result, ta.locations);
//          free (result);
//     }


//  printf("\n \n EG No memory: \n ");

//     debut = clock(); 
//     c = EG_p_2tablesNo_memory(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");



/* ---------------------------------memory in layers------------------------------------*/

//  printf("\n \n EF memory in layers: \n ");
 
//     debut = clock(); 
//     c = EF_p_Memory_in_Layer(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
//     printf("\n trouver Avec  EFP 2 tables No emory? : %s ", c? "true" : "false \n");
//     if (result != NULL){
//          printf("\n Le state qui verifie\n");
//          print_state(result, ta.locations);
//          free (result);
//     }



//  printf("\n \n EG:\n");
//     debut = clock(); 
//     c = EG_p_2tables_Memory_Layer(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");

/* ---------------------------------Full memory------------------------------------*/
   //  printf("\n \n EF Full memory: \n ");
 
   //  debut = clock(); 
   //  c = EF_FullMemory(& ta,init_state->location,init_state->clock_zone,&g, &result, check_p,heuristique_checkp);
   //  fin = clock();            // Fin du chronomètre
   //  temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
   //  printf("\n Temps d execution EFP 2 tables : %f secondes", temps_ecoule);
   //  printf("\n trouver Avec  EFP 2 tables full emory? : %s ", c? "true" : "false \n");
   //  if (result != NULL){
   //       printf("\n Le state qui verifie\n");
   //       print_state(result, ta.locations);
   //       free (result);
   //  }
//  printf("\n \n EG:\n");
//     debut = clock(); 
//     c = EG_FullMemory(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables full memory: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");

/*==========================Tesssssst===============================*/

//  printf("\n \n ===============EFEF Pn recursive: ====================\n");
//    debut = clock(); 
//     c = EFPn_recursive(& ta,init_state->location,init_state->clock_zone,Pops,3,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EFEF Pn recurssive 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver ? : %s ", c? "true" : "false  \n");



    return 0;
    
}

