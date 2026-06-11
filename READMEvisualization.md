# Requirements

Avant la compilation, récupérer le dossier flatcc avec la commande:

````bash
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


# State Space Visualizer

Outil de visualisation interactive d'espaces d'états pour automates temporisés. Le pipeline complet se déroule en quatre étapes : génération des fichiers C depuis le schéma FlatBuffers → compilation C → exécution pour générer le fichier binaire → génération des fichiers Python depuis le schéma FlatBuffers → lancement de la visualisation Python.

Les fichiers C générés (`structure_state_space_ta_reader.h`, `structure_state_space_ta_builder.h`, `structure_state_space_ta_verifier.h`, `flatbuffers_common_reader.h`, `flatbuffers_common_builder.h`) sont produits par `flatcc` à partir de `state_space_ta.fbs` et ne doivent pas être modifiés à la main.

---

## Prérequis

### Linux (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install gcc python3 python3-pip
````

### Windows

1. Installer **MSYS2** : https://www.msys2.org  
   Ouvrir le terminal **MSYS2 MinGW 64-bit** et exécuter :

   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```

   Puis ajouter `C:\msys64\mingw64\bin` au PATH système.

2. Installer **Python 3** : https://www.python.org/downloads  
   Cocher "Add Python to PATH" pendant l'installation.

Vérifier les installations :

```bash
gcc --version
python --version    # ou python3 --version sur Linux
```

---

## Installation

Si flatbuffers non installé:

```
pip install flatbuffers
```

### Dépendances Python

```bash
pip install dash flask
```

Sur Linux si pip n'est pas disponible :

```bash
pip3 install dash flask
```

---

## Pipeline complet

Les étapes doivent être exécutées dans l'ordre depuis la racine du projet.

---

### Étape 1 — Générer les fichiers C depuis le schéma FlatBuffers

Cette étape est nécessaire uniquement si tu modifies `state_space_ta.fbs`. Les fichiers générés sont déjà présents dans le dépôt.

Installer `flatcc` :

**Linux :**

```bash
git clone https://github.com/dvidelabs/flatcc.git
cd flatcc
mkdir build && cd build
cmake ..
make
sudo make install
cd ../..
```

**Windows :**  
Télécharger le binaire précompilé depuis https://github.com/dvidelabs/flatcc/releases  
ou compiler avec CMake + Visual Studio / MSYS2.

Générer les fichiers C :

```bash
flatcc --common --reader --builder --verifier -o . state_space_ta.fbs
```

Cela produit dans le répertoire courant :

- `structure_state_space_ta_reader.h`
- `structure_state_space_ta_builder.h`
- `structure_state_space_ta_verifier.h`
- `flatbuffers_common_reader.h`
- `flatbuffers_common_builder.h`

> Si tu ne modifies pas le schéma, saute directement à l'étape 2.

---

### Étape 2 — Compiler le projet C

**Linux :**

```bash
gcc -I. -I./flatcc/include \
    -o executable_name \
    main.c ta_extended_builder.c DBM.c variable.c ta_ext_fig2a.c save.c \
    flatcc/src/runtime/builder.c \
    flatcc/src/runtime/verifier.c \
    flatcc/src/runtime/refmap.c \
    flatcc/src/runtime/emitter.c
```

**Windows (invite de commandes ou PowerShell) :**

```bat
gcc -I. -I./flatcc/include -o executable_name.exe main.c ta_extended_builder.c DBM.c variable.c ta_ext_fig2a.c save.c flatcc/src/runtime/builder.c flatcc/src/runtime/verifier.c flatcc/src/runtime/refmap.c flatcc/src/runtime/emitter.c
```

> Si `gcc` n'est pas reconnu sous Windows, utiliser le terminal **MSYS2 MinGW 64-bit**.

---

### Étape 3 — Générer l'espace d'états

Cette commande exécute le model checker et sérialise le résultat dans un fichier binaire FlatBuffers.

**Linux :**

```bash
./executable_name save state_space
```

**Windows :**

```bat
.\executable_name.exe save state_space
```

Le fichier `state_space` est créé dans le répertoire courant.

---

### Étape 4 — Lancer la visualisation

```bash
python run_visualization.py state_space
```

Sur Linux si `python` pointe vers Python 2 :

```bash
python3 run_visualization.py state_space
```

Une fois les deux serveurs démarrés (Flask sur le port 8051, Dash sur le port 8050), ouvrir dans le navigateur :

```
http://127.0.0.1:8050
```

> Le démarrage peut prendre quelques secondes — le serveur précharge tous les états et voisins en mémoire avant d'accepter des connexions.

---

## Interface

| Contrôle                 | Rôle                                                 |
| ------------------------ | ---------------------------------------------------- |
| **Navigation dynamique** | Charge les nœuds à la volée autour du viewport       |
| **Navigation statique**  | Affiche le graphe complet avec layout BFS précalculé |
| **Prédecesseurs**        | Affiche uniquement les arcs entrants                 |
| **Successeurs**          | Affiche uniquement les arcs sortants                 |
| **Préd + Succ**          | Affiche les deux directions (défaut)                 |
| **ID état + Afficher**   | Démarre la visualisation depuis un état précis       |
| Molette / pinch          | Zoom                                                 |
| Clic-glisser             | Déplacer la vue                                      |
| Flèches clavier          | Déplacer la vue                                      |
| `+` / `-` clavier        | Zoom                                                 |
| Glisser un nœud          | Épingle le nœud à sa position                        |

---

## Dépannage

**`gcc : command not found` (Linux)**

```bash
sudo apt install gcc
```

**`flatcc : command not found`**  
Vérifier que flatcc est bien installé et dans le PATH. Sur Linux après `sudo make install`, relancer un terminal.

**`ModuleNotFoundError: No module named 'dash'`**

```bash
pip install dash flask
```

**`Fichier non trouvé : state_space`**  
Vérifier que l'étape 3 (génération du fichier binaire) a bien été exécutée et que le fichier `state_space` est présent dans le répertoire courant.

**Les fichiers `.h` générés ne correspondent pas au schéma**  
Relancer l'étape 1 après modification de `state_space_ta.fbs`. Les fichiers `*_reader.h`, `*_builder.h`, `*_verifier.h` et `flatbuffers_common_*.h` doivent être regénérés ensemble.

**Port 8050 ou 8051 déjà utilisé**

Linux :

```bash
lsof -i :8050
kill -9 <PID>
```

Windows (PowerShell) :

```powershell
netstat -ano | findstr :8050
taskkill /PID <PID> /F
```

Remplacer `8050` par `8051` si c'est l'autre port qui est bloqué. Remplacer `<PID>` par le numéro affiché dans la colonne de droite.

**Navigateur affiche "Entrez un ID d'état"**  
Normal — entrer `0` dans le champ ID et cliquer sur "Afficher".

```

```
