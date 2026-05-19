#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generation.h"

void generation_uthash_h(){
    FILE* uthash_h = fopen("modeles_generation/uthash.h", "r");
    if(!uthash_h){
        perror("Impossible d'ouvrir le fichier uthash.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier uthash.h

    FILE* uthash_h_copy = fopen("../uthash.h", "w");
    if(!uthash_h_copy){
        printf("Le fichier uthash.h n'a pas pu etre cree.\n");
        fclose(uthash_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char tampon;
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

    fclose(uthash_h);
    fclose(uthash_h_copy);
}

void generation_structure_DBM_h(int nb_clocks){ 
    FILE* structure_DBM_h = fopen("modeles_generation/structure_DBM.h", "r");
    if(!structure_DBM_h){
        perror("Impossible d'ouvrir le fichier structure_DBM.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_DBM.h

    FILE* structure_DBM_h_copy = fopen("../structure_DBM.h", "w");
    if(!structure_DBM_h_copy){
        printf("Le fichier structure_DBM.h n'a pas pu etre cree.\n");
        fclose(structure_DBM_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int find_line = 0;
    char* chaine_tampon = malloc(1000 * sizeof(char));
    while(1){
        chaine_tampon = fgets(chaine_tampon, 1000, structure_DBM_h);
        if(chaine_tampon == NULL){
            if(feof(structure_DBM_h)) break;
            else{
                printf("Erreur de lecture du fichier structure_DBM.h.\n");
                fclose(structure_DBM_h);
                fclose(structure_DBM_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_DBM.h
        }
        else if(strchr(chaine_tampon, '\n') == NULL){
            printf("Erreur de taille : La ligne (%s) du fichier structure_DBM.h depasse 1000 caracteres.\n", chaine_tampon);
            fclose(structure_DBM_h);
            fclose(structure_DBM_h_copy);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne trop grande pour le tampon

        
        if(strcmp(chaine_tampon, "#define NB_CLOCKS structure_DBM_h_nbclocks\n") == 0){
            char convert[1000];
            snprintf(convert, sizeof(convert), "#define NB_CLOCKS %d\n", nb_clocks); //Conversion et concaténation de la chaine à coller dans le fichier généré
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

    fclose(structure_DBM_h);
    fclose(structure_DBM_h_copy);
}

void generation_structure_variable_h(int nb_define, line* def_variables_define, int** nb_clines_typedef, int nb_typedef_struct, int nb_typedef_primitive, char*** label_typedef, line*** def_variables_typedef){
    FILE* structure_variable_h = fopen("modeles_generation/structure_variable.h", "r");
    if(!structure_variable_h){
        perror("Impossible d'ouvrir le fichier structure_variable.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_variable.h

    FILE* structure_variable_h_copy = fopen("../structure_variable.h", "w");
    if(!structure_variable_h_copy){
        printf("Le fichier structure_variable.h n'a pas pu etre cree.\n");
        fclose(structure_variable_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int* find_line = calloc(3, sizeof(int));
    char** error_message = malloc(3 * sizeof(char*));
    error_message[0] = "'structure_variable_h_define'";
    error_message[1] = "'structure_variable_h_typedef_primitive'";
    error_message[2] = "'structure_variable_h_typedef_struct'";

    char* chaine_tampon = malloc(1000 * sizeof(char));
    while(1){
        chaine_tampon = fgets(chaine_tampon, 1000, structure_variable_h);
        if(chaine_tampon == NULL){
            if(feof(structure_variable_h)) break;
            else{
                printf("Erreur de lecture du fichier structure_variable.h.\n");
                fclose(structure_variable_h);
                fclose(structure_variable_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_variable.h
        }
        else if(strchr(chaine_tampon, '\n') == NULL){
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
            }
            fputs("\n", structure_variable_h_copy);
            find_line[0] = 1;
        }

        else if(strcmp(chaine_tampon, "structure_variable_h_typedef_primitive\n") == 0){
            for(int i = 0; i < nb_typedef_primitive; i++){
                for(int j = 0; j < nb_clines_typedef[1][i]; j++){
                    char convert[1000];
                    snprintf(convert, sizeof(convert), "%s\n", def_variables_typedef[1][i][j]);
                    fputs(convert, structure_variable_h_copy);
                }
            }
            if(nb_typedef_primitive != 0) fputs("\n", structure_variable_h_copy);
            find_line[1] = 1;
        }

        else if(strcmp(chaine_tampon, "structure_variable_h_typedef_struct\n") == 0){
            for(int i = 0; i < nb_typedef_struct; i++){
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
            }
            find_line[2] = 1;
        }

        else fputs(chaine_tampon, structure_variable_h_copy); //Copie si la ligne n'est pas une ligne à modifier
    }

    for(int i = 0; i < 3; i++){
        if(find_line[i] != 1){
            printf("Erreur de syntaxe : La ligne %s n'a pas ete trouvee dans le fichier structure_variable.h.\n", error_message[i]);
            fclose(structure_variable_h);
            fclose(structure_variable_h_copy);
            exit(EXIT_FAILURE);
        } //Détection de l'absence de la ligne à modifier
    }

    free(find_line);
    free(error_message);
    fclose(structure_variable_h);
    fclose(structure_variable_h_copy);
}

void generation_structure_ta_h(){
    FILE* structure_ta_h = fopen("modeles_generation/structure_ta.h", "r");
    if(!structure_ta_h){
        perror("Impossible d'ouvrir le fichier structure_ta.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier main.c

    FILE* structure_ta_h_copy = fopen("../structure_ta.h", "w");
    if(!structure_ta_h_copy){
        printf("Le fichier structure_ta.h n'a pas pu etre cree.\n");
        fclose(structure_ta_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    char tampon;
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

    fclose(structure_ta_h);
    fclose(structure_ta_h_copy);
}

void generation_structure_state_space_ta_h(){
    FILE* structure_state_space_ta_h = fopen("modeles_generation/structure_state_space_ta.h", "r");
    if(!structure_state_space_ta_h){
        perror("Impossible d'ouvrir le fichier structure_state_space_ta.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_state_space_ta.h

    FILE* structure_state_space_ta_h_copy = fopen("../structure_state_space_ta.h", "w");
    if(!structure_state_space_ta_h_copy){
        printf("Le fichier structure_state_space_ta.h n'a pas pu etre cree.\n");
        fclose(structure_state_space_ta_h);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    char tampon;
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

    fclose(structure_state_space_ta_h);
    fclose(structure_state_space_ta_h_copy);
}

void generation_DBM_c(){
    FILE* DBM_c = fopen("modeles_generation/DBM.c", "r");
    if(!DBM_c){
        perror("Impossible d'ouvrir le fichier DBM.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier DBM.c

    FILE* DBM_c_copy = fopen("../DBM.c", "w");
    if(!DBM_c_copy){
        printf("Le fichier DBM.c n'a pas pu etre cree.\n");
        fclose(DBM_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    char tampon;
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

    fclose(DBM_c);
    fclose(DBM_c_copy);
}

void generation_variable_c(int** nb_clines_typedef, int nb_typedef_struct, int nb_typedef_primitive, char*** label_typedef, line*** def_variables_typedef, int*** dim_elements_typedef_variables){
    printf("v0\t");
    FILE* variable_c = fopen("modeles_generation/variable.c", "r");
    if(!variable_c){
        perror("Impossible d'ouvrir le fichier variable.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier variable.c

    FILE* variable_c_copy = fopen("../variable.c", "w");
    if(!variable_c_copy){
        printf("Le fichier variable.c n'a pas pu etre cree.\n");
        fclose(variable_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    printf("v1\t");
    
    int* find_line = calloc(2, sizeof(int));
    char** error_message = malloc(2 * sizeof(char*));
    error_message[0] = "'  variable_c_equal_function'";
    error_message[1] = "'  variable_c_print_function'";
    printf("v2\t");

    char* chaine_tampon = malloc(1000 * sizeof(char));
    printf("v3\t");
    
    line*** copy = malloc(2* sizeof(line**)); //Copie du tableau des lignes de code des typedef types pour éviter une perte d'informations
    copy[0] = malloc(nb_typedef_struct * sizeof(line*));
    copy[1] = malloc(nb_typedef_primitive * sizeof(line*));
    for(int i = 0; i < nb_typedef_struct; i++){
        copy[0][i] = malloc(nb_clines_typedef[0][i] * sizeof(line));
        for(int j = 0; j < nb_clines_typedef[0][i]; j++){
            copy[0][i][j] = strdup(def_variables_typedef[0][i][j]);
        }
    }
    for(int i = 0; i < nb_typedef_primitive; i++){
        copy[1][i] = malloc(nb_clines_typedef[1][i] * sizeof(line));
        for(int j = 0; j < nb_clines_typedef[1][i]; j++){
            copy[1][i][j] = strdup(def_variables_typedef[1][i][j]);
        }
    } //Copie des lignes de code de Variable
    printf("v4\t");

    char seps[] = " [;]*"; //Chaine de caractères contenant les caractères de sépération pour le découpage
    char* field_names[1000] = {NULL}; //Récupération des noms des champs de l'objet Variable
    char* field_types[1000] = {NULL}; //Récupération du nom du type de chaque champs de l'objet Variable
    char** size_tab[1000] = {NULL}; //Récupération des tailles des tableaux 
    char** size_tab_type[1000] = {NULL}; //Tableau qui permet de savoir si la taille de la dimension est définie dynamiquement ou statiquement
    char* field_types_def[1000] = {NULL};
    printf("v5\t");

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_struct-1]; i++){
        printf("v6\t");
        char *split_string = strtok(copy[0][nb_typedef_struct-1][i], seps); //Découpe la chaine selon les caractères de seps
        int count = 0; //Variable pour savoir quel type de texte est attendu lors du parcours de la chaine découpée
        int size_count = 0; //Variable pour compter le nombre de chaine à ajouter dans size_tab et size_tab_type
        char* temp[1000];
        printf("v7\t");
        while(split_string != NULL){
            printf("v8\t");
            if(count == 0) {
                printf("v9\t");
                field_types_def[i] = "n";
                field_types[i] = strdup(split_string); //Récupération du type du champ de Variable
                for(int j = 0; j < nb_typedef_primitive; j++){
                    if(strcmp(field_types[i], label_typedef[1][j]) == 0){
                        char temp_type[1000];
                        snprintf(temp_type, sizeof(temp_type), "p%d", j);
                        field_types_def[i] = strdup(temp_type);
                    }
                }
                for(int j = 0; j < nb_typedef_struct; j++){
                    if(strcmp(field_types[i], label_typedef[0][j]) == 0){
                        char temp_type[1000];
                        snprintf(temp_type, sizeof(temp_type), "s%d", j);
                        field_types_def[i] = strdup(temp_type);
                    }
                }
                printf("v10\t");
            }
            else if(count == 1) field_names[i] = strdup(split_string); //Récupération du nom du champ de Variable
            else if(count > 1){
                if(strcmp(split_string, "//") == 0) break; //Sortie de la boucle si la suite découpée correspond à un commentaire
                else{
                    temp[size_count] = strdup(split_string);
                    size_count++;
                } //Récupération du nom de la variable sauvegardant la taille d'une dimension du champ de Variable
            }
            split_string = strtok(NULL, seps);
            count++;
            printf("v11\t");
        }

        int dim_size;
        dim_size = dim_elements_typedef_variables[0][nb_typedef_struct-1][i];
        if(field_types_def[i][0] == 'p'){
            char* conv;
            int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
            dim_size += dim_elements_typedef_variables[1][num_t][i];
        }
        size_tab[i] = malloc((dim_size+1) * sizeof(char*));
        size_tab_type[i] = malloc((dim_size+1) * sizeof(char*));

        if(size_count == 0){
            for(int j = 0; j < dim_size; j++){
                size_tab[i][j] = NULL;
                size_tab_type[i][j] = NULL;
            }
            
            printf("v12\t");
        } //Initialisation de size_tab et size_tab_type si size_count est égal à 0

        else {
            if(size_count > dim_size){
                printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles de %s.\n", field_names[i]);
                fclose(variable_c);
                fclose(variable_c_copy);
                exit(EXIT_FAILURE);
            }
            int count_dim = 0;
            for(int j = 0; j < dim_size; j++){
                if(count_dim < size_count){
                    size_tab[i][j] = strdup(temp[j]);
                    size_tab_type[i][j] = "define";
                }
                else{
                    size_tab[i][j] = NULL;
                    size_tab_type[i][j] = NULL;
                }
                count_dim++;
            }
        } //Remplissage de size_tab et size_tab_type si des dimensions ont été récupérées pour des champs de Variable  
    }
    printf("v13\t");

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_struct-1]; i++){
        printf("v14\t");
        char* copy_name = strdup(field_names[i]);
        char *split_name = strtok(copy_name, "_"); //Découpage de chaque nom de champ de Variable pour récupérer des variables savegardant des dimensions de tableaux dynamiques de Variable
        char* temp[1000];
        int count = 0;
        while(split_name != NULL){
            temp[count] = strdup(split_name);
            split_name = strtok(NULL, "_");
            count++;
        }
        printf("v15\t");

        if(count == 1) continue; //Passe à l'itération de la boucle suivante si le nom ne contient pas le caractère '_'

        if(strcmp(temp[1], "size") == 0){ //Détection du mot size indiquant que la variable considérée permet de sauvegarder une dimension d'un des tableaux dynamiques de Variable
            switch(count){
                case 2 : { 
                    printf("v16\t");
                    for(int j = 0; j < nb_clines_typedef[0][nb_typedef_struct-1]; j++){
                        if(strcmp(field_names[j], temp[0]) == 0){
                            if(size_tab[j][0] != NULL){
                                printf("Erreur de syntaxe : La taille de %s est definie deux fois par la constante symbolique %s et %s", field_names[j], size_tab[j][0], field_names[i]);
                                free(copy_name);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            } //Détection d'un conflit de variable sauvegardant la dimension du tableau 

                            size_tab[j][0] = field_names[i];
                            size_tab_type[j][0] = "*";
                            break;
                        }
                    }
                    printf("v17\t");
                    break;
                } //Détection d'une variable sauvegardant la première dimension

                case 3 : {
                    printf("v18\t");
                    char* convert;
                    int taille = (int)strtol(temp[2], &convert, 10);
                    if(*convert != '\0') break;
                    for(int j = 0; j < nb_clines_typedef[0][nb_typedef_struct-1]; j++){
                        if(strcmp(field_names[j], temp[0]) == 0){
                            int dim_size = dim_elements_typedef_variables[0][nb_typedef_struct-1][i];
                            if(field_types_def[j][0] == 'p'){
                                char* conv;
                                int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
                                dim_size += dim_elements_typedef_variables[1][num_t][0];
                            }
                            if(taille > dim_size){
                                printf("%d %d\n", taille, dim_size);
                                printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles de %s.\n", field_names[j]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                            if(size_tab[j][taille] != NULL){
                                printf("Erreur de syntaxe : La taille de la dimension %d de %s est definie deux fois par la constante symbolique %s et %s", taille, field_names[j], size_tab[j][taille], field_names[i]);
                                free(copy_name);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            } //Détection d'un conflit de variable sauvegardant la taille de la dimension du tableau

                            size_tab[j][taille] = field_names[i];
                            size_tab_type[j][taille] = "*";
                            break;
                        }
                    }
                    printf("v19\t");
                    break;
                } //Détection d'une variable sauvegardant une autre dimension du tableau

                default : break;
            }
        }
        free(copy_name);
    }
    printf("v20\t");

    char** field_struct_names[1000] = {NULL};
    char** field_struct_types[1000] = {NULL};
    char*** size_struct_tab[1000] = {NULL};
    char*** size_struct_tab_type[1000] = {NULL};
    printf("v21\t");

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_struct-1]; i++){
        printf("v22\t");
        if(field_types_def[i][0] == 's'){
            printf("v23\t");
            char* conv;
            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

            field_struct_names[i] = malloc(nb_clines_typedef[0][num_t] * sizeof(char*));
            field_struct_types[i] = malloc(nb_clines_typedef[0][num_t] * sizeof(char*));
            size_struct_tab[i] = malloc(nb_clines_typedef[0][num_t] * sizeof(char*));
            size_struct_tab_type[i] = malloc(nb_clines_typedef[0][num_t] * sizeof(char*));
            printf("v24\t");

            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                printf("v25\t");
                char *split_struct = strtok(copy[0][num_t][j], seps);
                int count = 0; 
                int size_count = 0;
                char* temp[1000];
                printf("v26\t");
                while(split_struct != NULL){
                    printf("v27\t");
                    if(count == 0) field_struct_types[i][j] = strdup(split_struct);
                    else if(count == 1) field_struct_names[i][j] = strdup(split_struct);
                    else if (count > 1){
                        if(strcmp(split_struct, "//") == 0) break;
                        else{
                            temp[size_count] = strdup(split_struct);
                            size_count++;
                        }
                    }
                    split_struct = strtok(NULL, seps);
                    count++;
                    printf("v28\t");
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
                        printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles de %s.\n", field_names[i]);
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
    printf("v29\t");

    for(int i = 0; i < nb_clines_typedef[0][nb_typedef_struct-1]; i++){
        printf("v30\t");
        if(field_types_def[i][0] == 's'){
            char* conv;
            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);
            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                printf("v31\t");
                char* copy_name = strdup(field_struct_names[i][j]);
                char *split_name = strtok(copy_name, "_");
                char* temp[1000];
                int count = 0;
                while(split_name != NULL){
                    temp[count] = strdup(split_name);
                    split_name = strtok(NULL, "_");
                    count++;
                }
                printf("v32\t");

                if(count == 1) continue;

                if(strcmp(temp[1], "size") == 0){
                    switch(count){
                        case 2 : {
                            printf("v33\t");
                            for(int k = 0; k < nb_clines_typedef[0][num_t]; k++){
                                if(strcmp(field_struct_names[i][k], temp[0]) == 0){
                                    if(size_struct_tab[i][k][0] != NULL){
                                        printf("Erreur de syntaxe : La taille de %s est definie deux fois par la constante symbolique %s et %s", field_struct_names[i][k], size_struct_tab[i][k][0], field_struct_names[i][j]);
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
                            printf("v34\t");
                            break;
                        }

                        case 3 : {
                            printf("v35\t");
                            char* convert;
                            int taille = (int)strtol(temp[2], &convert, 10);
                            if(*convert != '\0') break;
                            for(int k = 0; k < nb_clines_typedef[0][nb_typedef_struct-1]; k++){
                                if(strcmp(field_names[j], temp[0]) == 0){
                                    if(taille > dim_elements_typedef_variables[0][num_t][j]){
                                        printf("Erreur de syntaxe : Trop de variables ont ete definie pour les tailles de %s.\n", field_struct_names[i][j]);
                                        fclose(variable_c);
                                        fclose(variable_c_copy);
                                        exit(EXIT_FAILURE);
                                    }
                                    if(size_struct_tab[i][k][taille] != NULL){
                                        printf("Erreur de syntaxe : La taille de la dimension %d de %s est definie deux fois par la constante symbolique %s et %s", taille, field_struct_names[i][k], size_struct_tab[i][k][taille], field_struct_names[i][j]);
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
                            printf("v36\t");
                            break;
                        }

                        default : break;
                    }
                }
                free(copy_name);
            }
        }
    }
    printf("v37\t");

    for(int i = 0; i < nb_typedef_struct; i++){
        free(copy[0][i]);
    }
    for(int i = 0; i < nb_typedef_primitive; i++){
        free(copy[1][i]);
    }
    free(copy[0]);
    free(copy[1]);
    free(copy);
    printf("v38\t");

    while(1){
        printf("v39\t");
        chaine_tampon = fgets(chaine_tampon, 1000, variable_c);
        if(chaine_tampon == NULL){
            if(feof(variable_c)) break;
            else{
                printf("Erreur de lecture du fichier variable.c.\n");
                fclose(variable_c);
                fclose(variable_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier variable.c
        }
        else if(strchr(chaine_tampon, '\n') == NULL){
            printf("Erreur de taille : La ligne (%s) du fichier variable.c depasse 1000 caracteres.\n", chaine_tampon);
            fclose(variable_c);
            fclose(variable_c_copy);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne trop grande pour le tampon
        printf("v40\t");
 
        if(strcmp(chaine_tampon, "  variable_c_equal_function\n") == 0){
            printf("v41\t");
            for(int i = 0; i < nb_clines_typedef[0][nb_typedef_struct-1]; i++){
                printf("v42\t");
                int is_struct;
                int dim_l = dim_elements_typedef_variables[0][nb_typedef_struct-1][i];
                if(strcmp(field_types_def[i], "n") == 0){
                    printf("v43\t");
                    is_struct = 0;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles de %s.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    printf("v44\t");
                }
                else if(field_types_def[i][0] == 's'){
                    printf("v45\t");
                    is_struct = 1;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles de %s.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    printf("v46\t");
                }
                else{
                    printf("v47\t");
                    is_struct = 0;
                    char* conv;
                    int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
                    dim_l += dim_elements_typedef_variables[1][num_t][0];
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles de %s.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    printf("v48\t");
                }

                if(dim_l == 0){
                    printf("v49\t");
                    switch(is_struct){
                        case 0 : {
                            printf("v50\t");
                            char convert[1000];
                            snprintf(convert, sizeof(convert), "  if(v1->%s != v2->%s){\n", field_names[i], field_names[i]);
                            fputs(convert, variable_c_copy);
                            fputs("    return false;\n", variable_c_copy);
                            fputs("  }\n", variable_c_copy);
                            printf("v51\t");
                            break;
                        }

                        case 1 : {
                            printf("v52\t");
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    printf("v53\t");
                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "  if(v1->%s.%s != v2->%s.%s){\n", field_names[i], field_struct_names[i][j], field_names[i], field_struct_names[i][j]);
                                    fputs(convert, variable_c_copy);
                                    fputs("    return false;\n", variable_c_copy);
                                    fputs("  }\n", variable_c_copy);
                                    printf("v54\t");
                                }

                                else{
                                    printf("v55\t");
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
                                    }
                                    printf("v56\t");

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
                                    }
                                    printf("v57\t");

                                    for(int k = (dim_elements_typedef_variables[0][num_t][j]-1); k >= 0 ; k--){
                                        char convert[1000];
                                        snprintf(convert, sizeof(convert), "%s}\n", indent[k]);
                                        fputs(convert, variable_c_copy);
                                    }
                                    printf("v58\t");
                                }
                            }
                            break;
                        }

                        default : break;
                    }
                    
                }

                else{
                    printf("v59\t");
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
                    printf("v60\t");

                    switch(is_struct){
                        case 0 : {
                            for(int j = 0; j < 3; j++){
                                printf("v60.1\t");
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
                                printf("v60.2\t");
                            } //Génération du contenu des boucles
                            break;
                        }

                        case 1 : {
                            printf("v60.3\t");
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    printf("v60.4\t");
                                    for(int k = 0; k < 3; k++){
                                        char convert[1000];
                                        printf("v60.5\t");
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
                                        printf("v60.6\t");
                                    }
                                    printf("v60.7\t");
                                }

                                else{
                                    printf("v60.8\t");
                                    char* indent_s[1000] = {NULL};
                                    char dim_s[1000] = "";
                                    char level_s = level;
                                    int dim_l_s = dim_l;

                                    for(int k = 0; k <= dim_l; k++){
                                        indent_s[k] = strdup(indent[k]);
                                    }
                                    printf("v60.9\t");

                                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                        printf("v60.10\t");
                                        char convert[1000];
                                        char temp[1000];
                                        if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c; %c < %s; %c++){\n", indent_s[dim_l_s], level_s, level_s, size_struct_tab[i][j][k], level_s);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c; %c < v1->%s%s.%s; %c++){\n", indent_s[dim_l_s], level_s, level_s, field_names[i], dim, size_struct_tab[i][j][k], level_s);
                                        printf("bug0\t");
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level_s);
                                        printf("bug1\t");
                                        strcat(dim_s, temp);
                                        indent_s[dim_l_s+1] = strdup(indent_s[dim_l_s]);
                                        strcat(indent_s[dim_l_s+1], "  ");
                                        level_s++;
                                        dim_l_s++;
                                        printf("v60.11\t");
                                    }

                                    for(int k = 0; k < 3; k++){
                                        printf("v60.12\t");
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
                                        printf("v60.13\t");
                                    }

                                    for(int k = (dim_elements_typedef_variables[0][num_t][j]-1); k <= 0; k++){
                                        printf("bugbug0\t");
                                        char convert[1000];
                                        dim_l_s--;
                                        snprintf(convert, sizeof(convert), "%s}\n", indent_s[dim_l_s]);
                                        printf("bugbug1\t");
                                        fputs(convert, variable_c_copy);
                                    }
                                    printf("v60.14\t");
                                }
                            }
                            break;
                        }
                    }
                    printf("v61\t");

                    for(int j = (dim_l-1); j >= 0 ; j--){
                        char convert[1000];
                        snprintf(convert, sizeof(convert), "%s}\n", indent[j]);
                        fputs(convert, variable_c_copy);
                    } //Génération de la fermeture des boucles itératives

                } //Génération du code s'il s'agit d'un tableau
            }
            fputs("  return true;\n", variable_c_copy);
            find_line[0] = 1;
            printf("v62\t");
        }

        else if(strcmp(chaine_tampon, "  variable_c_print_function\n") == 0){
            printf("v63\t");
            for(int i = 0; i < nb_clines_typedef[0][nb_typedef_struct-1]; i++){
                printf("v64\t");
                int dim_l = dim_elements_typedef_variables[0][nb_typedef_struct-1][i];
                int is_struct;
                if(strcmp(field_types_def[i], "n") == 0){
                    printf("v65\t");
                    is_struct = 0;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles de %s.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    printf("v66\t");
                }
                else if(field_types_def[i][0] == 's'){ 
                    printf("v67\t");
                    is_struct = 1;
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles de %s.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    printf("v67\t");
                }
                else{
                    printf("v68\t");
                    is_struct = 0;
                    char* conv;
                    int num_t = (int)strtol(field_types_def[i] + 1, &conv, 10);
                    dim_l += dim_elements_typedef_variables[1][num_t][0];
                    if(dim_l > 0){
                        for(int j = 0; j < dim_l; j++){
                            if(size_tab[i][j] == NULL){
                                printf("Erreur de syntaxe : Aucune variable n'est detecte pour definir une des tailles de %s.\n", field_names[i]);
                                fclose(variable_c);
                                fclose(variable_c_copy);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }

                    char* copy_line = strdup(def_variables_typedef[1][num_t][0]);
                    char *split_line = strtok(copy_line, " ;*");
                    int count_line = 0;
                    while(split_line != NULL){
                        if(count_line == 1) field_types[i] = strdup(split_line);
                        split_line = strtok(NULL, " ;*");
                        count_line++;
                    }
                    printf("v69\t");
                }

                if(dim_l == 0){
                    printf("v70\t");
                    switch(is_struct){
                        case 0 : {
                            printf("v71\t");
                            char convert[1000];
                            snprintf(convert, sizeof(convert), "  printf(\"\\t %s = ", field_names[i]);
                            if((strcmp(field_types[i],"int") == 0)||(strcmp(field_types[i],"bool") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"float") == 0)||(strcmp(field_types[i], "double") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"char") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            else{
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s);\n", field_names[i]);
                                strcat(convert, convertBis);
                            }
                            fputs(convert, variable_c_copy);
                            printf("v72\t");
                            break;
                        }

                        case 1 : {
                            printf("v73\t");
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    printf("v74\t");
                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "  printf(\"\\t %s.%s = ", field_names[i], field_struct_names[i][j]);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"float") == 0)||(strcmp(field_struct_types[i][j], "double") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"char") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s.%s);\n",field_names[i], field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);
                                    printf("v75\t");
                                }

                                else{
                                    printf("v76\t");
                                    char* indent[1000] = {NULL};
                                    indent[0] = "  ";
                                    char dim[1000] = "";
                                    char level = 'i';

                                    for(int k = 0; k < dim_elements_typedef_variables[0][num_t][j]; k++){
                                        char convert[1000];
                                        char temp[1000];
                                        if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent[k], level, level, size_struct_tab[i][j][k], level);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < variable->%s.%s; %c++){\n", indent[k], level, level, field_names[i], size_struct_tab[i][j][k], level);
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level);
                                        strcat(dim, temp);
                                        indent[k+1] = strdup(indent[k]);
                                        strcat(indent[k+1], "  ");
                                        level++;
                                    }
                                    printf("v77\t");

                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "%sprintf(\"\\t %s.%s%s = ", indent[dim_elements_typedef_variables[0][num_t][j]], field_names[i], field_struct_names[i][j], dim);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s.%s%s);\n",field_names[i], field_struct_names[i][j], dim);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"float") == 0)||(strcmp(field_struct_types[i][j], "double") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s.%s%s);\n",field_names[i], field_struct_names[i][j], dim);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"char") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s.%s%s);\n",field_names[i], field_struct_names[i][j], dim);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s.%s%s);\n",field_names[i], field_struct_names[i][j], dim);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);
                                    printf("v78\t");

                                    for(int k = (dim_elements_typedef_variables[0][num_t][j]-1); k >= 0 ; k--){
                                        char convert[1000];
                                        snprintf(convert, sizeof(convert), "%s}\n", indent[k]);
                                        fputs(convert, variable_c_copy);
                                    }
                                    printf("v79\t");
                                }
                            }
                            break;
                        }

                        default : break;
                    }
                } //Génération du code de comparaison s'il ne s'agit pas d'un tableau

                else{
                    printf("v80\t");
                    char* indent[1000] = {NULL};
                    indent[0] = "  ";
                    char dim[1000] = "";
                    char level = 'i';

                    for(int j = 0; j < dim_l; j++){
                        char convert[1000];
                        char temp[1000];
                        if(strcmp(size_tab_type[i][j], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < %s; %c++){\n", indent[j], level, level, size_tab[i][j], level);
                        else snprintf(convert, sizeof(convert), "%sfor(int %c = 0; %c < variable->%s; %c++){\n", indent[j], level, level, size_tab[i][j], level);
                        fputs(convert, variable_c_copy);
                        snprintf(temp, sizeof(temp), "[%c]", level);
                        strcat(dim, temp);
                        indent[j+1] = strdup(indent[j]);
                        strcat(indent[j+1], "  ");
                        level++;
                    } //Génération des ouvertures des boucles itératives
                    printf("v81\t");

                    switch(is_struct){
                        case 0 : {
                            char convert[1000];     
                            snprintf(convert, sizeof(convert), "%sprintf(\"\\t %s%s = ", indent[dim_l], field_names[i], dim);
                            if((strcmp(field_types[i],"int") == 0)||(strcmp(field_types[i],"bool") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s%s);\n", field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if((strcmp(field_types[i],"float") == 0)||(strcmp(field_types[i], "double") == 0)){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s%s);\n", field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else if(strcmp(field_types[i],"char") == 0){
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s%s);\n", field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            else{
                                char convertBis[1000];
                                snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s%s);\n", field_names[i], dim);
                                strcat(convert, convertBis);
                            }
                            fputs(convert, variable_c_copy);
                            break;
                        }

                        case 1 : {
                            char* conv;
                            int num_t = (int)strtol(field_types_def[i]+1, &conv, 10);

                            for(int j = 0; j < nb_clines_typedef[0][num_t]; j++){
                                if(dim_elements_typedef_variables[0][num_t][j] == 0){
                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "  printf(\"\\t %s%s.%s = ", field_names[i], dim, field_struct_names[i][j]);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s%s.%s);\n",field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"float") == 0)||(strcmp(field_struct_types[i][j], "double") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s%s.%s);\n",field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"char") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s%s.%s);\n",field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s%s.%s);\n",field_names[i], dim, field_struct_names[i][j]);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);
                                }

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
                                        if(strcmp(size_struct_tab_type[i][j][k], "define") == 0) snprintf(convert, sizeof(convert), "%sfor(int %c; %c < %s; %c++){\n", indent_s[dim_l_s], level_s, level_s, size_struct_tab[i][j][k], level_s);
                                        else snprintf(convert, sizeof(convert), "%sfor(int %c; %c < variable->%s%s.%s; %c++){\n", indent_s[dim_l_s], level_s, level_s, field_names[i], dim, size_struct_tab[i][j][k], level_s);
                                        fputs(convert, variable_c_copy);
                                        snprintf(temp, sizeof(temp), "[%c]", level_s);
                                        strcat(dim_s, temp);
                                        indent_s[dim_l_s+1] = strdup(indent_s[dim_l_s]);
                                        strcat(indent_s[dim_l_s+1], "  ");
                                        level_s++;
                                        dim_l_s++;
                                    }

                                    char convert[1000];
                                    snprintf(convert, sizeof(convert), "%sprintf(\"\\t %s%s.%s%s = ", indent_s[dim_l_s], field_names[i], dim, field_struct_names[i][j], dim_s);
                                    if((strcmp(field_struct_types[i][j],"int") == 0)||(strcmp(field_struct_types[i][j],"bool") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%d\\n\", variable->%s%s.%s%s);\n",field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if((strcmp(field_struct_types[i][j],"float") == 0)||(strcmp(field_struct_types[i][j], "double") == 0)){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%f\\n\", variable->%s%s.%s%s);\n",field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else if(strcmp(field_struct_types[i][j],"char") == 0){
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%c\\n\", variable->%s%s.%s%s);\n",field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    else{
                                        char convertBis[1000];
                                        snprintf(convertBis, sizeof(convertBis), "%%p\\n\", variable->%s%s.%s%s);\n",field_names[i], dim, field_struct_names[i][j], dim_s);
                                        strcat(convert, convertBis);
                                    }
                                    fputs(convert, variable_c_copy);

                                    for(int k = (dim_elements_typedef_variables[0][num_t][j]-1); k >= 0 ; k--){
                                        char convert[1000];
                                        dim_l_s--;
                                        snprintf(convert, sizeof(convert), "%s}\n", indent_s[dim_l_s]);
                                        fputs(convert, variable_c_copy);
                                    }
                                }
                            }
                        }
                    }
                    
                    printf("v82\t");

                    for(int j = (dim_l-1); j >= 0 ; j--){
                        char convert[1000];
                        snprintf(convert, sizeof(convert), "%s}\n", indent[j]);
                        fputs(convert, variable_c_copy);
                    } //Génération de la fermeture des boucles itératives
                    printf("v83\t");

                } //Génération du code s'il s'agit d'un tableau
            }
            find_line[1] = 1;
        }

        else fputs(chaine_tampon, variable_c_copy); //Ajout de la chaine de caractère dans le fichier généré
        printf("v84\t");
    }

    free(find_line);
    free(error_message);
    fclose(variable_c);
    fclose(variable_c_copy);
    printf("vFin\t");
} //A poursuivre

void generation_model_c(int nb_locations, int nb_actions, int nb_clocks, char** locations, DBM* invariants, char** actions, int* nb_transitions_locations, Transition*** transitions, int nb_clines_init_variables, line* init_variables_function, int* nb_clines_updatef, line** update_functions, int* nb_clines_constraints, line** constraints_functions){
    FILE* model_c = fopen("modeles_generation/model.c", "r");
    if(!model_c){
        perror("Impossible d'ouvrir le fichier model.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier model.c

    FILE* model_c_copy = fopen("../model.c", "w");
    if(!model_c_copy){
        printf("Le fichier model.c n'a pas pu etre cree.\n");
        fclose(model_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int* find_line = calloc(11, sizeof(int));
    char** error_message = malloc(11 * sizeof(char*));
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
        chaine_tampon = fgets(chaine_tampon, 1000, model_c);
        if(chaine_tampon == NULL){
            if(feof(model_c)) break;
            else{
                printf("Erreur de lecture du fichier model.c.\n");
                fclose(model_c);
                fclose(model_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier model.c
        }
        else if(strchr(chaine_tampon, '\n') == NULL){
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

    for(int i = 0; i < 11; i++){
        if(find_line[i] != 1){
            printf("Erreur de syntaxe : La ligne %s n'a pas ete trouvee dans le fichier model.c.\n", error_message[i]);
            fclose(model_c);
            fclose(model_c_copy);
            exit(EXIT_FAILURE);
        } //Détection de l'absence de la ligne à modifier
    }

    free(find_line);
    free(error_message);
    fclose(model_c);
    fclose(model_c_copy);
}

void generation_ta_extended_builder_c(){
    FILE* ta_extended_builder_c = fopen("modeles_generation/ta_extended_builder.c", "r");
    if(!ta_extended_builder_c){
        perror("Impossible d'ouvrir le fichier ta_extended_builder.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier ta_extended_builder.c

    FILE* ta_extended_builder_c_copy = fopen("../ta_extended_builder.c", "w");
    if(!ta_extended_builder_c_copy){
        printf("Le fichier ta_extended_builder.c n'a pas pu etre cree.\n");
        fclose(ta_extended_builder_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    char tampon;
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

    fclose(ta_extended_builder_c);
    fclose(ta_extended_builder_c_copy);
}

void generation_main_c(){
    FILE* main_c = fopen("modeles_generation/main.c", "r");
    if(!main_c){
        perror("Impossible d'ouvrir le fichier main.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier main.c

    FILE* main_c_copy = fopen("../main.c", "w");
    if(!main_c_copy){
        printf("Le fichier main.c n'a pas pu etre cree.\n");
        fclose(main_c);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    char tampon;
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

    fclose(main_c);
    fclose(main_c_copy);
}

void generation_gitignore(){
    FILE* gitignore = fopen(".gitignore", "r");
    if(!gitignore){
        perror("Impossible d'ouvrir le fichier .gitignore.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier .gitignore

    FILE* gitignore_copy = fopen("../.gitignore", "w");
    if(!gitignore_copy){
        printf("Le fichier .gitignore n'a pas pu etre cree.\n");
        fclose(gitignore);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier
    
    char tampon;
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

    fclose(gitignore);
    fclose(gitignore_copy);
}

void generation(ParseInfos* parseInfos){
    printf("0\n");
    generation_uthash_h(); //Fonction de génération du fichier uthash.h
    printf("1\n");
    generation_structure_DBM_h(parseInfos->nb_clocks); //Fonction de génération du fichier structure_DBM.h
    printf("2\n");
    generation_structure_state_space_ta_h(); //Fonction de génération du fichier structure_state_space_ta.h
    printf("3\n");
    generation_ta_extended_builder_c(); //Fonction de génération du fichier ta_extended_builder.c
    printf("4\n");
    generation_main_c(); //Fonction de génération du fichier main.c
    printf("5\n");
    generation_gitignore(); //Fonction de génération du fichier .gitignore
    printf("6\n");
    generation_structure_ta_h(); //Fonction de génération du fichier structure_ta.h
    printf("7\n");
    generation_DBM_c(); //Fonction de génération du fichier DBM.c
    printf("8\n");
    generation_structure_variable_h(parseInfos->nb_define, parseInfos->def_variables_define, parseInfos->nb_clines_typedef, parseInfos->nb_typedef_struct, parseInfos->nb_typedef_primitive, parseInfos->label_typedef, parseInfos->def_variables_typedef); //Fonction de génération du fichier structure_variable.h
    printf("9\n");
    generation_variable_c(parseInfos->nb_clines_typedef, parseInfos->nb_typedef_struct, parseInfos->nb_typedef_primitive, parseInfos->label_typedef, parseInfos->def_variables_typedef, parseInfos->dim_elements_typedef_variables); //Fonction de génération du fichier variable.c
    printf("10\n");
    generation_model_c(parseInfos->nb_locations, parseInfos->nb_actions, parseInfos->nb_clocks, parseInfos->locations, parseInfos->invariants, parseInfos->actions, parseInfos->nb_transitions_locations, parseInfos->transitions, parseInfos->nb_clines_init_variables, parseInfos->init_variables_function, parseInfos->nb_clines_updatef, parseInfos->update_functions, parseInfos->nb_clines_constraints, parseInfos->constraints_functions); //Fonction de génération du fichier model.c
    printf("11\n");
}