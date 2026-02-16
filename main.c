#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include "hashTable.h"
// Déclarations manuelles de la fonction de construction du modèle

void fill_ta_struct(TA* ta);


int main() {
    TA ta;
    fill_ta_struct(&ta);
    State* init_state = compute_init_state(&ta);
    int c = EF_p(& ta,init_state,11,check_p);
    printf("\n la valeur retourne dans main%d",c);

    // explore_state_space_ta(&ta);
    // bool c = c_EFP(& ta);
    // printf("%d\n",c);

    // State_space_TA state_space_ta;
    // build_state_space_ta(&ta, &state_space_ta);
    // print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
    // printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);
    // return 0;
    
}

