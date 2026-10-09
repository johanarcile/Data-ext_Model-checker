# Reproducing the Paper Results
 
This document explains how to compile and run the experiments of the paper. The queries used for the experiments are already saved in the code, so no query needs to be rewritten: it is enough to compile the project with the right model and the right `main` file, and to run the executable.
 
The latest results we obtained are stored in the `Results/` folder, for the models `linear_layers`, `small_layers` and `large_layers`.
 
---
 
## Requirements
 
- `gcc` (on Windows, e.g. MinGW-w64), available from the terminal.
---
 
## What determines the configuration
 
Two choices determine which executable is built:
 
- **The model**: `linear_layers`, `small_layers` or `large_layers`. It is selected with `-DMODEL=<name>`, and the two files specific to the model must be compiled with it: `model_<name>.c` and `structure_variable_model_<name>.c`.
- **The case of the query EF(p)**:
  - `main_rep_true.c`: p **is satisfied**.
  - `main_rep_false.c`: p **is not satisfied**.
The queries themselves are saved in `check_p_var_true()` and `check_p_var_false()` of each `structure_variable_model_<name>.c`.
 
> **Warning**
> - `MODEL`, `model_<name>.c` and `structure_variable_model_<name>.c` must always refer to the same model. Mixing files of different models causes compilation errors or incorrect behaviour.
> - Write `-DMODEL=small_layers` **without any space** after `=`. 
 
---
 
## Compilation
 
All the commands below can be copied and pasted as they are. Run them from the project folder.
 
### Model `linear_layers`
 
**EF(p) where p is satisfied:**
```bash
gcc -DMODEL=linear_layers -o executable_name main_rep_true.c ta_extended_builder.c DBM.c model_linear_layers.c structure_variable_model_linear_layers.c data_structures.c heuristiques_et_check.c
```
 
**EF(p) where p is not satisfied:**
```bash
gcc -DMODEL=linear_layers -o executable_name main_rep_false.c ta_extended_builder.c DBM.c model_linear_layers.c structure_variable_model_linear_layers.c data_structures.c heuristiques_et_check.c
```
 
### Model `small_layers`
 
**EF(p) where p is satisfied:**
```bash
gcc -DMODEL=small_layers -o executable_name main_rep_true.c ta_extended_builder.c DBM.c model_small_layers.c structure_variable_model_small_layers.c data_structures.c heuristiques_et_check.c
```
 
**EF(p) where p is not satisfied:**
```bash
gcc -DMODEL=small_layers -o executable_name main_rep_false.c ta_extended_builder.c DBM.c model_small_layers.c structure_variable_model_small_layers.c data_structures.c heuristiques_et_check.c
```
 
### Model `large_layers`
 
**EF(p) where p is satisfied:**
```bash
gcc -DMODEL=large_layers -o executable_name main_rep_true.c ta_extended_builder.c DBM.c model_large_layers.c structure_variable_model_large_layers.c data_structures.c heuristiques_et_check.c
```
 
**EF(p) where p is not satisfied:**
```bash
gcc -DMODEL=large_layers -o executable_name main_rep_false.c ta_extended_builder.c DBM.c model_large_layers.c structure_variable_model_large_layers.c data_structures.c heuristiques_et_check.c
```
 
---
 
## Execution
 
```bash
./executable_name        # Linux / macOS
.\executable_name.exe    # Windows (PowerShell)
```
 
The program runs the different exploration algorithms on the query EF(p) and prints, for each of them, the number of visited states and the execution time. These are the values reported in the paper (execution times depend on the machine).
 
---

## Long execution times
 
> **Note**
> For some models, some of the exploration functions called in `main_rep_true.c` and `main_rep_false.c` take a very long time to run (see the results of the paper, also stored in `Results/`).
> To reproduce the other results without waiting for them, comment out the calls to these functions in `main_rep_true.c` and `main_rep_false.c` (with `//` or `/* ... */`), then compile and run as described above.
> To reproduce the results of these functions as well, uncomment their calls and recompile.
 
## Models used
 
The three models are extended timed automata with two clocks `x` and `y` and one data variable `v`.
 
| Model | Locations | Invariant | `vmax` |
|-------|-----------|-----------|--------|
| `linear_layers` | 4 (`l0` to `l3`) | `x, y ≤ 2` | 9 900 000 |
| `small_layers` | 2 (`l0`, `l1`) | `x, y ≤ 32` | 3 000 |
| `large_layers` | 4 (`l0` to `l3`), same structure as `linear_layers` | `x, y ≤ 12` | 20 000 |
 
For the description of the project files and functions, see [DOCUMENTATION.md](DOCUMENTATION.md).