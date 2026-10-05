#ifndef STRUCTURE_VARIABLE_H
#define STRUCTURE_VARIABLE_H

#if MODEL == 1
  #include "structure_variable_model1.h"
#elif MODEL == 2
  #include "structure_variable_model2.h"
#elif MODEL == 3
  #include "structure_variable_model3.h"
#else
   #include "structure_variable_model2.h"
 // #error "Definir MODEL a 1, 2 ou 3 (ex: -DMODEL=2)"
#endif

#endif