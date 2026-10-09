# Timed Automata Model Checker: Project Documentation
 
This document lists, for each file of the project, the structures and functions it defines. It is a quick reference for navigating the code without opening every file.

---

## Folders
 
### `Results/`
Latest results obtained for the models `linear_layers`, `small_layers` and `large_layers`, on the properties defined in the paper.
 
---

## Files
 
### `structure_DBM.h` / `DBM.c`
DBM (Difference Bound Matrix) structure used to represent the clocks of the automata, and the associated functions.
 
---
 
### `data_structures.c`
Management functions for the data structures used by the exploration algorithms: hash tables (via `uthash.h`) and MinHeap.
 
The structures themselves (`visit`, `StateWeight`, `StateWeightExp`, `StateHash`, `MinHeap`, `MinHeapP`) are declared in `structure_state_space_ta.h`.
 
- Every hash table follows the same function pattern: `xxx_add`, `xxx_find`, `xxx_destroy`.
- The two MinHeaps (`MinHeap` and its pointer variant `MinHeapP`) follow the classic pattern: `heap_create`, `heap_push`, `heap_pop`, `heap_destroy` (plus `heap_swap`, `heap_sift_up` and `heap_sift_down` internally).
---
 
### `heuristiques_et_check.c`
Defines the searched property (`check`) and the exploration heuristic (`heuristique_check`) used by the exploration algorithms.
 
---
 
### `main_rep_true.c` / `main_rep_false.c`
Entry points of the experiments. Compile **one** of them, not both.
 
- `main_rep_true.c`: runs the different algorithms answering the query EF(p) in the case where p **is satisfied**, and prints for each of them the results (number of visited states and execution time).
- `main_rep_false.c`: same principle, for the query EF(p) in the case where p **is not satisfied**.
---


### `structure_state_space_ta.h`
Central header grouping:
- the state structure (`State`) and the structures used by the hash tables and the MinHeap,
- the management functions of these data structures,
- the `check` and heuristic functions,
- the functions that build and explore the state space,
- the different variants of the on-the-fly exploration functions for EF(p) and EG(p).
---
 
### `structure_ta.h`
Structure of a timed automaton (Timed Automaton) and of its transitions.
 
---
 
### `ta_extended_builder.c`
Basic functions for building and exploring the state space, and the different variants of on-the-fly exploration for EF(p) and EG(p).
 
**State space construction**
- `ajouter_etat`
- `build_state_space_ta`
- `print_state_space_ta`
- `compute_init_state`
- `get_successors`
- `print_state`
- `explore_state_space_ta`
**On-the-fly exploration, variant 1: only the border states are stored**
- `NextBorder`, `EGNextBorder`
- `EF_p_1table`, `EF_p`, `EF_p_HV`, `EF_p_HV_M`
- `EG_p_1table`, `EG_p_HV_M`, `EG_p_2tables`
**On-the-fly exploration, variant 2: border states + states of the current layer are stored**
- `NextBorderMemory`, `EF_p_Memory_in_Layer`
- `NextBorderMemoryEG`, `EG_p_2tables_Memory_Layer`
**On-the-fly exploration, variant 3: no memory**
- `EF_pNO_memory`
- `EG_p_2tablesNo_memory`
**On-the-fly exploration, variant 4: all visited states are stored**
- `EF_FullMemory`
- `EG_FullMemory`
**Test function**
- `print_all_exist`: explores every state of the state space to check whether a property holds. It serves as a reference to validate the exploration functions above.
---
 
### `uthash.h`
External hash table library. Documentation: https://troydhanson.github.io/uthash/
 
---
 ## Models
 
The three models are extended timed automata with two clocks `x` and `y` and a data variable `v`.
 
| Model | Name | Locations | Invariant | `vmax` |
|-------|------|-----------|-----------|--------|
| 1 | `linear_layers` | 4 (`l0` to `l3`) | `x, y ≤ 2` | 9 900 000 |
| 2 | `small_layers` | 2 (`l0`, `l1`) | `x, y ≤ 32` | 3 000 |
| 3 | `large_layers` | 4 (`l0` to `l3`), same structure as `linear_layers` | `x, y ≤ 12` | 20 000 |
 
Each model is made of two files, named after the model:
 
- `model_<name>.c`: the automaton itself,
- `structure_variable_model_<name>.h` / `.c`: the data structure and functions for the variable `v` (see below).
---
 
### `structure_variable.h` and `structure_variable_model_<name>.h` / `.c`
Data structure specific to each model, and functions related to its variables. `<name>` is one of `linear_layers`, `small_layers` or `large_layers`.
 
- `structure_variable.h`: common header, included by all the other files of the project. It defines the model names as numeric macros (`linear_layers` = 1, `small_layers` = 2, `large_layers` = 3) and selects the header of the active model according to the macro `MODEL` given at compile time. If `MODEL` is not defined, `small_layers` is used. An invalid value triggers a `#error`.
- `structure_variable_model_linear_layers.h`, `structure_variable_model_small_layers.h`, `structure_variable_model_large_layers.h`: definition of `struct Variable` for each model, together with the prototypes of the associated functions.
- `structure_variable_model_linear_layers.c`, `structure_variable_model_small_layers.c`, `structure_variable_model_large_layers.c`: implementation of these functions for each model.
**Main functions**
- `equal_var()`: returns true if two variables are equal, false otherwise
- `print_variable()`: prints the value of each field of the variable
- `check_p_var()`: checks the property p on the data variables
- `heuristique_checkp_var()`: exploration heuristic on the data variables
- `check_p_var_true()`: predefined query on the data variables, kept as used to obtain the results of the paper (case where p is satisfied)
- `check_p_var_false()`: same for the second query of the paper (case where p is not satisfied)
---


