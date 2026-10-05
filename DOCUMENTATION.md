# Documentation des structures et modules

Ce document liste, pour chaque fichier du projet, les structures et les fonctions
définies. Il sert de référence rapide pour naviguer dans le code sans avoir à
ouvrir chaque fichier.

---

## Dossiers

### `Results/`
Derniers résultats obtenus pour les modèles 1, 2 et 3, sur les propriétés définies
dans l'article.

### `tests/`
Résultats d'autres tests (modèles et propriétés différents de ceux retenus dans
`Results/`).

### `other models/`
Autres modèles utilisés lors de tests antérieurs, non retenus pour les résultats
finaux.

---

## Fichiers

### `structure_DBM.h` / `DBM.c`
Structure DBM (Difference Bound Matrix) utilisée pour représenter les horloges
des automates, et fonctions associées.

---

### `data_structures.c`
Fonctions de gestion des structures de données utilisées par les algorithmes
d'exploration : tables de hachage (via `uthash.h`) et MinHeap.

Les structures elles-mêmes (`visit`, `StateWeight`, `StateWeightExp`, `StateHash`,
`mark`, `MinHeap`, `MinHeapP`) sont déclarées dans `structure_state_space_ta.h`.

Chaque table de hachage suit le même schéma de fonctions : `xxx_add`, `xxx_find`,
`xxx_destroy`. Les deux MinHeap (`MinHeap` et sa variante pointeur `MinHeapP`)
suivent le schéma classique : `heap_create`, `heap_push`, `heap_pop`, `heap_destroy`
(+ `heap_swap` / `heap_sift_up` / `heap_sift_down` en interne).

---

### `heuristiques_et_check.c`
Définit la propriété recherchée (`check`) et l'heuristique d'exploration
(`heuristique_check`) utilisées par les algorithmes d'exploration.

---

### `main.c`
Programme principal à exécuter.

---

### `main_rep_true.c` / `main_rep_false.c`

- `main_rep_true.c` : exécute les différents algorithmes qui répondent à la requête EF(p) dans le cas où p **est vérifiée**, et affiche pour chacun les résultats (nombre d'états visités et temps d'exécution).
- `main_rep_false.c` : même principe, pour la requête EF(p) dans le cas où p **n'est pas vérifiée**. 
---
## Modèles utilisés

Les trois modèles sont des automates temporisés étendus avec deux horloges `x` et `y` et une variable de données `v`. 

### Modèle 1 : linear layers (`modele01.c`)
4 localités `l0` à `l3`, invariant `x, y ≤ 2`, `vmax = 9 900 000`. 

### Modèle 2 : small layers(`modele02.c`)
2 localités `l0`, `l1`, invariant `x, y ≤ 32`, `vmax = 3 000`. 

### Modèle 3 : large layers(`modele03.c`)
Même structure que le modèle 1, avec invariant `x, y ≤ 12`, `vmax = 20 000`.




### `nested_queries.h` / `nested_queries.c`
Implémentation des requêtes CTL imbriquées.

**Fonctions principales**
- `EGEF_p_2tables`
- `EFEF_pn_2tables`
- `EFEG_pn`

---

### `structure_state_space_ta.h`
Header central regroupant :
- la structure de l'état (`State`) et les structures utilisées par les tables de
  hachage et le MinHeap,
- les fonctions de gestion de ces structures de données,
- les fonctions `check` et `heuristiques`,
- les fonctions de construction et d'exploration de l'espace d'états,
- les différentes variantes des fonctions d'exploration à la volée de EF(p) et EG(p).

---

### `structure_ta.h`
Structure d'un automate temporisé (Timed Automaton) et de ses transitions.

---

### `structure_variable.h` / `structure_variable_model{1,2,3}.h` / `structure_variable_model{1,2,3}.c`
Structure de données propre à chaque modèle, et fonctions relatives aux variables.

- `structure_variable.h` : header commun, inclus par tous les autres fichiers du projet. Il sélectionne le header du modèle actif selon la macro `MODEL` définie à la compilation (`modele2` si `MODEL` n'est pas défini).
- `structure_variable_model1.h`, `structure_variable_model2.h`, `structure_variable_model3.h` : définition de `struct Variable` pour chaque modèle, ainsi que les prototypes des fonctions associées (`equal_var`, `print_variable`, `check_p_var`, `heuristique_checkp_var`, `check_p_var_true`, `check_p_var_false`).
- `structure_variable_model1.c`, `structure_variable_model2.c`, `structure_variable_model3.c` : implémentation de ces fonctions pour chaque modèle.

> **Important** : les autres fichiers du projet doivent inclure uniquement `structure_variable.h`, jamais directement un `structure_variable_modeli.h`.

#### Choix du modèle

Le modèle se choisit à la compilation avec `-DMODEL=<i>`, et il faut compiler le `.c` correspondant (un seul des trois) :

```bash
gcc -DMODEL=2 -o executable_name main.c ta_extended_builder.c DBM.c modele02.c structure_variable_model2.c data_structures.c heuristiques_et_check.c
```

Pour changer de modèle, modifier à la fois la valeur de `MODEL` et le fichier `structure_variable_modeli.c` (ainsi que le fichier `modele0i.c` correspondant). Penser à recompiler tous les fichiers `.c` ensemble.

**Fonctions principales**
- `equal_var()` — retourne vrai si deux variables sont égales, faux sinon
- `eprint_variable()` — affiche la valeur de chaque champ de la variable
- `checkp_var()` — vérifie la propriété p relative aux variables de données
- `heuristiquep_var()` — heuristique d'exploration relative aux variables de données
- `check_p_var_true()` : requête prédéfinie sur les variables de données, conservée telle qu'utilisée pour obtenir les résultats du papier (cas où la propriété p est vérifiée)
- `check_p_var_false()` : idem pour le second cas de requête du papier (cas où la propriété p n'est pas vérifiée)

---


### `ta_ext_fig2a.c`
Modèle de base ayant servi à la construction des autres modèles (`modele01.c`,
`modele02.c`, `modele03.c`).

---

### `ta_extended_builder.c`
Fonctions de base pour la construction et l'exploration de l'espace d'états, et
différentes variantes d'exploration à la volée de EF(p) et EG(p).

**Construction de l'espace d'états**
- `ajouter_etat`
- `build_state_space_ta`
- `print_state_space_ta`
- `compute_init_state`
- `get_successors`
- `print_state`
- `explore_state_space_ta`

**Exploration à la volée — variante 1 : mémorisation des border states uniquement**
- `NextBorder`, `EGNextBorder`
- `EF_p_1table`, `EF_p`, `EF_p_HV`, `EF_p_HV_M`
- `EG_p_1table`, `EG_p_HV_M`, `EG_p_2tables`

**Exploration à la volée — variante 2 : mémorisation des border states + états de
la couche actuelle**
- `NextBorderMemory`, `EF_p_Memory_in_Layer`
- `NextBorderMemoryEG`, `EG_p_2tables_Memory_Layer`

**Exploration à la volée — variante 3 : sans mémoire**
- `EF_pNO_memory`
- `EG_p_2tablesNo_memory`

**Exploration à la volée — variante 4 : mémorisation de tous les états visités**
- `EF_FullMemory`
- `EG_FullMemory`

**Fonction de test**
- `print_all_exist` — explore tous les états de l'espace d'états pour vérifier
  l'existence d'une propriété ; sert de référence pour valider les fonctions
  d'exploration ci-dessus.

---

### `uthash.h`
Bibliothèque externe de tables de hachage. Documentation : https://troydhanson.github.io/uthash/

---
