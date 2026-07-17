# LTS Extended Builder

Cet outil permet la **construction** et l'**exploration** de l'**espace d'états** d'un **automate temporisé étendu par des variables**.

---

## Modélisation

On peut créer un nouveau modèle sur la base de `ta_ext_fig2a.c` en modifiant :
- `init_ta()`
- `init_variables()`
- `init_update_functions()`
- `init_constraints()`

ainsi que la déclaration des variables dans le header `structure_variable.h`.

---

## Définir la proposition p à vérifier

1. Dans `heuristiques_et_check.c`, écrire :
   - `check_p` : la condition recherchée sur la partie **localité** et **horloge**
     de l'état.
   - `heuristique_checkp` : l'heuristique d'exploration sur cette même partie
     localité/horloge.
2. Dans `structure_variable.c`, écrire la contrepartie pour la partie **variable**
   de l'état :
   - `checkp_var` : la condition recherchée sur les variables, appelée par `check_p`.
   - `heuristiquep_var` : l'heuristique sur les variables, appelée par
     `heuristique_checkp`.

---

## Choisir l'algorithme et lancer l'exploration

Dans `main.c`, appeler l'un des algorithmes d'exploration à la volée, `EF_p` ou
`EG_p` selon la propriété recherchée, dans la variante de mémoire souhaitée :

**Variante 1 : mémorisation des border states uniquement**
- `EF_p_1table`, `EF_p`, `EF_p_HV`, `EF_p_HV_M`
- `EG_p_1table`, `EG_p_HV_M`, `EG_p_2tables`

**Variante 2 : mémorisation des border states + états de la couche actuelle**
- `EF_p_Memory_in_Layer`
- `EG_p_2tables_Memory_Layer`

**Variante 3 : sans mémoire**
- `EF_pNO_memory`
- `EG_p_2tablesNo_memory`

**Variante 4 : mémorisation de tous les états visités**
- `EF_FullMemory`
- `EG_FullMemory`

Ou, pour une requête imbriquée :
- `EGEF_p_2tables`
- `EFEF_pn_2tables`
- `EFEG_pn`

Ces fonctions prennent en argument :

| Argument            | Description                                                          |
|---------------------|-----------------------------------------------------------------------|
| `ta`                | l'automate temporisé (Timed Automaton)                                |
| `location`          | location du border state                                              |
| `clock`             | horloge du border state                                               |
| `result`            | état retourné si la propriété est satisfaite (NULL sinon)             |
| `check`             | fonction définissant la proposition p recherchée                      |
| `heuristique_check` | heuristique utilisée pour guider l'exploration                        |



Exemple :
```c
EF_p(&ta, init_state->location, init_state->clock_zone, &result, check_p, heuristique_checkp);
```

---

## Compilation et Exécution

1. **Compilation** (remplacer `nom_modele.c` par le fichier du modèle choisi) :
   ```bash
   gcc -o executable_name main.c ta_extended_builder.c DBM.c nom_modele.c structure_variable.c data_structures.c heuristiques_et_check.c nested_queries.c
   ```
   > Si l'algorithme utilisé est une fonction de base (`EF_p`, `EG_p`, ...) et non
   > une requête imbriquée, il n'est pas nécessaire d'inclure `nested_queries.c`.

2. **Exécution** :
   ```bash
   ./executable_name
   ```
