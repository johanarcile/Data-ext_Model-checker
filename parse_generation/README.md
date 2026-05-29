# JSON parser

Ces fonctions permettent de parser les informations contenues dans un fichier au format json et de générer des copies des fichiers modèles du code source auxquels sont ajoutées les informations de modélisation de l'automate temporisé. 

---

## Utilisation

Un fichier au format json doit être créé et complété avec les informations de modélisation de l'automate comme indiqué dans le fichier explicatif du format json attendu. 
Les fichiers modèles de génération doivent respecter des règles de formattage explicitée dans le fichier explicatif du format des modèles de génération. Ils doivent être dans un même dossier dont le chemin d'accès est ajouté comme variable d'environnement nommée 'ModelesLocation'. 

---

## Compilation et Exécution

1. La compilation : gcc main.c parse.c cJSON.c generation.c -o executable_name
2. L'exécution : ./executable_name json_path source_code_folder_path (les chemins d'accès doivent être complets "C:User\name\...\json_name.json" et "C:User\name\...\source_code_folder\")
3. Autre possibilité ne nécessitant pas de compilation : ./program json_path source_code_folder_path