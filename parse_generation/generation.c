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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere; //Création d'un caractère tampon pour la copie caractère par caractère du fichier
    while((caractere = fgetc(uthash_h)) != EOF){ //Récupération du caractère dans le fichier modèle
        fputc(caractere, uthash_h_copy); //Ajout du caractère dans le fichier généré
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    int count = 0;
    char* chaine = malloc(100 * sizeof(char));
    while((chaine = fgets(chaine, 100, structure_DBM_h)) != NULL){
        if(strcmp(chaine, "#define NB_CLOCKS structure_DBM_h_nbclocks\n") == 0){
            char convert[100];
            snprintf(convert, sizeof(convert), "#define NB_CLOCKS %d\n", nb_clocks); //Conversion et concaténation de la chaine à coller dans le fichier généré
            fputs(convert, structure_DBM_h_copy);
            count++;
        }
        else fputs(chaine, structure_DBM_h_copy); //Copie si la ligne n'est pas celle 
    }

    if(count != 1){
        printf("Erreur de syntaxe : La ligne '#define NB_CLOCKS structure_DBM_nbclocks' n'a pas été trouvée dans le fichier structure_DBM.h.\n");
        fclose(structure_DBM_h);
        fclose(structure_DBM_h_copy);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de la ligne à modifier

    fclose(structure_DBM_h);
    fclose(structure_DBM_h_copy);
}

void generation_structure_variable_h(){
    FILE* structure_variable_h = fopen("modeles_generation/structure_variable_h.h", "r");
    if(!structure_variable_h){
        perror("Impossible d'ouvrir le fichier structure_variable_h.h.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier structure_variable_h.h

    FILE* structure_variable_h_copy = fopen("../structure_variable_h.h", "w");
    if(!structure_variable_h_copy){
        printf("Le fichier structure_variable_h.h n'a pas pu etre cree.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere; //Création d'un caractère tampon pour la copie caractère par caractère du fichier
    while((caractere = fgetc(structure_variable_h)) != EOF){ //Récupération du caractère dans le fichier modèle
        fputc(caractere, structure_variable_h_copy); //Ajout du caractère dans le fichier généré
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(structure_ta_h)) != EOF){
        fputc(caractere, structure_ta_h_copy);
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(structure_state_space_ta_h)) != EOF){
        fputc(caractere, structure_state_space_ta_h_copy);
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(DBM_c)) != EOF){
        fputc(caractere, DBM_c_copy);
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(variable_c)) != EOF){
        fputc(caractere, variable_c_copy);
    }

    fclose(variable_c);
    fclose(variable_c_copy);
} //A poursuivre

void generation_model_c(){
    FILE* model_c = fopen("modeles_generation/model.c", "r");
    if(!model_c){
        perror("Impossible d'ouvrir le fichier model.c.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier model.c

    FILE* model_c_copy = fopen("../model.c", "w");
    if(!model_c_copy){
        printf("Le fichier model.c n'a pas pu etre cree.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(model_c)) != EOF){
        fputc(caractere, model_c_copy);
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(ta_extended_builder_c)) != EOF){
        fputc(caractere, ta_extended_builder_c_copy);
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
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(main_c)) != EOF){
        fputc(caractere, main_c_copy);
    }

    fclose(main_c);
    fclose(main_c_copy);
}

void generation_gitignore(){
    FILE* gitignore = fopen(".gitignore", "r");
    if(!gitignore){
        perror("Impossible d'ouvrir le fichier .gitignore.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier main.c

    FILE* gitignore_copy = fopen("../.gitignore", "w");
    if(!gitignore_copy){
        printf("Le fichier .gitignore n'a pas pu etre cree.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de création du fichier

    char caractere;
    while((caractere = fgetc(gitignore)) != EOF){
        fputc(caractere, gitignore_copy);
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
    generation_model_c(); //Fonction de génération du fichier model.c
}