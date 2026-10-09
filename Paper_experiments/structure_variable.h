#ifndef STRUCTURE_VARIABLE_H
#define STRUCTURE_VARIABLE_H

#define linear_layers 1
#define small_layers  2
#define large_layers  3

#if MODEL == linear_layers
  #include "structure_variable_model_linear_layers.h"
#elif MODEL == small_layers
  #include "structure_variable_model_small_layers.h"
#elif MODEL == large_layers
  #include "structure_variable_model_large_layers.h"
#else
   #include "structure_variable_model_small_layers.h"
 // #error "Definir MODEL a 1, 2 ou 3 (ex: -DMODEL=2)"
#endif

#endif