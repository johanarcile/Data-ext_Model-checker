# JSON parser

Ces fonctions permettent de parser les informations contenues dans un fichier au format json et de générer des copies des fichiers modèles du code source auxquels sont ajoutées les informations de modélisation de l'automate temporisé. 

---

## Utilisation

Un fichier au format json doit être créé et complété avec les informations de modélisation de l'automate comme indiqué dans le fichier explicatif du format json attendu.

Les fichiers modèles de génération doivent respecter des règles de formattage explicitée dans le fichier explicatif du format des modèles de génération. Ils doivent être regroupés dans un même dossier. Le nom donné au dossier n'impacte pas la compilation ou l'exécution.

Indications supplémentaires pour les utilisateurs sous Windows 11 ou 10 de versions ultérieures à 2004 : 

1. Aller dans paramètres>système>avancé, activer le mode développeur et sudo.
2. Aller dans paramètres>système>avancé>espace de travail virutels, activer platefroem d'ordinateur virtuel et plateforme de l'hyperviseur Windows.
3. Ouvrir Powershell ou l'invite de commande en mode administrateur et exécuter la commande suivante :
wsl --install
4. Télécharger MinGW-w64 depuis MSYS2 (https://www.msys2.org) et ouvrir l'exécutable wingw64.exe.
5. Une invite de commande sera ouverte dans laquelle il faudra exécuter les commandes suivantes : 
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
6. Vérifier l'installation avec l'exécution de la commande suivante (toujours dans l'invite de commande de MSYS2):
gcc --version
Cette commande doit retourner :
x86_64-w64-mingw32-gcc
7. Ajouter au PATH Windows le chemin d'accès de MinGW-w64 qui est généralement : C:\msys64\mingw64\bin.
8. Ouvrir un nouveau Powershell et exécuter la commande suivante :
gcc --version
Cette commande doit retourner:
x86_64-w64-mingw32-gcc

---

## Compilation et Exécution

1. La compilation : gcc main.c parse.c cJSON.c generation.c -o executable_name
2. L'exécution : ./executable_name json_path source_code_folder_path (les chemins d'accès doivent être complets : "C:Users\name\...\json_name.json" et "C:Users\name\...\source_code_folder" sous Windows et "/home/user_name/.../json_name.json" et "/home/user_name/.../source_code_folder" sous Linux)
3. Le chemin d'accès au dossier regroupant les modèles de génération peut être ajouté en tant que variable d'environnement nommée "ModelesLocation" si souhaité. Si elle n'existe pas, il sera demandé lors de l'exécution d'indiquer le chemin d'accès qui sera ajouté en tant que variable d'environnement utilisateur (pas en tant que variable d'environnement système). De même lorsque le chemin aura été modifié. L'enregistrement de la variable d'environnement ne pourra se faire que si l'utilisateur lançant l'exécution est celui disposant des droits administrateur. 
4. Le répertoire entré pour l'enregistrement des fichiers générés peut ne pas exister, il sera alors créé automatiquement.