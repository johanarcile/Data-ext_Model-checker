#ifndef STRUCTURE_VARIABLE_H
#define STRUCTURE_VARIABLE_H

#define chain_layers 1
#define small_layers  2
#define large_layers  3



#if MODEL == chain_layers
  #include "structure_variable_model_chain_layers.h"
#elif MODEL == small_layers
  #include "structure_variable_model_small_layers.h"
#elif MODEL == large_layers
  #include "structure_variable_model_large_layers.h"
#else
   #error "Define MODEL :  chain_layers , small_layers or large_layers (ex: -DMODEL= chain_layers)"
    #include "structure_variable_model_small_layers.h"
#endif

#endif