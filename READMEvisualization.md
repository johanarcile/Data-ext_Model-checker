## Compilation

gcc -I. -I./flatcc/include -o executable_name.exe main.c ta_extended_builder.c DBM.c variable.c ta_ext_fig2a.c save.c flatcc/src/runtime/builder.c flatcc/src/runtime/verifier.c flatcc/src/runtime/refmap.c flatcc/src/runtime/emitter.c

## Génération de l'espace d'états

.\executable_name.exe save state_space

## Lancement de la visualisation

python run_visualization.py state_space

Si dash et flask non installés:
pip install dash flask
