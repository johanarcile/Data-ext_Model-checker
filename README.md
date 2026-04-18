# LTS Extended Builder

Cet outil permet la **construction** et l'**exploration** de l'**espace d'états** d'un **automate temporisé étendu par des variables**.


---

## Modèlisation

On peut créer un nouveau modèle sur la base de ta_ext_fig2a.c en modifiant
   - `init_ta()`
   - `init_variables()`
   - `init_update_functions()`
   - `init_constraints()`
ainsi que la déclaration des variables dans le header `structure_variable.h`


---

##  Compilation et Execution

1. La compilation : gcc -o executable_name main.c ta_extended_builder.c DBM.c variable.c ta_ext_fig2a.c
2. L'execution : ./executable_name 

