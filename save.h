#ifndef SAVE_H
#define SAVE_H

#include "structure_state_space_ta.h"

int save_to_fbs(State_space_TA *ss, const char *filename, char **locations, int nb_locations, char **actions, int nb_actions);

#endif