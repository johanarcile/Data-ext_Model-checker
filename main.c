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
    
    double tnmef = 0,tmbef = 0,  tmbhpef = 0,  tmbhmef = 0, tmlef = 0,  tmfef = 0, 
           tnmeg = 0,tmbeg = 0 ,   tmfeg = 0,  tmleg = 0, tmbhmeg = 0, tmbhpeg = 0;
    int   nbr_visit_efnom = 0,nbr_visit_efmb =0,nbr_visit_efmbhp= 0, nbr_visit_efmbhm = 0,nbr_visit_efml = 0,nbr_visit_efmf =0,  
    nbr_visit_egnom =0 ,  nbr_visit_egmb = 0,  nbr_visit_egmbhp = 0, nbr_visit_egmbhm = 0,nbr_visit_egml=0, nbr_visit_egmf = 0;
     GoalCondition g;

    //      g.mask =  CHECK_ACTIVE; 
//     g.active = false;
//     g.v = 501;
//     g.x = 500;
//     strcpy(  g.name, "transition b");
    
//  printf("\n \n **********************EG Full memory************************: \n ");
   
//     debut = clock(); 
//     c = EG_FullMemory(& ta,init_state->location,init_state->clock_zone,&g,check_p,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables full memory: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
//     tmfeg =+ temps_ecoule;

    printf("\n ***************************Resultats pour EF(p) :************************************** ");



    State* init_state = compute_init_state(&ta);
    //print_state(init_state,ta.locations);
 
//  /*--------------------------- Goal cherche ----------------------------*/
   
   // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
//     g.mask =  CHECK_ACTIVE; 
//     g.active = false;
//     g.v = 1001000;
//     g.x = 500;
//     strcpy(  g.name, "transition b");

 
 /*--------------------------- calculer nbr etats ----------------------------*/

    debut = clock(); 
    build_state_space_ta(&ta, &state_space_ta);
    fin = clock();
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Nombre total d'états étendus : %d", state_space_ta.nb_etats);
    printf("\n Temps d execution : %f secondes\n", temps_ecoule);




printf("\n ======================================== Les tests =====================================");

for(int i=0; i< 3; i++) {

     //  /*--------------------------- Goal cherche ----------------------------*/

   // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask =  CHECK_V | CHECK_ACTIVE ;//|CHECK_X; 
    g.active = false;
    g.v = 1001000;//990000;//500;//990000;//9000;
    g.x = 95;
    strcpy(  g.name, "transition b");

printf("\n ---------------- Test n: %d ----------------------------------------------------------",i);
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
     tmbef =+ temps_ecoule;




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

     tmbhpef =+ temps_ecoule;

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
   tmbhmef =+ temps_ecoule;

    
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

     tmlef += temps_ecoule; 


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

    tmfef += temps_ecoule;


//  printf("\n \n ****************EF No memory:****************** \n ");
 
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

//     tnmef = tnmef + temps_ecoule;


//  /*--------------------------- EG(p) ----------------------------*/

 // g.mask = CHECK_V | CHECK_ACTIVE |CHECK_NAME;
    g.mask =  CHECK_ACTIVE ; 
    g.active = false;
    g.v = 990001;//501;
    g.x = 1000;
    strcpy(  g.name, "transition b");

  printf("\n \n ------------------EG:-------------------------- \n ");
     printf("\n EG *********Memory on the borders only*************: \n ");
    debut = clock(); 
    c = EG_p_2tables(& ta,init_state->location,init_state->clock_zone,&g,check_p_inf,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
    tmbeg = + temps_ecoule;



 

     debut = clock(); 
    c = EG_p_HV_M(& ta,init_state->location,init_state->clock_zone,&g,check_p_inf,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP heap pool juse maloc au besoins: %f secondes", temps_ecoule);
    printf("\n trouver Avec  HEAP ET TABLE? : %s ", c? "true" : "false  \n");
    tmbhmeg = + temps_ecoule;

  printf("\n \n **************EG memory in layers:******************* \n ");

    debut = clock(); 
    c = EG_p_2tables_Memory_Layer(& ta,init_state->location,init_state->clock_zone,&g,check_p_inf,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");

    tmleg =+ temps_ecoule;

  printf("\n \n **********************EG Full memory************************: \n ");
   
    debut = clock(); 
    c = EG_FullMemory(& ta,init_state->location,init_state->clock_zone,&g,check_p_inf,heuristique_checkp);
    fin = clock();            // Fin du chronomètre
    temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\n Temps d execution EGP Avec 2 tables full memory: %f secondes", temps_ecoule);
    printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
    tmfeg =+ temps_ecoule;

    
//      printf("\n \n ****************EG No memory:****************** \n ");

//     debut = clock(); 
//     c = EG_p_2tablesNo_memory(& ta,init_state->location,init_state->clock_zone,&g,check_p_inf,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
//     tnmeg =+ temps_ecoule;





}


printf("\n ======================================== sumurry =====================================");

printf("\n ***************************Resultats pour EF(p) :************************************** ");
printf("\n No memory :  temps :  %f  nbr visite: %d", tnmef /3,nbr_visit_efnom );
printf("\n memory in borders only  2 tables :  temps %f  nbr visite", tmbef /3,nbr_visit_efmb );
printf("\n memory in borders only  HEAP ET TABLE :  temps %f  nbr visite", tmbhpef/3,nbr_visit_efmbhp );
printf("\n memory in borders only  HEAP ET TABLE juse maloc au besoins :  temps %f  nbr visite", tmbhmef/3,nbr_visit_efmbhm );
printf("\n memory in borders + current layer :  temps %f  nbr visite", tmlef/3,nbr_visit_efml );
printf("\n memory in full memory :  temps %f  nbr visite", tmfef/3,nbr_visit_efmf );

printf("\n ***************************Resultats pour EG(p) :************************************** ");
printf("\n No memory :  temps :  %f  nbr visite: %d", tnmeg/3,nbr_visit_egnom );
printf("\n memory in borders only  2 tables :  temps %f  nbr visite", tmbeg/3,nbr_visit_egmb );
printf("\n memory in borders only  HEAP ET TABLE juse maloc au besoins :  temps %f  nbr visite", tmbhmeg/3,nbr_visit_egmbhm );
printf("\n memory in borders + current layer :  temps %f  nbr visite", tmleg/3,nbr_visit_egml );
printf("\n memory in full memory :  temps %f  nbr visite", tmfeg/3,nbr_visit_egmf );



// g.mask =  CHECK_V |CHECK_ACTIVE ; 
//     g.active = false;
//     g.v = 500;
//     g.x = 1000;
//     strcpy(  g.name, "transition b");

//   printf("\n \n **********************EG Full memory************************: \n ");
   
//     debut = clock(); 
//     c = EG_FullMemory(& ta,init_state->location,init_state->clock_zone,&g,check_p_inf,heuristique_checkp);
//     fin = clock();            // Fin du chronomètre
//     temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
//     printf("\n Temps d execution EGP Avec 2 tables full memory: %f secondes", temps_ecoule);
//     printf("\n trouver Avec  2 TABLES ? : %s ", c? "true" : "false  \n");
//     tmfeg =+ temps_ecoule;

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

