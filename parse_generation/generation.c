#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generation.h"

void generation_uthash_h(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/uthash.h") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/uthash.h");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/uthash.h") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/uthash.h");

    FILE* uthash_h = fopen(modelesPath, "r");
    if(!uthash_h){
        perror("Impossible d'ouvrir le fichier uthash.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier uthash.h

    FILE* uthash_h_copy = fopen(copyPath, "w");
    if(!uthash_h_copy){
        printf("Le fichier uthash.h n'a pas pu etre cree.\n");
        fclose(uthash_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int tampon;
    while(1){
        tampon = fgetc(uthash_h);
        if(tampon == EOF){
            if(feof(uthash_h)) break; //Détection de la fin du fichier uthash.h
            else{
                printf("Erreur de lecture du fichier uthash.h.\n");
                fclose(uthash_h);
                fclose(uthash_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier uthash.h
        }
        fputc(tampon, uthash_h_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(uthash_h);
    fclose(uthash_h_copy);
}

void generation_structure_DBM_h(char* copy_path, char* modeles_path, int nb_clocks){ 
    char* copyPath = malloc(strlen(copy_path) + strlen("/structure_DBM.h") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/structure_DBM.h");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/structure_DBM.h") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/structure_DBM.h");

    FILE* structure_DBM_h = fopen(modelesPath, "r");
    if(!structure_DBM_h){
        perror("Impossible d'ouvrir le fichier structure_DBM.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_DBM.h

    FILE* structure_DBM_h_copy = fopen(copyPath, "w");
    if(!structure_DBM_h_copy){
        printf("Le fichier structure_DBM.h n'a pas pu etre cree.\n");
        fclose(structure_DBM_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int find_line = 0;
    char* chaine_tampon = malloc(1000 * sizeof(char));
    while(1){
        char* result = fgets(chaine_tampon, 1000, structure_DBM_h);
        if(result == NULL){
            if(feof(structure_DBM_h)) break;
            else{
                printf("Erreur de lecture du fichier structure_DBM.h.\n");
                fclose(structure_DBM_h);
                fclose(structure_DBM_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_DBM.h
        }
        else if(strchr(result, '\n') == NULL){
            printf("Erreur de taille : La ligne (%s) du fichier structure_DBM.h depasse 1000 caracteres.\n", chaine_tampon);
            fclose(structure_DBM_h);
            fclose(structure_DBM_h_copy);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne trop grande pour le tampon

        
        if(strcmp(chaine_tampon, "#define NB_CLOCKS structure_DBM_h_nbclocks\n") == 0){
            char convert[1000];
            snprintf(convert, sizeof(convert), "#define NB_CLOCKS %d\n", nb_clocks); //Création de la ligne à coller dans le fichier généré
            fputs(convert, structure_DBM_h_copy);
            find_line++;
        }
        else fputs(chaine_tampon, structure_DBM_h_copy); //Copie si la ligne n'est pas celle à modifier
    }

    if(find_line != 1){
        printf("Erreur de syntaxe : La ligne '#define NB_CLOCKS structure_DBM_nbclocks' n'a pas ete trouvee dans le fichier structure_DBM.h.\n");
        fclose(structure_DBM_h);
        fclose(structure_DBM_h_copy);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de la ligne à modifier

    free(copyPath);
    free(modelesPath);
    fclose(structure_DBM_h);
    fclose(structure_DBM_h_copy);
}

void generation_structure_variable_h(char* copy_path, char* modeles_path, int nb_define, line* def_variables_define, int** nb_clines_typedef, int nb_typedef_structure, int nb_typedef_alias, char*** label_typedef, line*** def_variables_typedef){
    char* copyPath = malloc(strlen(copy_path) + strlen("/structure_variable.h") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/structure_variable.h");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/structure_variable.h") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/structure_variable.h");

    FILE* structure_variable_h = fopen(modelesPath, "r");
    if(!structure_variable_h){
        perror("Impossible d'ouvrir le fichier structure_variable.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_variable.h

    FILE* structure_variable_h_copy = fopen(copyPath, "w");
    if(!structure_variable_h_copy){
        printf("Le fichier structure_variable.h n'a pas pu etre cree.\n");
        fclose(structure_variable_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int* find_line = calloc(4, sizeof(int));
    char** error_message = malloc(4 * sizeof(char*));
    error_message[0] = "'structure_variable_h_define'";
    error_message[1] = "'structure_variable_h_typedef_alias'";
    error_message[2] = "'structure_variable_h_typedef_structure'";

    char* chaine_tampon = malloc(1000 * sizeof(char));
    while(1){
        char* result = fgets(chaine_tampon, 1000, structure_variable_h);
        if(result == NULL){
            if(feof(structure_variable_h)) break;
            else{
                printf("Erreur de lecture du fichier structure_variable.h.\n");
                fclose(structure_variable_h);
                fclose(structure_variable_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_variable.h
        }
        else if(strchr(result, '\n') == NULL){
            printf("Erreur de taille : La ligne (%s) du fichier structure_variable.h depasse 1000 caracteres.\n", chaine_tampon);
            fclose(structure_variable_h);
            fclose(structure_variable_h_copy);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne trop grande pour le tampon

        if(strcmp(chaine_tampon, "structure_variable_h_define\n") == 0){
            for(int i = 0; i < nb_define; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "%s\n", def_variables_define[i]);
                fputs(convert, structure_variable_h_copy);
            } //Ajout des lignes de code de déclarations dont la comparaison est voulue sur les adresses et des constantes symboliques
            fputs("\n", structure_variable_h_copy);
            find_line[0] = 1; //Mémorise la présence de la chaine à remplacer dans le fichier modèle
        }

        else if(strcmp(chaine_tampon, "structure_variable_h_typedef_alias\n") == 0){
            for(int i = 0; i < nb_typedef_alias; i++){
                for(int j = 0; j < nb_clines_typedef[1][i]; j++){
                    char convert[1000];
                    snprintf(convert, sizeof(convert), "%s\n", def_variables_typedef[1][i][j]);
                    fputs(convert, structure_variable_h_copy);
                }
            } //Ajout des lignes de code de déclaration des alias dont la comparaison est souhaitée sur les valeurs
            if(nb_typedef_alias != 0) fputs("\n", structure_variable_h_copy);
            find_line[1] = 1; //Mémorise la présence de la chaine à remplacer dans le fichier modèle
        }

        else if(strcmp(chaine_tampon, "structure_variable_h_typedef_structure\n") == 0){
            for(int i = 0; i < nb_typedef_structure; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "typedef struct %s {\n", label_typedef[0][i]);
                fputs(convert, structure_variable_h_copy);
                for(int j = 0; j < nb_clines_typedef[0][i]; j++){
                    char convertBis[1000];
                    snprintf(convertBis, sizeof(convertBis), "%s\n", def_variables_typedef[0][i][j]);
                    fputs(convertBis, structure_variable_h_copy);
                }
                char convertBisS[1000];
                snprintf(convertBisS, sizeof(convertBisS), "} %s;\n", label_typedef[0][i]);
                fputs(convertBisS, structure_variable_h_copy);
                fputs("\n", structure_variable_h_copy);
            } //Ajout des lignes de code de déclaration de la structure Variable et de celles dont la comparaison est souhaitée sur les valeurs
            find_line[2] = 1;
        }

        else fputs(chaine_tampon, structure_variable_h_copy); //Copie si la ligne n'est pas une ligne à modifier
    }

    int error = 0;
    for(int i = 0; i < 3; i++){
        if(find_line[i] != 1){
            printf("Erreur de syntaxe : La ligne %s n'a pas ete trouvee dans le fichier structure_variable.h.\n", error_message[i]);
            error++;
        } //Détection de l'absence des lignes à modifier
    }
    if(error != 0){
        fclose(structure_variable_h);
        fclose(structure_variable_h_copy);
        exit(EXIT_FAILURE);
    } //Si l'absence d'une ligne est détectée, l'exécution est stoppée

    free(copyPath);
    free(modelesPath);
    free(find_line);
    free(error_message);
    fclose(structure_variable_h);
    fclose(structure_variable_h_copy);
}

void generation_structure_ta_h(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/structure_ta.h") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/structure_ta.h");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/structure_ta.h") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/structure_ta.h");

    FILE* structure_ta_h = fopen(modelesPath, "r");
    if(!structure_ta_h){
        perror("Impossible d'ouvrir le fichier structure_ta.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_ta.h

    FILE* structure_ta_h_copy = fopen(copyPath, "w");
    if(!structure_ta_h_copy){
        printf("Le fichier structure_ta.h n'a pas pu etre cree.\n");
        fclose(structure_ta_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int tampon;
    while(1){
        tampon = fgetc(structure_ta_h);
        if(tampon == EOF){
            if(feof(structure_ta_h)) break; //Détection de la fin du fichier structure_ta.h
            else{
                printf("Erreur de lecture du fichier structure_ta.h.\n");
                fclose(structure_ta_h);
                fclose(structure_ta_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_ta.h
        }
        fputc(tampon, structure_ta_h_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(structure_ta_h);
    fclose(structure_ta_h_copy);
}

void generation_structure_state_space_ta_h(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/structure_state_space.h") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/structure_state_space_ta.h");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/structure_state_space.h") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/structure_state_space_ta.h");

    FILE* structure_state_space_ta_h = fopen(modelesPath, "r");
    if(!structure_state_space_ta_h){
        perror("Impossible d'ouvrir le fichier structure_state_space_ta.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_state_space_ta.h

    FILE* structure_state_space_ta_h_copy = fopen(copyPath, "w");
    if(!structure_state_space_ta_h_copy){
        printf("Le fichier structure_state_space_ta.h n'a pas pu etre cree.\n");
        fclose(structure_state_space_ta_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int tampon;
    while(1){
        tampon = fgetc(structure_state_space_ta_h);
        if(tampon == EOF){
            if(feof(structure_state_space_ta_h)) break; //Détection de la fin du fichier structure_state_space_ta.h
            else{
                printf("Erreur de lecture du fichier structure_state_space_ta.h.\n");
                fclose(structure_state_space_ta_h);
                fclose(structure_state_space_ta_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_state_space_ta.h
        }
        fputc(tampon, structure_state_space_ta_h_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(structure_state_space_ta_h);
    fclose(structure_state_space_ta_h_copy);
}

void generation_DBM_c(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/DBM.c") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/DBM.c");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/DBM.c") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/DBM.c");

    FILE* DBM_c = fopen(modelesPath, "r");
    if(!DBM_c){
        perror("Impossible d'ouvrir le fichier DBM.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier DBM.c

    FILE* DBM_c_copy = fopen(copyPath, "w");
    if(!DBM_c_copy){
        printf("Le fichier DBM.c n'a pas pu etre cree.\n");
        fclose(DBM_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int tampon;
    while(1){
        tampon = fgetc(DBM_c);
        if(tampon == EOF){
            if(feof(DBM_c)) break; //Détection de la fin du fichier DBM.c
            else{
                printf("Erreur de lecture du fichier DBM.c.\n");
                fclose(DBM_c);
                fclose(DBM_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier DBM.c
        }
        fputc(tampon, DBM_c_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(DBM_c);
    fclose(DBM_c_copy);
}

void generation_variable_c(char* copy_path, char* modeles_path, int** nb_clines_typedef, int nb_typedef_structure, int nb_typedef_alias, char*** label_typedef, line*** def_variables_typedef, int*** dim_elements_typedef_variables){
    char* copyPath = malloc(strlen(copy_path) + strlen("/variable.c") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/variable.c");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/variable.c") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/variable.c");

    FILE* variable_c = fopen(modelesPath, "r");
    if(!variable_c){
        perror("Impossible d'ouvrir le fichier variable.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier variable.c

    FILE* variable_c_copy = fopen(copyPath, "w");
    if(!variable_c_copy){
        printf("Le fichier variable.c n'a pas pu etre cree.\n");
        fclose(variable_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int* find_line = calloc(3, sizeof(int)); //Tableau stockant 0 ou 1 pour chaque ligne trouvée
    char** error_message = malloc(3 * sizeof(char*)); //Tableau contenant les lignes de codes pour le message d'erreur lorsque l'une d'entre elles n'est pas trouvée
    error_message[0] = "'  variable_c_equal_function'";
    error_message[1] = "'  variable_c_print_function'";
    
    line*** copy = malloc(3 * sizeof(line**)); //Copie du tableau des lignes de code des typedef types pour éviter une perte d'information
    copy[0] = malloc((nb_typedef_structure+1) * sizeof(line*));
    copy[1] = malloc((nb_typedef_alias+1) * sizeof(line*));
    for(int i = 0; i < nb_typedef_structure; i++){
        copy[0][i] = malloc((nb_clines_typedef[0][i]+1) * sizeof(line));
        for(int j = 0; j < nb_clines_typedef[0][i]; j++){
            copy[0][i][j] = strdup(def_variables_typedef[0][i][j]);
        }
    }
    for(int i = 0; i < nb_typedef_alias; i++){
        copy[1][i] = malloc((nb_clines_typedef[1][i]+1) * sizeof(line));
        for(int j = 0; j < nb_clines_typedef[1][i]; j++){
            copy[1][i][j] = strdup(def_variables_typedef[1][i][j]);
        }
    } //Copie des lignes de code de Variable

    char** types_two_words = malloc(6 * sizeof(char*)); //Tableau contenant les deuxièmes mots que l'on peut trouver dans les noms des types en c
    types_two_words[0] = "int";
    types_two_words[1] = "char";
    types_two_words[2] = "short";
    types_two_words[3] = "long";
    types_two_words[4] = "double";

    char** types_three_words = malloc(3 * sizeof(char*)); //Tableau contenant les troisièmes mots que l'on peut trouver dans les noms des types en c
    types_three_words[0] = "int";
    types_three_words[1] = "long";

    char* types_four_words = "int"; //Quatrième mot que l'on peut trouver dans les noms des types en c

    char seps[] = " [;]*"; //Chaine de caractères contenant les caractères de séparation pour le découpage
    char* field_names[1000] = {NULL}; //Tableau permettant de récupérer les noms des champs de la structure Variable
    char* field_types[1000] = {NULL}; //Tableau permettant de récupérer le nom du type de chaque champ de la structure Variable
    char** size_tab[1000] = {NULL}; //Tableau permettant de récupérer les tailles des tableaux
    char** size_tab_type[1000] = {NULL}; //Tableau qui permet de savoir si la taille de la dimension est définie dynamiquement ou statiquement
    char* field_types_def[1000] = {NULL}; //Tableau qui mémorise si le type du champ de la structure Variable est un alias, une structure ou un type natif

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_structure-1]; i++){
        char *split_string = strtok(copy[0][nb_typedef_structure-1][i], seps); //Remplace les caractères indiqués par seps trouvés dans la chaine copy[0][nb_typedef_structure-1][i] (ligne de code de la structure Variable) par le caractère '\0'
        int enum_words = 0; //Variable pour savoir quel type de texte est attendu lors du parcours de la chaine découpée (type : 0, nom : 1 ou dimension du champ : supérieur à 1)
        int count_types_words = 0; //Variable pour compter le nombre de mots dans le type du champ
        int size_count = 0; //Variable pour compter le nombre de constantes symboliques utilisées pour mémoriser les tailles des dimensions des tableaux
        char* temp[1000]; //Tableau temporaire pour mémoriser les noms des variables mémorisant les tailles des dimensions des tableaux

        while(split_string != NULL){
            if((count_types_words > 0)&&(enum_words == 0)){
                switch(count_types_words){
                    case 1 : {
                        for(int j = 0; j < 5; j++){
                            if(strcmp(split_string, types_two_words[j]) == 0){
                                enum_words = 0;
                                break;
                            }
                            else enum_words = 1;
                        }
                        break;
                    } //Détecte si le deuxième mot appartient au type ou au nom du champ
                    case 2 : {
                        if((strcmp(split_string, types_three_words[0]) != 0)&&(strcmp(split_string, types_three_words[1]) != 0)) enum_words = 1;
                        break;
                    } //Détecte si le troisième mot appartient au type ou au nom du champ
                    case 3 : {
                        if(strcmp(split_string, types_four_words) != 0) enum_words = 1;
                        break;
                    } //Détecte si le quatrième mot appartient au type ou au nom du champ
                    default : {
                        enum_words = 1;
                        break;
                    } //Incrémente enum_words pour indiquer que le mot trouvé est le nom du champ
                }
            } //Détection d'un mot appartenant au type ou au nom du champ

            if(enum_words == 0){
                if(count_types_words == 0) field_types[i] = strdup(split_string); //Récupération du premier mot du type du champ
                else{
                    char* realloc_s = realloc(field_types[i], strlen(field_types[i])+strlen(split_string)+1);
                    if(realloc_s == NULL){
                        printf("Erreur de reallocation memoire pour la chaine de caracteres recueillant le nom du type du champ %s de la structure Variable (field_types).\n", field_names[i]);
                        fclose(variable_c);
                        fclose(variable_c_copy);
                        exit(EXIT_FAILURE);
                    }
                    field_types[i] = realloc_s;
                    strcat(field_types[i], split_string); //Concaténation des autres mots du type du champ au mots précédents
                }
                count_types_words++; //Incrémentation du nombre de mots trouvés désignant le type du champ
            }
            else if(enum_words == 1) {
                field_names[i] = strdup(split_string); //Récupération du nom du champ
                enum_words++;
            }
            else if(enum_words > 1){
                if(strcmp(split_string, "//") == 0) break; //Sortie de la boucle si la suite correspond à un commentaire
                else{
                    temp[size_count] = strdup(split_string);
                    size_count++;
                } //Récupération du nom de la variable sauvegardant la taille d'une dimension du champ
                enum_words++;
            }
            split_string = strtok(NULL, seps); //Permet de passer au mot suivant de la chaine copy[0][nb_typedef_structure-1][i]
        } //Parcours mot par mot de la chaine modifiée copy[0][nb_typedef_structure-1][i]
        
        field_types_def[i] = "n"; //Indique que le type du champ n'est ni un alias ni une structure dont la comparaison est souhaitée sur les valeurs
        for(int j = 0; j < nb_typedef_alias; j++){
            if(strcmp(field_types[i], label_typedef[1][j]) == 0){
                char temp_type[1000];
                snprintf(temp_type, sizeof(temp_type), "p%d", j); //Indique que le type du champ est un alias défini comme tel dans le json et sauvegarde sa position dans le tableau nb_typedef_alias
                field_types_def[i] = strdup(temp_type);
            }
        }
        for(int j = 0; j < nb_typedef_structure; j++){
            if(strcmp(field_types[i], label_typedef[0][j]) == 0){
                char temp_type[1000];
                snprintf(temp_type, sizeof(temp_type), "s%d", j); //Indique que le type du champ est une structure définie comme tel dans le json et sauvegarde sa position dans le tableau nb_typedef_structure
                field_types_def[i] = strdup(temp_type);
            }
        }

        int dim_size;
        dim_size = dim_elements_typedef_variables[0][nb_typedef_structure-1][i]; //Sauvegarde la dimension du champ de la structure Variable
        if(field_types_def[i][0] == 'p'){
            char* conv;
            int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
            dim_size += dim_elements_typedef_variables[1][num_t][0]; //S'il s'agit d'un alias, la dimension de l'alias est ajoutée à la dimension du champ de Variable
        }
        size_tab[i] = malloc((dim_size+1) * sizeof(char*));
        size_tab_type[i] = malloc((dim_size+1) * sizeof(char*)); //Allocation de l'espace mémoire pour les tableaux de sauvegarde des noms des variables mémorisant la taille des dimensions

        if(size_count == 0){
            for(int j = 0; j < dim_size; j++){
                size_tab[i][j] = NULL;
                size_tab_type[i][j] = NULL;
            }
        } //Initialisation de size_tab et size_tab_type si aucune constante symbolique ne mémorise la taille d'une dimension du tableau

        else {
            if(size_count > dim_size){
                printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles du champ %sde la structure Variable.\n", field_names[i]);
                fclose(variable_c);
                fclose(variable_c_copy);
                exit(EXIT_FAILURE);
            } //Détecte si trop de variables sont définies pour les tailles des dimensions du tableau

            for(int j = 0; j < dim_size; j++){
                if(j < size_count){
                    size_tab[i][j] = strdup(temp[j]);
                    size_tab_type[i][j] = "define"; //Mémorise que la variable est une constante symbolique
                } //Mémorisation des nom des variables étant des constantes symboliques
                else{
                    size_tab[i][j] = NULL;
                    size_tab_type[i][j] = NULL;
                } //Initialisation pour les cases restantes
            }
        } //Mémorisation des noms des constantes symboliques sauvegardant les tailles des dimensions du champ  
    }

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_structure-1]; i++){
        char* copy_name = strdup(field_names[i]); //Copie du nom du champ pour éviter des pertes d'informations
        char *split_name = strtok(copy_name, "_"); //Remplace les caractères '_' du nom du champ de la structure Variable (copy_name) par le caractère '\0'
        char* temp[1000]; //Tableau temporaire pour mémoriser chaque mot découpé du nom du champ
        int enum_words = 0; //Variable pour parcourir la chaine modifiée field_names[i]
        while(split_name != NULL){
            temp[enum_words] = strdup(split_name);
            split_name = strtok(NULL, "_"); //Permet de passer au mot suivant de la chaine copy_name
            enum_words++;
        } //Parcours mot par mot de la chaine modifiée copy_name

        if(enum_words == 1) continue; //Passe à l'itération de la boucle suivante si le nom ne contient pas le caractère '_'

        if(strcmp(temp[1], "size") == 0){
            switch(enum_words){
                case 2 : {
                    for(int j = 0; j < nb_clines_typedef[0][nb_typedef_structure-1]; j++){
                        if(strcmp(field_names[j], temp[0]) == 0){
                            if(dim_elements_typedef_variables[0][nb_typedef_structure-1][j] == 0){
                                printf("Erreur de syntaxe : Le champ %s de la structure Variable n'est pas un tableau. Le nom du champ %s de la structure Variable doit etre modifie.\n", field_names[j], field_names[i]);
                                free(copy_name);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            } //Détection d'une variable sauvegardant la dimension d'un champ n'étant pas un tableau

                            if(size_tab[j][0] != NULL){
                                printf("Erreur de syntaxe : La taille de %s est definie deux fois par %s et %s", field_names[j], size_tab[j][0], field_names[i]);
                                free(copy_name);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            } //Détection d'un conflit de variable sauvegardant la dimension du tableau 

                            size_tab[j][0] = field_names[i];
                            size_tab_type[j][0] = "*"; //Mémorise que la variable est un champ de la structure Variable
                            break;
                        }
                    }
                    break;
                } //Détection d'une variable sauvegardant la première dimension du tableau 

                case 3 : {
                    char* convert; //Chaine tampon pour convertir la chaine temp[2] en entier
                    int taille = (int)strtol(temp[2], &convert, 10); //Conversion du nombre de la variable en entier
                    if(*convert != '\0') break; //Détection d'une erreur de conversion

                    for(int j = 0; j < nb_clines_typedef[0][nb_typedef_structure-1]; j++){
                        if(strcmp(field_names[j], temp[0]) == 0){
                            int dim_size = dim_elements_typedef_variables[0][nb_typedef_structure-1][j];
                            if(field_types_def[j][0] == 'p'){
                                char* conv;
                                int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
                                dim_size += dim_elements_typedef_variables[1][num_t][0];
                            } //Ajoute à la dimension du champ de la structure Variable la dimension de l'alias

                            if(taille >= dim_size){
                                printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles du champ %s de la structure Variable.\n", field_names[j]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            } //Détecte si trop de variables sont définies pour les tailles des dimensions du tableau

                            if(size_tab[j][taille] != NULL){
                                printf("Erreur de syntaxe : La taille de la dimension %d de %s est definie deux fois par %s et %s", taille, field_names[j], size_tab[j][taille], field_names[i]);
                                free(copy_name);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            } //Détection d'un conflit de variable sauvegardant la taille de la dimension du tableau

                            size_tab[j][taille] = field_names[i];
                            size_tab_type[j][taille] = "*"; //Mémorise que la variable est un champ de la structure Variable
                            break;
                        }
                    }
                    break;
                } //Détection d'une variable sauvegardant une autre dimension du tableau

                default : break;
            }
        } //Détection du mot size indiquant que la variable considérée permet de sauvegarder une dimension d'un des tableaux dynamiques de Variable
        free(copy_name);
    }

    char** field_struct_names[1000] = {NULL}; //Equivalent de field_names pour les autres structures que Variable
    char** field_struct_types[1000] = {NULL}; //Equivalent de field_types pour les autres structures que Variable
    char*** size_struct_tab[1000] = {NULL}; //Equivalent de size_tab pour les autres structures que Variable
    char*** size_struct_tab_type[1000] = {NULL}; //Equivalent de size_tab_type pour les autres structures que Variable

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_structure-1]; i++){
        if(field_types_def[i][0] == 's'){
            char* conv;
            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10); //Conversion du nombre en entier

            field_struct_names[i] = malloc((nb_clines_typedef[0][num_t]+1) * sizeof(char*));
            field_struct_types[i] = malloc((nb_clines_typedef[0][num_t]+1) * sizeof(char*));
            size_struct_tab[i] = malloc((nb_clines_typedef[0][num_t]+1) * sizeof(char*));
            size_struct_tab_type[i] = malloc((nb_clines_typedef[0][num_t]+1) * sizeof(char*));

            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                char *split_struct = strtok(copy[0][num_t][j], seps); //Remplace les caractères seps par le caractère '\0' dans la ligne de code du champ de la structure
                int enum_words = 0; 
                int count_types_words = 0;
                int size_count = 0;
                char* temp[1000];

                while(split_struct != NULL){
                    if((count_types_words > 0)&&(enum_words == 0)){
                        switch(count_types_words){
                            case 1 : {
                                for(int k = 0; k < 5; k++){
                                    if(strcmp(split_struct, types_two_words[k]) == 0){
                                        enum_words = 0;
                                        break;
                                    }
                                    else enum_words = 1;
                                }
                                break;
                            }
                            case 2 : {
                                if((strcmp(split_struct, types_three_words[0]) != 0)&&(strcmp(split_struct, types_three_words[1]) != 0)) enum_words = 1;
                                break;
                            }
                            case 3 : {
                                if(strcmp(split_struct, types_four_words) != 0) enum_words = 1;
                                break;
                            }
                            default : {
                                enum_words = 1;
                                break;
                            }
                        }
                    } //Détection des types c en plusieurs mots

                    if(enum_words == 0){
                        if(count_types_words == 0) field_struct_types[i][j] = strdup(split_struct);
                        else{
                            char* realloc_st = realloc(field_struct_types[i][j], strlen(field_struct_types[i][j])+strlen(split_struct)+1);
                            if(realloc_st == NULL){
                                printf("Erreur de reallocation memoire pour la chaine de caracteres recueillant le nom du type du champ %s de la structure %s (field_struct_types).\n", field_struct_names[i][j], label_typedef[0][num_t]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                            field_struct_types[i][j] = realloc_st;
                            strcat(field_struct_types[i][j], split_struct);
                        }
                        count_types_words++;
                    }
                    else if(enum_words == 1){
                        field_struct_names[i][j] = strdup(split_struct);
                        enum_words++;
                    }
                    else if (enum_words > 1){
                        if(strcmp(split_struct, "//") == 0) break;
                        else{
                            temp[size_count] = strdup(split_struct);
                            size_count++;
                        }
                        enum_words++;
                    }
                    split_struct = strtok(NULL, seps);
                }

                size_struct_tab[i][j] = malloc((dim_elements_typedef_variables[0][num_t][j]+1) * sizeof(char*));
                size_struct_tab_type[i][j] = malloc((dim_elements_typedef_variables[0][num_t][j]+1) * sizeof(char*));
                if(size_count == 0){
                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                        size_struct_tab[i][j][k] = NULL;
                        size_struct_tab_type[i][j][k] = NULL;
                    }
                }

                else{
                    if(size_count > dim_elements_typedef_variables[0][num_t][j]){
                        printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles du champ %s de la structure Variable.\n", field_names[i]);
                        fclose(variable_c);
                        fclose(variable_c_copy);
                        exit(EXIT_FAILURE);
                    }
                    int count_dim = 0;
                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                        if(count_dim < size_count){
                            size_struct_tab[i][j][k] = strdup(temp[k]);
                            size_struct_tab_type[i][j][k] = "define";
                        }
                        else{
                            size_struct_tab[i][j][k] = NULL;
                            size_struct_tab_type[i][j][k] = NULL;
                        }
                    }
                }
            }
        }
    }

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_structure-1]; i++){
        if(field_types_def[i][0] == 's'){
            char* conv;
            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);
            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                char* copy_name = strdup(field_struct_names[i][j]);
                char *split_name = strtok(copy_name, "_");
                char* temp[1000];
                int enum_words = 0;
                while(split_name != NULL){
                    temp[enum_words] = strdup(split_name);
                    split_name = strtok(NULL, "_");
                    enum_words++;
                }

                if(enum_words == 1) continue;

                if(strcmp(temp[1], "size") == 0){
                    switch(enum_words){
                        case 2 : {
                            for(int k = 0; k < nb_clines_typedef[0][num_t]; k++){
                                if(strcmp(field_struct_names[i][k], temp[0]) == 0){
                                    if(dim_elements_typedef_variables[0][num_t][k] == 0){
                                        printf("Erreur de syntaxe : Le champ %s du champ %s de la structure Variable n'est pas un tableau. Le nom du champ %s de la structure %s doit etre modifie.\n", field_struct_names[i][k], field_names[i], field_struct_names[i][j], label_typedef[0][num_t]);
                                        free(copy_name);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                    if(size_struct_tab[i][k][0] != NULL){
                                        printf("Erreur de syntaxe : La taille de %s est definie deux fois par %s et %s", field_struct_names[i][k], size_struct_tab[i][k][0], field_struct_names[i][j]);
                                        free(copy_name);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }

                                    size_struct_tab[i][k][0] = field_struct_names[i][j];
                                    size_struct_tab_type[i][k][0] = "*";
                                    break;
                                }
                            }
                            break;
                        }

                        case 3 : {
                            char* convert;
                            int taille = (int)strtol(temp[2], &convert, 10);
                            if(*convert != '\0') break;
                            for(int k = 0; k < nb_clines_typedef[0][nb_typedef_structure-1]; k++){
                                if(strcmp(field_struct_names[i][k], temp[0]) == 0){
                                    if(taille >= dim_elements_typedef_variables[0][num_t][k]){
                                        printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles du champ %s du champ %s de la structure Variable.\n", field_struct_names[i][j], field_names[i]);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                    if(size_struct_tab[i][k][taille] != NULL){
                                        printf("Erreur de syntaxe : La taille de la dimension %d de %s est definie deux fois par %s et %s", taille, field_struct_names[i][k], size_struct_tab[i][k][taille], field_struct_names[i][j]);
                                        free(copy_name);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }

                                    size_struct_tab[i][k][taille] = field_struct_names[i][j];
                                    size_struct_tab_type[i][k][taille] = "*";
                                    break;
                                }
                            }
                            break;
                        }

                        default : break;
                    }
                }
                free(copy_name);
            }
        }
    }

    //Libération de la mémoire pour copy
    for(int i = 0; i < nb_typedef_structure; i++){
        for(int j = 0; j < nb_clines_typedef[0][i]; j++){
            free(copy[0][i][j]);
        }
        free(copy[0][i]);
    }
    for(int i = 0; i < nb_typedef_alias; i++){
        for(int j = 0; j < nb_clines_typedef[1][i]; j++){
            free(copy[1][i][j]);
        }
        free(copy[1][i]);
    }
    free(copy[0]);
    free(copy[1]);
    free(copy);

    char* chaine_tampon = malloc(1000 * sizeof(char));
    while(1){
        char* result = fgets(chaine_tampon, 1000, variable_c);
        if(result == NULL){
            if(feof(variable_c)) break;
            else{
                printf("Erreur de lecture du fichier variable.c.\n");
                fclose(variable_c);
                fclose(variable_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier variable.c
        }
        else if(strchr(result, '\n') == NULL){
            printf("Erreur de taille : La ligne (%s) du fichier variable.c depasse 1000 caracteres.\n", chaine_tampon);
            fclose(variable_c);
            fclose(variable_c_copy);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne trop grande pour le tampon
 
        if(strcmp(chaine_tampon, "  variable_c_equal_function\n") == 0){
            for(int i = 0; i < nb_clines_typedef[0][nb_typedef_structure-1]; i++){
                int is_struct;
                int dim_l = dim_elements_typedef_variables[0][nb_typedef_structure-1][i];
                if(strcmp(field_types_def[i], "n") == 0){
                    is_struct = 0;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s de la structure Variable.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    } //Détection d'une absence de variables définissant des tailles du champ de la structure Variable
                }
                else if(field_types_def[i][0] == 's'){
                    is_struct = 1;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s de la structure Variable.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    } //Détection d'une absence de variables définissant des tailles du champ de la structure Variable
                }
                else{
                    is_struct = 0;
                    char* conv;
                    int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
                    dim_l += dim_elements_typedef_variables[1][num_t][0];
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s de la structure Variable.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    } //Détection d'une absence de variables définissant des tailles du champ de la structure Variable
                }

                if(dim_l == 0){
                    switch(is_struct){
                        case 0 : {
                            char convert[1000];
                            snprintf(convert, sizeof(convert), "  if(v1->%s != v2->%s){\n", field_names[i], field_names[i]);
                            fputs(convert, variable_c_copy);
                            fputs("    return false;\n", variable_c_copy);
                            fputs("  }\n", variable_c_copy);
                            break;
                        } //Cas pour un champ de type alias ou natif

                        case 1 : {
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                    if(size_struct_tab[i][j][k] == NULL){
                                        printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s du champ %s de la structure Variable.\n", field_struct_names[i][j], field_names[i]);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                } //Détection d'une absence de variables définissant des tailles du champ d'une structure

                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "  if(v1->%s.%s != v2->%s.%s){\n", field_names[i], field_struct_names[i][j], field_names[i], field_struct_names[i][j]);
                                    fputs(convert, variable_c_copy);
                                    fputs("    return false;\n", variable_c_copy);
                                    fputs("  }\n", variable_c_copy);
                                } //Cas où le champ de la structure n'est pas un tableau

                                else{
                                    char* indent[1000] = {NULL};
                                    indent[0] = "  ";
                                    char dim[1000] = "";
                                    char level = 'i';

                                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                        char convert[1000];
                                        char temp[1000];
                                        if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent[k], level, level, size_struct_tab[i][j][k], level);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < v1->%s.%s; %c++){\n", indent[k], level, level, field_names[i], size_struct_tab[i][j][k], level);
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level);
                                        strcat(dim, temp);
                                        indent[k+1] = strdup(indent[k]);
                                        strcat(indent[k+1], "  ");
                                        level++;
                                    } //Génération des boucles itératives

                                    for(int k = 0; k < 3; k++){
                                        char convert[1000];
                                        switch(k){
                                            case 0 :
                                                snprintf(convert, sizeof(convert), "%sif(v1->%s.%s%s != v2->%s.%s%s){\n", indent[dim_elements_typedef_variables[0][num_t][j]], field_names[i], field_struct_names[i][j], dim, field_names[i], field_struct_names[i][j], dim);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            case 1 :
                                                snprintf(convert, sizeof(convert), "%s  return false;\n", indent[dim_elements_typedef_variables[0][num_t][j]]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            default : 
                                                snprintf(convert, sizeof(convert), "%s}\n", indent[dim_elements_typedef_variables[0][num_t][j]]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                        }
                                    } //Génération du contenu des boucles

                                    for(int k = (dim_elements_typedef_variables[0][num_t][j]-1); k >= 0 ; k--){
                                        char convert[1000];
                                        snprintf(convert, sizeof(convert), "%s}\n", indent[k]);
                                        fputs(convert, variable_c_copy);
                                    }
                                }
                            }
                            break;
                        } //Cas où le champ de la structure est un tableau

                        default : break;
                    } //Cas pour un champ de type structure 
                } //Cas où le champ de la structure Variable n'est pas un tableau

                else{
                    char* indent[1000] = {NULL};
                    indent[0] = "  ";
                    char dim[1000] = "";
                    char level = 'i';

                    for(int j = 0; j < dim_l; j++){
                        char convert[1000];
                        char temp[1000];
                        if(strcmp(size_tab_type[i][j], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent[j], level, level, size_tab[i][j], level);
                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < v1->%s; %c++){\n", indent[j], level, level, size_tab[i][j], level);
                        fputs(convert, variable_c_copy);
                        snprintf(temp, sizeof(temp), "[%c]", level);
                        strcat(dim, temp);
                        indent[j+1] = strdup(indent[j]);
                        strcat(indent[j+1], "  ");
                        level++;
                    } //Génération des ouvertures des boucles itératives

                    switch(is_struct){
                        case 0 : {
                            for(int j = 0; j < 3; j++){
                                char convert[1000];
                                switch(j){
                                    case 0 :
                                        snprintf(convert, sizeof(convert), "%sif(v1->%s%s != v2->%s%s){\n", indent[dim_l], field_names[i], dim, field_names[i], dim);
                                        fputs(convert, variable_c_copy);
                                        break;
                                    case 1 :
                                        snprintf(convert, sizeof(convert), "%s  return false;\n", indent[dim_l]);
                                        fputs(convert, variable_c_copy);
                                        break;
                                    default : 
                                        snprintf(convert, sizeof(convert), "%s}\n", indent[dim_l]);
                                        fputs(convert, variable_c_copy);
                                        break;
                                }
                            }
                            break;
                        } //Génération du contenu des boucles itératives pour un champ de type alias ou natif

                        case 1 : {
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                    if(size_struct_tab[i][j][k] == NULL){
                                        printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s du champ %s de la structure Variable.\n", field_struct_names[i][j], field_names[i]);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                } //Détection d'une absence de variables définissant des tailles du champ d'une structure

                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    for(int k = 0; k < 3; k++){
                                        char convert[1000];
                                        switch(k){
                                            case 0 : {
                                                snprintf(convert, sizeof(convert), "%sif(v1->%s%s.%s != v2->%s%s.%s){\n", indent[dim_l], field_names[i], dim, field_struct_names[i][j], field_names[i], dim, field_struct_names[i][j]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            }
                                            case 1 : {
                                                snprintf(convert, sizeof(convert), "%s  return false;\n", indent[dim_l]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            }
                                            default : {
                                                snprintf(convert, sizeof(convert), "%s}\n", indent[dim_l]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            }
                                        }
                                    }
                                } //Cas où le champ de la structure n'est pas un tableau

                                else{
                                    char* indent_s[1000] = {NULL};
                                    char dim_s[1000] = "";
                                    char level_s = level;
                                    int dim_l_s = dim_l;

                                    for(int k = 0; k <= dim_l; k++){
                                        indent_s[k] = strdup(indent[k]);
                                    }

                                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                        char convert[1000];
                                        char temp[1000];
                                        if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent_s[dim_l_s], level_s, level_s, size_struct_tab[i][j][k], level_s);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < v1->%s%s.%s; %c++){\n", indent_s[dim_l_s], level_s, level_s, field_names[i], dim, size_struct_tab[i][j][k], level_s);
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level_s);
                                        strcat(dim_s, temp);
                                        indent_s[dim_l_s+1] = strdup(indent_s[dim_l_s]);
                                        strcat(indent_s[dim_l_s+1], "  ");
                                        level_s++;
                                        dim_l_s++;
                                    } //Génération des boucles itératives

                                    for(int k = 0; k < 3; k++){
                                        char convert[1000];
                                        switch(k){
                                            case 0 : {
                                                snprintf(convert, sizeof(convert), "%sif(v1->%s%s.%s%s != v2->%s%s.%s%s){\n", indent_s[dim_l_s], field_names[i], dim, field_struct_names[i][j], dim_s, field_names[i], dim, field_struct_names[i][j], dim_s);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            }
                                            case 1 : {
                                                snprintf(convert, sizeof(convert), "%s  return false;\n", indent_s[dim_l_s]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            }
                                            default : {
                                                snprintf(convert, sizeof(convert), "%s}\n", indent_s[dim_l_s]);
                                                fputs(convert, variable_c_copy);
                                                break;
                                            }
                                        }
                                    } //Génération du contenu des boucles itératives

                                    for(int k = (dim_elements_typedef_variables[0][num_t][j]-1); k >= 0; k--){
                                        char convert[1000];
                                        dim_l_s--;
                                        snprintf(convert, sizeof(convert), "%s}\n", indent_s[dim_l_s]);
                                        fputs(convert, variable_c_copy);
                                    } //Génération de la fermeture des boucles itératives
                                } //Cas où le champ de la structure est un tableau
                            }
                            break;
                        } //Génération du contenu des boucles itératives pour un champ de type structure
                    }

                    for(int j = (dim_l-1); j >= 0 ; j--){
                        char convert[1000];
                        snprintf(convert, sizeof(convert), "%s}\n", indent[j]);
                        fputs(convert, variable_c_copy);
                    } //Génération de la fermeture des boucles itératives

                } //Génération du code s'il s'agit d'un tableau
            }
            fputs("  return true;\n", variable_c_copy);
            find_line[0] = 1;
        }

        else if(strcmp(chaine_tampon, "  variable_c_print_function\n") == 0){
            for(int i = 0; i < nb_clines_typedef[0][nb_typedef_structure-1]; i++){
                int dim_l = dim_elements_typedef_variables[0][nb_typedef_structure-1][i];
                int is_struct;
                if(strcmp(field_types_def[i], "n") == 0){
                    is_struct = 0;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s de la structure Variable.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
                else if(field_types_def[i][0] == 's'){
                    is_struct = 1;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s de la structure Variable.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
                else{
                    is_struct = 0;
                    char* conv;
                    int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
                    dim_l += dim_elements_typedef_variables[1][num_t][0];
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s de la structure Variable.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }

                    char* copy_line = strdup(def_variables_typedef[1][num_t][0]);
                    char *split_line = strtok(copy_line, " ;*");
                    int count_types_words = 0; 
                    int count_line = 0;
                    while(split_line != NULL){
                        if(count_line == 0){
                            count_line++;
                            split_line = strtok(NULL, " ;*");
                            continue;
                        }
                        else if((count_line == 1)&&(count_types_words > 0)){
                            switch(count_types_words){
                                case 1 : {
                                    for(int j = 0; j < 5; j++){
                                        if(strcmp(split_line, types_two_words[j]) == 0){
                                            count_line = 1;
                                            break;
                                        }
                                        else count_line = 2;
                                    }
                                    break;
                                }
                                case 2 : {
                                    if((strcmp(split_line, types_three_words[0]) != 0)&&(strcmp(split_line, types_three_words[1]) != 0)) count_line = 2;
                                    break;
                                }
                                case 3 : {
                                    if(strcmp(split_line, types_four_words) != 0) count_line = 2;
                                    break;
                                }
                                default : {
                                    count_line = 2;
                                    break;
                                }
                            }
                        } 

                        if(count_line == 1){
                            if(count_types_words == 0) field_types[i] = strdup(split_line);
                            else{
                                char* realloc_sp = realloc(field_types[i], strlen(field_types[i])+strlen(split_line)+1);
                                if(realloc_sp == NULL){
                                    printf("Erreur de reallocation memoire pour la chaine de caracteres recueillant le nom du type du champ %s de type alias de la structure Variable (field_types).\n", field_names[i]);
                                    fclose(variable_c);
                                    fclose(variable_c_copy);
                                    exit(EXIT_FAILURE);
                                }
                                field_types[i] = realloc_sp;
                                strcat(field_types[i], split_line);
                            }
                            count_types_words++;
                        }
                        else break;
                        split_line = strtok(NULL, " ;*");
                    }
                }

                if(dim_l == 0){
                    switch(is_struct){
                        case 0 : {
                            char convert[1000];
                            snprintf(convert, sizeof(convert), "  printf(\"\\t\\t %s = ", field_names[i]);
                            if((strcmp(field_types[i],"int") == 0)||(strcmp(field_types[i],"bool") == 0)||(strcmp(field_types[i],"signedint") == 0)||(strcmp(field_types[i],"signed") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"float") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"char") == 0)||(strcmp(field_types[i],"signedchar") == 0)||(strcmp(field_types[i],"unsignedchar") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedint") == 0)||(strcmp(field_types[i],"unsigned") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%u\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"double") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%lf\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"longdouble") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%Lf\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"short") == 0)||(strcmp(field_types[i],"shortint") == 0)||(strcmp(field_types[i],"signedshort") == 0)||(strcmp(field_types[i],"signedshortint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%hd\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedshort") == 0)||(strcmp(field_types[i],"unsignedshortint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%hu\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"long") == 0)||(strcmp(field_types[i],"longint") == 0)||(strcmp(field_types[i],"signedlong") == 0)||(strcmp(field_types[i],"signedlongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%ld\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedlong") == 0)||(strcmp(field_types[i],"unsignedlongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%lu\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"longlong") == 0)||(strcmp(field_types[i],"longlongint") == 0)||(strcmp(field_types[i],"signedlonglong") == 0)||(strcmp(field_types[i],"signedlonglongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%lld\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedlonglong") == 0)||(strcmp(field_types[i],"unsignedlonglongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%llu\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else{
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            fputs(convert, variable_c_copy);
                            break;
                        }

                        case 1 : {
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                    if(size_struct_tab[i][j][k] == NULL){
                                        printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s du champ %s de la structure Variable.\n", field_struct_names[i][j], field_names[i]);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                }

                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "  printf(\"\\t\\t %s.%s = ", field_names[i], field_struct_names[i][j]);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)||(strcmp(field_struct_types[i][j],"signedint") == 0)||(strcmp(field_struct_types[i][j],"signed") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"float") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"char") == 0)||(strcmp(field_struct_types[i][j],"signedchar") == 0)||(strcmp(field_struct_types[i][j],"unsignedchar") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedint") == 0)||(strcmp(field_struct_types[i][j],"unsigned") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%u\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"double") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lf\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"longdouble") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%Lf\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"short") == 0)||(strcmp(field_struct_types[i][j],"shortint") == 0)||(strcmp(field_struct_types[i][j],"signedshort") == 0)||(strcmp(field_struct_types[i][j],"signedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hd\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedshort") == 0)||(strcmp(field_struct_types[i][j],"unsignedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hu\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"long") == 0)||(strcmp(field_struct_types[i][j],"longint") == 0)||(strcmp(field_struct_types[i][j],"signedlong") == 0)||(strcmp(field_struct_types[i][j],"signedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%ld\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lu\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"longlong") == 0)||(strcmp(field_struct_types[i][j],"longlongint") == 0)||(strcmp(field_struct_types[i][j],"signedlonglong") == 0)||(strcmp(field_struct_types[i][j],"signedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lld\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlonglong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%llu\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);
                                }

                                else{
                                    char* indent[1000] = {NULL};
                                    indent[0] = "  ";
                                    char dim[1000] = "";
                                    char dim_print[1000] = "";
                                    char level_print[1000] = "";
                                    char level = 'i';
                                    char convert_name[1000];
                                    int dim_count = 0;
                                    snprintf(convert_name, sizeof(convert_name), "  printf(\"\\t\\t %s.%s :\\n\");\n", field_names[i], field_struct_names[i][j]);
                                    fputs(convert_name, variable_c_copy);

                                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                        char convert[1000];
                                        char temp[1000];
                                        char temp_l[1000];
                                        if(((strcmp(field_struct_types[i][j], "char") == 0)||(strcmp(field_struct_types[i][j], "signedchar") == 0)||(strcmp(field_struct_types[i][j], "unsignedchar") == 0))&&((dim_elements_typedef_variables[0][num_t][j]-k) == 1)) break;
                                        else if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent[k], level, level, size_struct_tab[i][j][k], level);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < variable->%s.%s; %c++){\n", indent[k], level, level, field_names[i], size_struct_tab[i][j][k], level);
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level);
                                        strcat(dim, temp);
                                        strcat(dim_print, "[%d]");
                                        snprintf(temp_l, sizeof(temp_l), ", %c", level);
                                        indent[k+1] = strdup(indent[k]);
                                        strcat(indent[k+1], "  ");
                                        level++;
                                        dim_count++;
                                    }

                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "%sprintf(\"\\t\\t\\t %s.%s%s = ", indent[dim_count], field_names[i], field_struct_names[i][j], dim_print);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)||(strcmp(field_struct_types[i][j],"signedint") == 0)||(strcmp(field_struct_types[i][j],"signed") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"float") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"char") == 0)||(strcmp(field_struct_types[i][j],"signedchar") == 0)||(strcmp(field_struct_types[i][j],"unsignedchar") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%s\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedint") == 0)||(strcmp(field_struct_types[i][j],"unsigned") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%u\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"double") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lf\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"longdouble") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%Lf\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"short") == 0)||(strcmp(field_struct_types[i][j],"shortint") == 0)||(strcmp(field_struct_types[i][j],"signedshort") == 0)||(strcmp(field_struct_types[i][j],"signedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hd\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedshort") == 0)||(strcmp(field_struct_types[i][j],"unsignedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hu\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"long") == 0)||(strcmp(field_struct_types[i][j],"longint") == 0)||(strcmp(field_struct_types[i][j],"signedlong") == 0)||(strcmp(field_struct_types[i][j],"signedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%ld\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lu\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"longlong") == 0)||(strcmp(field_struct_types[i][j],"longlongint") == 0)||(strcmp(field_struct_types[i][j],"signedlonglong") == 0)||(strcmp(field_struct_types[i][j],"signedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lld\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlonglong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%llu\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s.%s%s%s);\n",field_names[i], field_struct_names[i][j], dim, level_print);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);

                                    for(int k = (dim_count-1); k >= 0 ; k--){
                                        char convert[1000];
                                        snprintf(convert, sizeof(convert), "%s}\n", indent[k]);
                                        fputs(convert, variable_c_copy);
                                    }
                                }
                            }
                            break;
                        }

                        default : break;
                    }
                } //Génération du code de comparaison s'il ne s'agit pas d'un tableau

                else{
                    char* indent[1000] = {NULL};
                    indent[0] = "  ";
                    char dim[1000] = "";
                    char dim_print[1000] = "";
                    char level_print[1000] = "";
                    char level = 'i';
                    int dim_count = 0;
                    char convert_name[1000];
                    snprintf(convert_name, sizeof(convert_name), "  printf(\"\\t\\t %s :\\n\");\n", field_names[i]);
                    fputs(convert_name, variable_c_copy);

                    for(int j = 0; j < dim_l; j++){
                        char convert[1000];
                        char temp[1000];
                        char temp_l[1000];
                        if(((strcmp(field_types[i], "char") == 0)||(strcmp(field_types[i], "signedchar") == 0)||(strcmp(field_types[i], "unsignedchar") == 0))&&((dim_l-j) == 1)) break;
                        if(strcmp(size_tab_type[i][j], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent[j], level, level, size_tab[i][j], level);
                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < variable->%s; %c++){\n", indent[j], level, level, size_tab[i][j], level);
                        fputs(convert, variable_c_copy);
                        snprintf(temp, sizeof(temp), "[%c]", level);
                        strcat(dim, temp);
                        strcat(dim_print, "[%d]");
                        snprintf(temp_l, sizeof(temp_l), ", %c", level);
                        strcat(level_print, temp_l);
                        indent[j+1] = strdup(indent[j]);
                        strcat(indent[j+1], "  ");
                        level++;
                        dim_count++;
                    } //Génération des ouvertures des boucles itératives


                    switch(is_struct){
                        case 0 : {
                            char convert[1000];     
                            snprintf(convert, sizeof(convert), "%sprintf(\"\\t\\t\\t %s%s = ", indent[dim_count], field_names[i], dim_print);
                            if((strcmp(field_types[i],"int") == 0)||(strcmp(field_types[i],"bool") == 0)||(strcmp(field_types[i],"signedint") == 0)||(strcmp(field_types[i],"signed") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%d\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"float") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%f\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"char") == 0)||(strcmp(field_types[i],"signedchar") == 0)||(strcmp(field_types[i],"unsignedchar") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%s\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedint") == 0)||(strcmp(field_types[i],"unsigned") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%u\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"double") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%lf\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"longdouble") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%Lf\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"short") == 0)||(strcmp(field_types[i],"shortint") == 0)||(strcmp(field_types[i],"signedshort") == 0)||(strcmp(field_types[i],"signedshortint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%hd\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedshort") == 0)||(strcmp(field_types[i],"unsignedshortint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%hu\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"long") == 0)||(strcmp(field_types[i],"longint") == 0)||(strcmp(field_types[i],"signedlong") == 0)||(strcmp(field_types[i],"signedlongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%ld\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedlong") == 0)||(strcmp(field_types[i],"unsignedlongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%lu\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"longlong") == 0)||(strcmp(field_types[i],"longlongint") == 0)||(strcmp(field_types[i],"signedlonglong") == 0)||(strcmp(field_types[i],"signedlonglongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%lld\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"unsignedlonglong") == 0)||(strcmp(field_types[i],"unsignedlonglongint") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%llu\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else{
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%p\\n\"%s, variable->%s%s);\n", level_print, field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            fputs(convert, variable_c_copy);
                            break;
                        }

                        case 1 : {
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                    if(size_struct_tab[i][j][k] == NULL){
                                        printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles du champ %s du champ %s de la structure Variable.\n", field_struct_names[i][j], field_names[i]);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                }

                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "%sprintf(\"\\t\\t\\t %s%s.%s = ", indent[dim_count], field_names[i], dim_print, field_struct_names[i][j]);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)||(strcmp(field_struct_types[i][j],"signedint") == 0)||(strcmp(field_struct_types[i][j],"signed") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"float") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"char") == 0)||(strcmp(field_struct_types[i][j],"signedchar") == 0)||(strcmp(field_struct_types[i][j],"unsignedchar") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%c\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedint") == 0)||(strcmp(field_struct_types[i][j],"unsigned") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%u\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"double") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lf\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"longdouble") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%Lf\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"short") == 0)||(strcmp(field_struct_types[i][j],"shortint") == 0)||(strcmp(field_struct_types[i][j],"signedshort") == 0)||(strcmp(field_struct_types[i][j],"signedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hd\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedshort") == 0)||(strcmp(field_struct_types[i][j],"unsignedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hu\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"long") == 0)||(strcmp(field_struct_types[i][j],"longint") == 0)||(strcmp(field_struct_types[i][j],"signedlong") == 0)||(strcmp(field_struct_types[i][j],"signedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%ld\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lu\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"longlong") == 0)||(strcmp(field_struct_types[i][j],"longlongint") == 0)||(strcmp(field_struct_types[i][j],"signedlonglong") == 0)||(strcmp(field_struct_types[i][j],"signedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lld\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlonglong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%llu\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\"%s, variable->%s%s.%s);\n", level_print, field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);
                                }

                                else{
                                    char* indent_s[1000] = {NULL};
                                    char dim_s[1000] = "";
                                    char dim_s_print[1000] = "";
                                    char level_s_print[1000] = "";
                                    char level_s = level;
                                    int dim_l_s = dim_l;
                                    int dim_count_s = 0;

                                    for(int k = 0; k <= dim_l; k++){
                                        indent_s[k] = strdup(indent[k]);
                                    }
                                    char convert_name[1000];
                                    snprintf(convert_name, sizeof(convert_name), "%sprintf(\"\\t\\t\\t %s%s.%s :\\n\"%s);\n", indent_s[dim_l_s], field_names[i], dim_print, field_struct_names[i][j], level_print);
                                    fputs(convert_name, variable_c_copy);

                                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                        char convert[1000];
                                        char temp[1000];
                                        char temp_l[1000];
                                        if(((strcmp(field_struct_types[i][j], "char") == 0)||(strcmp(field_struct_types[i][j], "signedchar") == 0)||(strcmp(field_struct_types[i][j], "unsignedchar") == 0))&&((dim_elements_typedef_variables[0][num_t][j]-k) == 1)) break;
                                        if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent_s[dim_l_s], level_s, level_s, size_struct_tab[i][j][k], level_s);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < variable->%s%s.%s; %c++){\n", indent_s[dim_l_s], level_s, level_s, field_names[i], dim, size_struct_tab[i][j][k], level_s);
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level_s);
                                        strcat(dim_s, temp);
                                        strcat(dim_s_print, "[%d]");
                                        snprintf(temp_l, sizeof(temp_l), ", %c", level);
                                        strcat(level_s_print, temp_l);
                                        indent_s[dim_l_s+1] = strdup(indent_s[dim_l_s]);
                                        strcat(indent_s[dim_l_s+1], "  ");
                                        level_s++;
                                        dim_l_s++;
                                        dim_count_s++;
                                    }


                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "%sprintf(\"\\t\\t\\t\\t %s%s.%s%s = ", indent_s[dim_count_s+dim_count], field_names[i], dim_print, field_struct_names[i][j], dim_s_print);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)||(strcmp(field_struct_types[i][j],"signedint") == 0)||(strcmp(field_struct_types[i][j],"signed") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print,field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"float") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"char") == 0)||(strcmp(field_struct_types[i][j],"signedchar") == 0)||(strcmp(field_struct_types[i][j],"unsignedchar") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%s\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedint") == 0)||(strcmp(field_struct_types[i][j],"unsigned") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%u\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"double") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lf\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"longdouble") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%Lf\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"short") == 0)||(strcmp(field_struct_types[i][j],"shortint") == 0)||(strcmp(field_struct_types[i][j],"signedshort") == 0)||(strcmp(field_struct_types[i][j],"signedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hd\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedshort") == 0)||(strcmp(field_struct_types[i][j],"unsignedshortint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%hu\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"long") == 0)||(strcmp(field_struct_types[i][j],"longint") == 0)||(strcmp(field_struct_types[i][j],"signedlong") == 0)||(strcmp(field_struct_types[i][j],"signedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%ld\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lu\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"longlong") == 0)||(strcmp(field_struct_types[i][j],"longlongint") == 0)||(strcmp(field_struct_types[i][j],"signedlonglong") == 0)||(strcmp(field_struct_types[i][j],"signedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%lld\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"unsignedlonglong") == 0)||(strcmp(field_struct_types[i][j],"unsignedlonglongint") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%ll\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\"%s%s, variable->%s%s.%s%s);\n", level_print, level_s_print, field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);

                                    for(int k = (dim_count_s-1); k >= 0 ; k--){
                                        char convert[1000];
                                        dim_l_s--;
                                        snprintf(convert, sizeof(convert), "%s}\n", indent_s[dim_l_s]);
                                        fputs(convert, variable_c_copy);
                                    }
                                }
                            }
                        }
                    }

                    for(int j = (dim_count-1); j >= 0 ; j--){
                        char convert[1000];
                        snprintf(convert, sizeof(convert), "%s}\n", indent[j]);
                        fputs(convert, variable_c_copy);
                    } //Génération de la fermeture des boucles itératives
                } //Génération du code s'il s'agit d'un tableau
            }
            find_line[1] = 1;
        }

        else fputs(chaine_tampon, variable_c_copy); //Ajout de la chaine de caractères dans le fichier généré
    }
    
    int error = 0;
    for(int i = 0; i < 2; i++){
        if(find_line[i] != 1){
            printf("Erreur de syntaxe : La ligne %s n'a pas ete trouvee dans le fichier variable.c.\n", error_message[i]); //Détection de l'absence des lignes à modifier
            error++;
        }
    }
    if(error != 0){
        fclose(variable_c);
        fclose(variable_c_copy);
        exit(EXIT_FAILURE);
    } //Si l'absence d'une ligne est détectée, l'exécution est stoppée


    free(copyPath);
    free(modelesPath);
    free(find_line);
    free(error_message);
    fclose(variable_c);
    fclose(variable_c_copy);
}

void generation_model_c(char* copy_path, char* modeles_path, int nb_locations, int nb_actions, int nb_clocks, char** locations, DBM* invariants, char** actions, int* nb_transitions_locations, Transition*** transitions, int nb_clines_init_variables, line* init_variables_function, int* nb_clines_updatef, line** update_functions, int* nb_clines_constraints, line** constraints_functions){
    char* copyPath = malloc(strlen(copy_path) + strlen("/model.c") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/model.c");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/model.c") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/model.c");

    FILE* model_c = fopen(modelesPath, "r");
    if(!model_c){
        perror("Impossible d'ouvrir le fichier model.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier model.c

    FILE* model_c_copy = fopen(copyPath, "w");
    if(!model_c_copy){
        printf("Le fichier model.c n'a pas pu etre cree.\n");
        fclose(model_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int* find_line = calloc(12, sizeof(int));
    char** error_message = malloc(12 * sizeof(char*));
    error_message[0] = "'    int nb_locations = model_c_nblocations;'";
    error_message[1] = "'    int nb_actions = model_c_nbactions;'";
    error_message[2] = "'    model_c_locations'";
    error_message[3] = "'    model_c_invariants'";
    error_message[4] = "'    model_c_actions'";
    error_message[5] = "'    model_c_transitions'";
    error_message[6] = "'    model_c_init_variables'";
    error_message[7] = "'model_c_update_functions'";
    error_message[8] = "'    model_c_init_update_functions'";
    error_message[9] = "'model_c_constraints'";
    error_message[10] = "'    model_c_init_constraints'";

    char* chaine_tampon = malloc(1000 * sizeof(char));
    while(1){
        char* result = fgets(chaine_tampon, 1000, model_c);
        if(result == NULL){
            if(feof(model_c)) break;
            else{
                printf("Erreur de lecture du fichier model.c.\n");
                fclose(model_c);
                fclose(model_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier model.c
        }
        else if(strchr(result, '\n') == NULL){
            printf("Erreur de taille : La ligne (%s) du fichier model.c depasse 1000 caracteres.\n", chaine_tampon);
            fclose(model_c);
            fclose(model_c_copy);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne trop grande pour le tampon
  
        if(strcmp(chaine_tampon, "    int nb_locations = model_c_nblocations;\n") == 0){
            char convert[1000];
            snprintf(convert, sizeof(convert), "    int nb_locations = %d;\n", nb_locations); //Conversion et concaténation de la chaine à coller dans le fichier généré
            fputs(convert, model_c_copy);
            find_line[0] = 1;
        }

        else if(strcmp(chaine_tampon, "    int nb_actions = model_c_nbactions;\n") == 0){
            char convert[1000];
            snprintf(convert, sizeof(convert), "    int nb_actions = %d;\n", nb_actions); //Conversion et concaténation de la chaine à coller dans le fichier généré
            fputs(convert, model_c_copy);
            find_line[1] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_locations\n") == 0){
            for(int i = 0; i < nb_locations; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    locations[%d] = \"%s\";\n", i, locations[i]);
                fputs(convert, model_c_copy);
            }
            find_line[2] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_invariants\n") == 0){
            for(int i = 0; i < nb_locations; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    static DBM i_%d = {", i);
                for(int j = 0; j < (nb_clocks+1); j++){
                    strcat(convert, "{");
                    for(int k = 0; k < (nb_clocks+1); k++){
                        char temp[9];
                        if(k == nb_clocks){
                            if(j == nb_clocks){
                                if(invariants[i][j][k] == infty) snprintf(temp, sizeof(temp), "infty}};\n");
                                else snprintf(temp, sizeof(temp), "%d}};\n", invariants[i][j][k]);
                            }
                            else{
                                 if(invariants[i][j][k] == infty) snprintf(temp, sizeof(temp), "infty},");
                                 else snprintf(temp, sizeof(temp), "%d},", invariants[i][j][k]);
                            } 
                        }
                        else{
                            if(invariants[i][j][k] == infty) snprintf(temp, sizeof(temp), "infty,");
                            else snprintf(temp, sizeof(temp), "%d,", invariants[i][j][k]);
                        } 
                        strcat(convert, temp);
                    }
                }
                fputs(convert, model_c_copy);
            } //Affectation des DBM statics

            for(int i = 0; i < nb_locations; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    invariants[%d] = &i_%d;\n", i, i);
                fputs(convert, model_c_copy);
            } //Affectation du tableau d'invariants

            find_line[3] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_actions\n") == 0){
            for(int i = 0; i < nb_actions; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    actions[%d] = \"%s\";\n", i, actions[i]);
                fputs(convert, model_c_copy);
            }
            find_line[4] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_transitions\n") == 0){
            for(int i = 0; i < nb_locations; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    nb_trans_par_location[%d] = %d;\n", i, nb_transitions_locations[i]);
                fputs(convert, model_c_copy);
            } //Affectation des nombres de transitions par localité

            for(int i = 0; i < nb_locations; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    transitions[%d] = malloc(nb_trans_par_location[%d] * sizeof(Transition));\n", i, i);
                fputs(convert, model_c_copy);
            } //Allocation mémoire pour les tableaux de transitions de chaque localité

            for(int i = 0; i < nb_locations; i++){
                for(int j = 0; j < nb_transitions_locations[i]; j++){
                    char convert[1000];
                    snprintf(convert, sizeof(convert), "    transitions[%d][%d] = (Transition){.location_in = %d, .label_action = %d, .guard = {", i, j, transitions[i][j]->location_in, transitions[i][j]->label_action); //Affectation des valeurs de localité entrante et d'action

                    for(int k = 0; k < (nb_clocks+1); k++){
                        strcat(convert, "{");
                        for(int l = 0; l < (nb_clocks+1); l++){
                            char temp[9];
                            if(l == nb_clocks){
                                if(k == nb_clocks){
                                    if(transitions[i][j]->guard[k][l] == infty) snprintf(temp, sizeof(temp), "infty}}, ");
                                    else snprintf(temp, sizeof(temp), "%d}}, ", transitions[i][j]->guard[k][l]);
                                }
                                else{
                                    if(transitions[i][j]->guard[k][l] == infty) snprintf(temp, sizeof(temp), "infty},");
                                    else snprintf(temp, sizeof(temp), "%d},", transitions[i][j]->guard[k][l]);
                                } 
                            }
                            else{
                                if(transitions[i][j]->guard[k][l] == infty) snprintf(temp, sizeof(temp), "infty,");
                                else snprintf(temp, sizeof(temp), "%d,", transitions[i][j]->guard[k][l]);
                            } 
                            strcat(convert, temp);
                        }
                    } //Affectation de la valeur d'invariant de transition

                    strcat(convert, ".reset = {");
                    for(int k = 0; k < nb_clocks; k++){
                        char temp[7];
                        if(k == (nb_clocks-1)){
                            if(transitions[i][j]->reset[k] == infty) snprintf(temp, sizeof(temp), "infty}");
                            else snprintf(temp, sizeof(temp), "%d}", transitions[i][j]->reset[k]);
                        }
                        else{
                            if(transitions[i][j]->reset[k] == infty) snprintf(temp, sizeof(temp), "infty,");
                            else snprintf(temp, sizeof(temp), "%d,", transitions[i][j]->reset[k]);
                        }
                        strcat(convert, temp);
                    } //Affectation de la valeur de l'ensemble reset
                    strcat(convert, "};\n");
                    fputs(convert, model_c_copy);
                }
                
            } //Affectation des transitions pour chaque localité

            find_line[5] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_init_variables\n") == 0){
            for(int i = 0; i < nb_clines_init_variables; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "%s\n", init_variables_function[i]);
                fputs(convert, model_c_copy);
            }
            find_line[6] = 1;
        }

        else if(strcmp(chaine_tampon, "model_c_update_functions\n") == 0){
            for(int i = 0; i < nb_actions; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "Variable update_%s(Variable variable) {\n", actions[i]);
                fputs(convert, model_c_copy);
                for(int j = 0; j < nb_clines_updatef[i]; j++){
                    char convertBis[1000];
                    snprintf(convertBis, sizeof(convertBis), "%s\n", update_functions[i][j]);
                    fputs(convertBis, model_c_copy);
                }
                fputs("}\n", model_c_copy);
                fputs("\n", model_c_copy);
            }
            find_line[7] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_init_update_functions\n") == 0){
            for(int i = 0; i < nb_actions; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    update_functions[%d] = update_%s;\n", i, actions[i]);
                fputs(convert, model_c_copy);
            }
            find_line[8] = 1;
        }

        else if(strcmp(chaine_tampon, "model_c_constraints\n") == 0){
            for(int i = 0; i < nb_actions; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "bool const_%s(Variable variable) {\n", actions[i]);
                fputs(convert, model_c_copy);
                for(int j = 0; j < nb_clines_constraints[i]; j++){
                    char convertBis[1000];
                    snprintf(convertBis, sizeof(convertBis), "%s\n", constraints_functions[i][j]);
                    fputs(convertBis, model_c_copy);
                }
                fputs("}\n", model_c_copy);
                fputs("\n", model_c_copy);
            }
            find_line[9] = 1;
        }

        else if(strcmp(chaine_tampon, "    model_c_init_constraints\n") == 0){
            for(int i = 0; i < nb_actions; i++){
                char convert[1000];
                snprintf(convert, sizeof(convert), "    constraints[%d] = const_%s;\n", i, actions[i]);
                fputs(convert, model_c_copy);
            }
            find_line[10] = 1;
        }

        else fputs(chaine_tampon, model_c_copy); //Copie si la ligne n'est pas une ligne à modifier
    }

    int error = 0;
    for(int i = 0; i < 11; i++){
        if(find_line[i] != 1){
            printf("Erreur de syntaxe : La ligne %s n'a pas ete trouvee dans le fichier model.c.\n", error_message[i]); //Détection de l'absence des lignes à modifier
            error++;
        }
    }
    if(error != 0){
        fclose(model_c);
        fclose(model_c_copy);
        exit(EXIT_FAILURE);
    } //Si l'absence d'une ligne est détectée, l'exécution est stoppée

    free(copyPath);
    free(modelesPath);
    free(find_line);
    free(error_message);
    fclose(model_c);
    fclose(model_c_copy);
}

void generation_ta_extended_builder_c(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/ta_extended_builder.c") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/ta_extended_builder.c");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/ta_extended_builder.c") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/ta_extended_builder.c");

    FILE* ta_extended_builder_c = fopen(modelesPath, "r");
    if(!ta_extended_builder_c){
        perror("Impossible d'ouvrir le fichier ta_extended_builder.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier ta_extended_builder.c

    FILE* ta_extended_builder_c_copy = fopen(copyPath, "w");
    if(!ta_extended_builder_c_copy){
        printf("Le fichier ta_extended_builder.c n'a pas pu etre cree.\n");
        fclose(ta_extended_builder_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int tampon;
    while(1){
        tampon = fgetc(ta_extended_builder_c);
        if(tampon == EOF){
            if(feof(ta_extended_builder_c)) break; //Détection de la fin du fichier ta_extended_builder.c
            else{
                printf("Erreur de lecture du fichier ta_extended_builder.c.\n");
                fclose(ta_extended_builder_c);
                fclose(ta_extended_builder_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier ta_extended_builder.c
        }
        fputc(tampon, ta_extended_builder_c_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(ta_extended_builder_c);
    fclose(ta_extended_builder_c_copy);
}

void generation_main_c(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/main.c") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/main.c");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/main.c") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/main.c");

    FILE* main_c = fopen(modelesPath, "r");
    if(!main_c){
        perror("Impossible d'ouvrir le fichier main.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier main.c

    FILE* main_c_copy = fopen(copyPath, "w");
    if(!main_c_copy){
        printf("Le fichier main.c n'a pas pu etre cree.\n");
        fclose(main_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int tampon;
    while(1){
        tampon = fgetc(main_c);
        if(tampon == EOF){
            if(feof(main_c)) break; //Détection de la fin du fichier main.c
            else{
                printf("Erreur de lecture du fichier main.c.\n");
                fclose(main_c);
                fclose(main_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier main.c
        }
        fputc(tampon, main_c_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(main_c);
    fclose(main_c_copy);
}

void generation_gitignore(char* copy_path, char* modeles_path){
    char* copyPath = malloc(strlen(copy_path) + strlen("/.gitignore") + 1);
    strcpy(copyPath, copy_path);
    strcat(copyPath, "/.gitignore");
    char* modelesPath = malloc(strlen(modeles_path) + strlen("/.gitignore") + 1);
    strcpy(modelesPath, modeles_path);
    strcat(modelesPath, "/.gitignore");

    FILE* gitignore = fopen(modelesPath, "r");
    if(!gitignore){
        perror("Impossible d'ouvrir le fichier .gitignore.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier .gitignore

    FILE* gitignore_copy = fopen(copyPath, "w");
    if(!gitignore_copy){
        printf("Le fichier .gitignore n'a pas pu etre cree.\n");
        fclose(gitignore);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    int tampon;
    while(1){
        tampon = fgetc(gitignore);
        if(tampon == EOF){
            if(feof(gitignore)) break; //Détection de la fin du fichier .gitignore
            else{
                printf("Erreur de lecture du fichier .gitignore.\n");
                fclose(gitignore);
                fclose(gitignore_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier .gitignore
        }
        fputc(tampon, gitignore_copy); //Ajout du caractère dans le fichier généré
    }

    free(copyPath);
    free(modelesPath);
    fclose(gitignore);
    fclose(gitignore_copy);
}

void generation(char* copy_path, char* modeles_path, ParseInfos* parseInfos){
    generation_uthash_h(copy_path, modeles_path); //Fonction de génération du fichier uthash.h
    generation_structure_DBM_h(copy_path, modeles_path, parseInfos->nb_clocks); //Fonction de génération du fichier structure_DBM.h
    generation_structure_state_space_ta_h(copy_path, modeles_path); //Fonction de génération du fichier structure_state_space_ta.h
    generation_ta_extended_builder_c(copy_path, modeles_path); //Fonction de génération du fichier ta_extended_builder.c
    generation_main_c(copy_path, modeles_path); //Fonction de génération du fichier main.c
    generation_gitignore(copy_path, modeles_path); //Fonction de génération du fichier .gitignore
    generation_structure_ta_h(copy_path, modeles_path); //Fonction de génération du fichier structure_ta.h
    generation_DBM_c(copy_path, modeles_path); //Fonction de génération du fichier DBM.c
    generation_structure_variable_h(copy_path, modeles_path, parseInfos->nb_define, parseInfos->def_variables_define, parseInfos->nb_clines_typedef, parseInfos->nb_typedef_structure, parseInfos->nb_typedef_alias, parseInfos->label_typedef, parseInfos->def_variables_typedef); //Fonction de génération du fichier structure_variable.h
    generation_variable_c(copy_path, modeles_path, parseInfos->nb_clines_typedef, parseInfos->nb_typedef_structure, parseInfos->nb_typedef_alias, parseInfos->label_typedef, parseInfos->def_variables_typedef, parseInfos->dim_elements_typedef_variables); //Fonction de génération du fichier variable.c
    generation_model_c(copy_path, modeles_path, parseInfos->nb_locations, parseInfos->nb_actions, parseInfos->nb_clocks, parseInfos->locations, parseInfos->invariants, parseInfos->actions, parseInfos->nb_transitions_locations, parseInfos->transitions, parseInfos->nb_clines_init_variables, parseInfos->init_variables_function, parseInfos->nb_clines_updatef, parseInfos->update_functions, parseInfos->nb_clines_constraints, parseInfos->constraints_functions); //Fonction de génération du fichier model.c
}