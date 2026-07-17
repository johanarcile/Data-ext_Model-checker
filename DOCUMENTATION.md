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

### `structure_variable.h` / `structure_variable.c`
Structure de données propre à chaque modèle, et fonctions relatives aux variables.

**Fonctions principales**
- `equal_var()` — retourne vrai si deux variables sont égales, faux sinon
- `eprint_variable()` — affiche la valeur de chaque champ de la variable
- `checkp_var()` — vérifie la propriété p relative aux variables de données
- `heuristiquep_var()` — heuristique d'exploration relative aux variables de données

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
