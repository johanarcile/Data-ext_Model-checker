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
            printf("Erreur de taille : La ligne (%s) depasse 1000 caracteres.\n", chaine_tampon);
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

void generation_structure_variable_h(){
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

    char tampon;
    while(1){
        tampon = fgetc(structure_variable_h);
        if(tampon == EOF){
            if(feof(structure_variable_h)) break; //Détection de la fin du fichier structure_variable.h
            else{
                printf("Erreur de lecture du fichier structure_variable.h.\n");
                fclose(structure_variable_h);
                fclose(structure_variable_h_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier structure_variable.h
        }
        fputc(tampon, structure_variable_h_copy); //Ajout du caractère dans le fichier généré
    }

    fclose(structure_variable_h);
    fclose(structure_variable_h_copy);
} //A poursuivre

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

void generation_variable_c(){
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
    
    char tampon;
    while(1){
        tampon = fgetc(variable_c);
        if(tampon == EOF){
            if(feof(variable_c)) break; //Détection de la fin du fichier variable.c
            else{
                printf("Erreur de lecture du fichier variable.c.\n");
                fclose(variable_c);
                fclose(variable_c_copy);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de lecture du fichier variable.c
        }
        fputc(tampon, variable_c_copy); //Ajout du caractère dans le fichier généré
    }

    fclose(variable_c);
    fclose(variable_c_copy);
} //A poursuivre

void generation_model_c(int nb_locations, int nb_actions, int nb_clocks, char** locations, DBM* invariants, char** actions, int* nb_transitions_locations, Transition*** transitions){
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

    int* find_line = malloc(6 * sizeof(int));
    char** error_message = malloc(6 * sizeof(char*));
    error_message[0] = "'    int nb_locations = model_c_nblocations;'";
    error_message[1] = "'    int nb_actions = model_c_nbactions;'";
    error_message[2] = "'    model_c_locations'";
    error_message[3] = "'    model_c_invariants'";
    error_message[4] = "'    model_c_actions'";
    error_message[5] = "'    model_c_transitions'";

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
            printf("Erreur de taille : La ligne (%s) depasse 1000 caracteres.\n", chaine_tampon);
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

        else fputs(chaine_tampon, model_c_copy); //Copie si la ligne n'est pas une ligne à modifier
    }

    for(int i = 0; i < 3; i++){
        if(find_line[i] != 1){
            printf("Erreur de syntaxe : La ligne %s n'a pas ete trouvee dans le fichier model.c.\n", error_message[i]);
            fclose(model_c);
            fclose(model_c_copy);
            exit(EXIT_FAILURE);
        } //Détection de l'absence de la ligne à modifier
    }

    fclose(model_c);
    fclose(model_c_copy);
} //A poursuivre

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
    generation_uthash_h(); //Fonction de génération du fichier uthash.h
    generation_structure_DBM_h(parseInfos->nb_clocks); //Fonction de génération du fichier structure_DBM.h
    generation_structure_state_space_ta_h(); //Fonction de génération du fichier structure_state_space_ta.h
    generation_ta_extended_builder_c(); //Fonction de génération du fichier ta_extended_builder.c
    generation_main_c(); //Fonction de génération du fichier main.c
    generation_gitignore(); //Fonction de génération du fichier .gitignore
    generation_structure_ta_h(); //Fonction de génération du fichier structure_ta.h
    generation_DBM_c(); //Fonction de génération du fichier DBM.c
    generation_structure_variable_h(); //Fonction de génération du fichier structure_variable.h
    generation_variable_c(); //Fonction de génération du fichier variable.c
    generation_model_c(parseInfos->nb_locations, parseInfos->nb_actions, parseInfos->nb_clocks, parseInfos->locations, parseInfos->invariants, parseInfos->actions, parseInfos->nb_transitions_locations, parseInfos->transitions); //Fonction de génération du fichier model.c
}