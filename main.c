#include <stdio.h>
#include "structure_DBM.h"
#include "structure_ta.h"
#include "structure_state_space_ta.h"
#include "save.h"

void fill_ta_struct(TA *ta);

int main()
{
    TA ta;
    fill_ta_struct(&ta);

    // explore_state_space_ta(&ta);

    State_space_TA state_space_ta;
    build_state_space_ta(&ta, &state_space_ta);
    // print_state_space_ta(&state_space_ta, ta.locations, ta.actions);
    printf("Nombre total d'états étendus : %d\n", state_space_ta.nb_etats);
    save_to_fbs(&state_space_ta, "state_space", ta.locations, 6, ta.actions, 3);
    return 0;
}