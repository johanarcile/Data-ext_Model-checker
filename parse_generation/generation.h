#ifndef GENERATION_H
#define GENERATION_H

#include "parse.h"

void generation_uthash_h();
void generation_structure_DBM_h(int nb_clocks);
void generation_structure_variable_h();
void generation_structure_ta_h();
void generation_state_space_ta_h();
void generation_DBM_c();
void generation_variable_c();
void generation_model_c(int nb_locations, int nb_actions, int nb_clocks, char** locations, DBM* invariants, char** actions, int* nb_transitions_locations, Transition*** transitions);
void generation_ta_extended_builder_c();
void generation_main_c();
void generation_gitignore();
void generation(ParseInfos* parseInfos);

#endif