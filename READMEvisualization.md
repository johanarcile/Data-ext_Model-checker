# Requirements

Avant la compilation, récupérer le dossier flatcc avec la commande:

```bash
git clone https://github.com/dvidelabs/flatcc.git

# Génération du code C

flatcc -a structure_state_space_ta.fbs

# Génération du code Python

flatc --python structure_state_space_ta.fbs

## Compilation

gcc -I. -I./flatcc/include -o executable_name.exe main.c ta_extended_builder.c DBM.c variable.c ta_ext_fig2a.c save.c flatcc/src/runtime/builder.c flatcc/src/runtime/verifier.c flatcc/src/runtime/refmap.c flatcc/src/runtime/emitter.c

## Génération de l'espace d'états

.\executable_name.exe save state_space

## Lancement de la visualisation

python run_visualization.py state_space

## Si dash et flask non installés:
pip install dash flask


## Si flatbuffers non installé
pip install flatbuffers
```
