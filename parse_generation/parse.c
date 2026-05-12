#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "cJSON.h"
#include "parse.h"

//Définition des champs de ParseInfos
int nb_actions; //Variable pour conserver le nombre d'actions
char** actions; //Tableau des nom des actions
int nb_clocks; //Variable pour conserver le nombre d'horloges
char** names_clocks; //Tableau de sauvegarde des noms d'horloges
int nb_locations; //Variable pour conserver le nombre de localités
char** locations; //Tableau des noms des localités
DBM* invariants; //Structure DBM pour conserver les invariants
int* nb_transitions_locations; //Tableau pour conserver le nombre de transitions sortantes de chaque localité
Transition*** transitions; //Tableau à double dimension des transitions

int nb_define; //Variable pour conserver le nombre de lignes de code pour la définition des constantes symboliques
line* def_variables_define; //Tableau des lignes de code pour la définition de la structure Variable du fichier structure_variable.h
int** nb_clines_typedef;
int nb_typedef_struct;
int nb_typedef_primitive;
char*** label_typedef;
line*** def_variables_typedef; //Taleau des lignes de codes des typdef
int*** dim_elements_typedef_variables; //Sauvegarde des dimensions des champs de la structure définie pour Variable qui servira pour l'écriture des fonctions de comparaison et d'affichage du fichier variable.c
int nb_clines_init_variables;
line* init_variables_function;
int* nb_clines_updatef;
line** update_functions;
int* nb_clines_constraints;
line** constraints_functions;

//Ouverture du fichier json en mode lecture
char* read_model_json(const char* filename){
    FILE* file = fopen(filename, "r");
    if(!file){
        perror("Impossible d'ouvrir le modele json.\n");
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'ouverture du fichier

    fseek(file, 0, SEEK_END); //Place la tête de lecture ou curseur à la fin du fichier pour obtenir sa taille
    long length = ftell(file); //Récupération de la taille du fichier passé en entrée
    fseek(file, 0, SEEK_SET); //Replace la tête de lecture ou curseur au début du fichier

    char* contenu = (char*)malloc(length+1); //Création d'une chaine de caractère vide de même taille que le fichier.
    if(!contenu){
        perror("Erreur d'allocation memoire.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour la chaine

    fread(contenu, 1, length, file); //Copie chaque caractère du fichier json lu dans la chaine contenu
    contenu[length] = '\0'; //Place le marqueur de fin de le chaine
    fclose(file);
    return contenu;
}

void parse_model_json(const char* json_donnees){
    //Parsing du contenu
    cJSON* json = cJSON_Parse(json_donnees); //Récupération de la structure de données json depuis la chaine
    if(!json){
        printf("Erreur d'analyse : Le fichier json passe en entree est invalide.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de syntaxe json

    // Parsing du tableau d'actions
    cJSON* actions_json = cJSON_GetObjectItemCaseSensitive(json, "actions"); //Récupération de la valeur associée à l'item actions
    if(!actions_json){
        printf("Erreur de syntaxe : Aucun objet actions detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet actions dans le json

    if(!cJSON_IsArray(actions_json)){
        printf("Erreur de syntaxe : Le type de l'objet actions est incorrect.\nType attendu : Array.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet actions

    int length_actions = cJSON_GetArraySize(actions_json); //Récupération de la taille du tableau d'actions
    nb_actions = length_actions;
    actions = malloc(length_actions * sizeof(char*)); //Allocation de l'espace mémoire pour le tableau d'actions
    if(!actions){
        printf("Erreur d'allocation memoire pour le tableau d'actions (actions).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour actions

    for(int i = 0; i < length_actions; i++){
        cJSON* action = cJSON_GetArrayItem(actions_json, i); //Récupération de la valeur d'action pour l'indice i
        if(!action){
            printf("Erreur de syntaxe : La valeur d'action d'indice %d est nulle.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une valeur d'action vide

        if(!cJSON_IsString(action)){
            printf("Erreur de syntaxe : Le type de l'action d'indice %d du tableau d'actions est incorrect.\nType attendu : String.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un type primitif pour une action du tableau actions différent d'un string

        actions[i] = strdup(action->valuestring); //Affectation de la valeur récupérée à la case mémoire du tableau d'actions de même indice 
        for(int j = 0; j < i; j++){
            if(strcmp(action->valuestring, actions[j]) == 0){
                printf("Erreur de syntaxe : L'action %s est definie en double dans le tableau d'actions.\n", actions[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection de doublons pour actions
        }
    }

    //Parsing du tableau des noms d'horloges
    cJSON* clocks_json = cJSON_GetObjectItemCaseSensitive(json, "clocks"); //Récupération de la valeur associée à l'item clocks
    if(!clocks_json){
        printf("Erreur de syntaxe : Aucun objet clocks detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet clocks
 
    if(!cJSON_IsArray(clocks_json)){
        printf("Erreur de syntaxe : Le type de l'objet clocks est incorrect.\nType attendu : Array.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet clocks
    
    int length_clocks = cJSON_GetArraySize(clocks_json); //Récupération de la taille du tableau de clocks
    nb_clocks = length_clocks;
    names_clocks = malloc(length_clocks * sizeof(char*)); //Allocation de l'espace mémoire pour le tableau de noms des horloges
    if(!names_clocks){
        printf("Erreur d'allocation memoire pour le tableau des noms d'horloges (names_clocks).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'un erreur d'allocation mémoire pour names_clocks

    for(int i = 0; i < length_clocks; i++){
        cJSON* clock = cJSON_GetArrayItem(clocks_json, i); //Récupération de la valeur de clocks à l'indice i
        if(!clock){
            printf("Erreur de syntaxe : La valeur d'horloge d'indice %d est nulle.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une valeur d'horloge vide

        if(!cJSON_IsString(clock)){
            printf("Erreur de syntaxe : Le type de la valeur d'horloge d'indice %d est incorrect.\nType attendu : String.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un type primitif pour clock différent d'une chaine de caractères

        names_clocks[i] = strdup(clock->valuestring); //Affectation de la valeur récupérée à la case mémoire du tableau de clocks de même indice  
        for(int j = 0; j < i; j++){
            if(strcmp(clock->valuestring, names_clocks[j]) == 0){
                printf("Erreur de syntaxe : L'horloge %s est definie en double dans le tableau d'horloges.\n", names_clocks[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection de doublons pour clocks
        } 
    }

    //Parsing du tableau des localités
    cJSON* locations_json = cJSON_GetObjectItemCaseSensitive(json, "locations"); //Récupération du tableau de localités
    if(!locations_json){
        printf("Erreur de syntaxe : Aucun objet locations detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet locations

    if(!cJSON_IsArray(locations_json)){
        printf("Erreur de syntaxe : Le type de l'objet locations est incorrect.\nType attendu : Array.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet locations

    int length_locations = cJSON_GetArraySize(locations_json); //Récupération de la taille du tableau de localités
    if(length_locations == 0){
        printf("Erreur de syntaxe : Aucune localite definie.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une absence de localité dans la définition 

    nb_locations = length_locations; //Récupération du nombre de localités définies
    locations = malloc(length_locations * sizeof(char*)); //Allocations de l'espace mémoire pour le tableau des localités
    if(!locations){
        printf("Erreur d'allocation memoire pour le tableau des localites (locations).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour locations


    for(int i = 0; i < nb_locations; i++){
        cJSON* location_json = cJSON_GetArrayItem(locations_json, i); //Récupération de la valeur de localité à l'indice i
        if(!location_json){
            printf("Erreur de syntaxe : La valeur d'indice %d du tableau de localite est nulle.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une absence de définition d'un nom de localité

        if(!cJSON_IsString(location_json)){
            printf("Erreur de syntaxe : Le type de la valeur d'indice %d du tableau de localites est incorrect.\nType attendu : String.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour le nom d'une localité

        locations[i] = strdup(location_json->valuestring); //Affectation de la valeur récupérée à la case mémoire du tableau de locations de même indice
        for(int j = 0; j < i; j++){
            if(strcmp(location_json->valuestring, locations[j]) == 0){
                printf("Erreur de syntaxe : La localite %s est definie en double dans le tableau de localites.\n", locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection de doublons pour locations
        } 
    }

    //Parsing de la localité initiale
    cJSON* init_json = cJSON_GetObjectItemCaseSensitive(json, "init"); //Récupéartion de la chaine de caractères du nom de la localité initiale
    if(!init_json){
        printf("Erreur de syntaxe : Aucune localite initiale definie.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet init

    if(!cJSON_IsString(init_json)){
        printf("Erreur de syntaxe : Le type de la valeur de l'objet init est incorrect.\nType attendu : String.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet init

    char* init_location = strdup(init_json->valuestring); //Affectation de la valeur récupérée à une chaine de caractères temporaire
    int count = 0; //Création d'un compteur qui permettra de déterminer si la localité initiale définie a été définie dans le tableau des localités
    for(int i = 0; i < nb_locations; i++){
        if(strcmp(init_location, locations[i]) == 0){
            char* temp = locations[0]; //Placement de la localité initiale au début de locations
            locations[0] = init_location;
            locations[i] = temp;
            break; //Arrêt de la boucle pour ne pas faire des itérations inutiles
        }
        else count++; 
    }
    if(count == nb_locations){
        printf("Erreur de syntaxe : La localite initiale n'est pas definie.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de définition de la localité initiale

    nb_transitions_locations = malloc(nb_locations * sizeof(int)); //Allocation de l'espace mémoire pour le tableau de nombre de transitions par localité
    if(!nb_transitions_locations){
        printf("Erreur d'allocation memoire pour le tableau de nombre de transitions sortantes de chaque localite (nb_transitions_locations).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation memoire pour le tableau des nombres de transitions sortantes de chaque localité

    invariants = malloc(nb_locations * sizeof(DBM)); //Allocation de l'espace mémoire pour le tableau des invariants
    if(!invariants){
        printf("Erreur d'allocation memoire pour le tableau d'invariants (invariants).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour le tableau d'invariants

    transitions = malloc(nb_locations * sizeof(Transition**)); //Allocation de l'espace mémoire pour le tableau des transitions
    if(!transitions){
        printf("Erreur d'allocation memoire pour le tableau de transitions (transitions).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour le tableau de transitions

    //Parsing des informations des localités
    for(int i = 0; i < nb_locations; i++){
        cJSON* location_json = cJSON_GetObjectItemCaseSensitive(json, locations[i]); //Récupération de la structure de données de la localité i 
        if(!location_json){
            printf("Erreur de syntaxe : Aucun objet trouve pour la localite %s.\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une absence de l'objet de localité i 

        if(!cJSON_IsObject(location_json)){
            printf("Erreur de syntaxe : Le type de la valeur de l'objet localite %s est incorrect.\nType attendu : Object.\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour la localité i

        cJSON* invariant_json = cJSON_GetObjectItemCaseSensitive(location_json, "invariant"); //Récupération de la valeur d'invariant pour la localité i
        if(!invariant_json){
            printf("Erreur de syntaxe : L'invariant de la localite %s est vide.\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un invariant vide

        if(!cJSON_IsArray(invariant_json)){
            printf("Erreur de syntaxe : Le type de l'invariant de la localite %s est incorrect.\nType attendu : Array.\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour l'invariant

        if(cJSON_GetArraySize(invariant_json) != (nb_clocks+1)){
            printf("Erreur de syntaxe : La taille de l'invariant est incorrecte.\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une taille d'invariant inattendue

        invariants[i] = malloc((nb_clocks+1) * sizeof(int*)); //Allocation de l'espace mémoire pour l'invariant i du tableau d'invariants
        if(!invariants[i]){
            printf("Erreur d'allocation memoire pour l'invariant de la localite %s (invariants[i]).\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour invariants[i]

        for(int k = 0; k < (nb_clocks+1); k++){
            invariants[i][k] = malloc((nb_clocks+1) * sizeof(int)); //Allocation de l'espace mémoire pour invariants[i][k]
            if(!invariants[i][k]){
                printf("Erreur d'allocation memoire pour l'invariant de la localite %s (invariants[i][j]).\n", locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur d'allocation mémoire pour invariants[i][k]

            //Parsing de l'invariant de la localité i
            cJSON* invariant_col_json = cJSON_GetArrayItem(invariant_json, k); //Récupération des tableaux d'entiers de l'invariant
            if(!invariant_col_json){
                printf("Erreur de syntaxe : La valeur d'indice %d de l'invariant de la localite %s est vide.\n", k, locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection de l'absence de définition de l'invariant

            if(!cJSON_IsArray(invariant_col_json)){
                printf("Erreur de syntaxe : Le type de la valeur d'indice %d de l'invariant de la localite %s est incorrect.\nType attendu : Array.\n", k, locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de type pour la ligne k de la matrice d'invariant de la localité i

            if(cJSON_GetArraySize(invariant_col_json) != (nb_clocks+1)){
                printf("Erreur de syntaxe : La taille de la valeur d'indice %d de l'invariant de la localite %s est incorrecte.\n", k, locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une taille en ligne d'invariant inattendue

            for(int l = 0; l < (nb_clocks+1); l++){
                cJSON* invariant_value_json = cJSON_GetArrayItem(invariant_col_json, l); //Récupération de la valeur à l'indice [k][l] de l'invariant
                if(!invariant_value_json){
                    printf("Erreur de syntaxe : La valeur d'indice [%d][%d] de l'invariant de la localite %s est vide.\n", k, l, locations[i]);
                    cJSON_Delete(json);
                    exit(EXIT_FAILURE);
                } //Détection d'une valeur d'invariant nulle pour la localité i

                if(cJSON_IsNumber(invariant_value_json)){
                    invariants[i][k][l] = invariant_value_json->valueint;
                    if(invariants[i][k][l] >= limit){
                        printf("Erreur de syntaxe : La valeur d'indice [%d][%d] du tableau d'invariant de la localite %s est superieure a la limite fixee (%d).\n", k, l, locations[i], limit);
                        cJSON_Delete(json);
                        exit(EXIT_FAILURE);
                    } //Détection d'une valeur supérieure ou égale à un milliard
                } 
                else if(cJSON_IsString(invariant_value_json) && (strcmp(invariant_value_json->valuestring, "infty") == 0)) invariants[i][k][l] = infty;
                else{
                    printf("Erreur de syntaxe : Le type de la valeur d'indice [%d][%d] du tableau d'invariant de la localite %s est incorrect.\nType attendu : Int ou String infty.\n", k, l, locations[i]);
                    cJSON_Delete(json);
                    exit(EXIT_FAILURE);
                } //Détection d'une erreur de type pour une valeur d'invariant de la localité i
            }
        }

        //Parsing des transitions de la localité i
        cJSON* transitions_json = cJSON_GetObjectItemCaseSensitive(location_json, "transitions"); //Récupération de la valeur de transition de la localité i
        if(!transitions_json){
            printf("Erreur de syntaxe : La valeur de l'objet transitions de la localite %s est vide.\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une absence de définition des transitions pour la localité i 

        if(!cJSON_IsArray(transitions_json)){
            printf("Erreur de syntaxe : Le type de la valeur de l'objet transitions de la localite %s est incorrect.\nType attendu : Array.\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour l'objet transitions

        int length_transitions = cJSON_GetArraySize(transitions_json); //Récupération de la taille du tableau de transitions pour la localité i 
        nb_transitions_locations[i] = length_transitions;
        transitions[i] = malloc(length_transitions * sizeof(Transition*)); //Allocation de l'espace mémoire pour le tableau de transition de la localité i 
        if(!transitions[i]){
            printf("Erreur d'allocation memoire pour le tableau de transitions de la localite %s (transitions[i]).\n", locations[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour transitions[i]

        for(int k = 0; k < length_transitions; k++){
            cJSON* transition_json = cJSON_GetArrayItem(transitions_json, k); //Récupération de la transition k du tableau de transitions pour la localité i
            if(!transition_json){
                printf("Erreur de syntaxe : Le tableau de transition de la localite %s est vide.\n", locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection de l'absence de définition pour la transition k de la localité i

            if(!cJSON_IsArray(transition_json)){
                printf("Erreur de syntaxe : Le type de la transition %d de la localite %s est incorrect.\nType attendu : Array.\n", k, locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            }

            transitions[i][k] = malloc(sizeof(Transition)); //Allocation de l'espace mémoire pour la transition k de la localité i
            if(!transitions[i][k]){
                printf("Erreur d'allocation memoire pour la transition %d de la localite %s (transitions[i][j]).\n", k, locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur d'allocation mémoire pour la transition k de la localité i

            int length_transition = cJSON_GetArraySize(transition_json); //Récupération de la taille de la transition k de la localité i
            if(length_transition != 4){
                printf("Erreur de syntaxe : La taille de la transition %d de la localite %s est incorrecte.\n", k, locations[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une taille de transition inattendue pour la ransition k de la localité i

            for(int l = 0; l < length_transition; l++){
                cJSON* transition_value_json = cJSON_GetArrayItem(transition_json, l); //Récupération de chaque valeur de la transition k de la localité i
                if(!transition_value_json){
                    printf("Erreur de syntaxe : La valeur d'indice %l de la transition %d de la localite %s est vide.\n", k, locations[i]);
                    cJSON_Delete(json);
                    exit(EXIT_FAILURE);
                } //Détection d'une absence de définition de la valeur l de la transition k de la localité i

                int length_transition_array = cJSON_GetArraySize(transition_value_json); //Récupération de la taille du tableau pour la valeur d'indice l de la transition k de la localité i

                switch(l){
                    case 0:
                        if(!cJSON_IsString(transition_value_json)){
                            printf("Erreur de syntaxe : Le type de l'action de la transition %d de la localite %s est incorrect.\nType attendu : String.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur de type pour l'action de la transition k de la localité i

                        int count_action = 0;
                        for(int m = 0; m < nb_actions; m++){
                            if(strcmp(transition_value_json->valuestring, actions[m]) == 0){
                                transitions[i][k]->label_action = m; //Affectation de l'indice du nom de l'action dans actions à la valeur d'action de la transition k de la localité i 
                                break; //Arrêt de la boucle pour éviter des itérations inutiles
                            }
                            else count_action++;
                        }
                        if(count_action == nb_actions){
                            printf("Erreur de syntaxe : L'action entree pour la transition %d de la localite %s n'est pas definie.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une action non définie pour la transition k de la localité i 
                        break;

                    case 1:
                        if(!cJSON_IsArray(transition_value_json)){
                            printf("Erreur de syntaxe : Le type de garde de la transition %d de la localite %s est incorrect.\nType attendu : Array.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur de type pour la garde de la transition k de la localité i
                        
                        if(length_transition_array != (nb_clocks+1)){
                            printf("Erreur de syntaxe : La taille de garde de la transition %d de la localite %s est incorrecte.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une taille de garde inattendue pour la transition k de la localité i

                        transitions[i][k]->guard = malloc((nb_clocks+1) * sizeof(int*)); //Allocation de l'espace mémoire pour l'invariant de la transition k de la localité i
                        if(!transitions[i][k]->guard){
                            printf("Erreur d'allocation memoire pour la garde de la transition %d de la localite %s (transitions[i][j]->guard).\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur d'allocation mémoire pour la garde de la transition k de la localité i

                        for(int m = 0; m < (nb_clocks+1); m++){
                            cJSON* transition_value_guard_json = cJSON_GetArrayItem(transition_value_json, m); //Récupération du tableau d'entier d'indice m de l'invariant de la transition k de la localité i
                            if(!cJSON_IsArray(transition_value_guard_json)){
                                printf("Erreur de syntaxe : Le type de la valeur d'indice %d de garde de la transition %d de la localite %s est incorrect.\nType attendu : Array.\n", m, k, locations[i]);
                                cJSON_Delete(json);
                                exit(EXIT_FAILURE);
                            } //Détection d'une erreur de type pour la ligne m de la matrice d'invariant de la transition k de la localité i

                            if(cJSON_GetArraySize(transition_value_guard_json) != (nb_clocks+1)){
                                printf("Erreur de syntaxe : La taille de garde de la transition %d de la localite %s est incorrecte.\n", k, locations[i]);
                                cJSON_Delete(json);
                                exit(EXIT_FAILURE);
                            } //Détection d'une taille inattendue pour la garde de la transition k de la localité i 

                            transitions[i][k]->guard[m] = malloc((nb_clocks+1) * sizeof(int)); //Allocation de l'espace mémoire pour l'indice m de l'invariant de la transition k de la localité i
                            if(!transitions[i][k]->guard[m]){
                                printf("Erreur d'allocation memoire pour la garde de la transition %d de la localite %s (transitions[i][j]->guard[k]).\n", k, locations[i]);
                                cJSON_Delete(json);
                                exit(EXIT_FAILURE);
                            } //Détection d'une erreur d'allocation mémoire pour la garde de la transition k de la localité i

                            for(int n = 0; n < (nb_clocks+1); n++){
                                cJSON* transition_value_guard_value_json = cJSON_GetArrayItem(transition_value_guard_json, n); //Récupération de la valeur d'indice [m][n] de l'invariant de la transition k de la localité i
                                if(!transition_value_guard_value_json){
                                    printf("Erreur de syntaxe : La valeur de garde d'indice [%d][%d] de la transition %d de la localite %s est nulle.\n", m, n, k, locations[i]);
                                    cJSON_Delete(json);
                                    exit(EXIT_FAILURE);
                                } //Détection d'une absence de définition d'une valeur de garde de la transition k de la localité i

                                if(cJSON_IsNumber(transition_value_guard_value_json)){
                                    transitions[i][k]->guard[m][n] = transition_value_guard_value_json->valueint; //Si la valeur est un nombre, alore elle est récupérée dans l'invariant de la transition k de la localité i à l'indice [m][n]                                   
                                    if(transitions[i][k]->guard[m][n] >= limit){
                                        printf("Erreur de syntaxe : La valeur de garde d'indice [%d][%d] de la transition %d de la localite %s est superieure a la limite fixee (%d).\n", m, n, k, locations[i], limit);
                                        cJSON_Delete(json);
                                        exit(EXIT_FAILURE);
                                    } //Détection d'une valeur supérieure ou égale à un milliard
                                }
                                else if(cJSON_IsString(transition_value_guard_value_json) && (strcmp(transition_value_guard_value_json->valuestring, "infty") == 0)) transitions[i][k]->guard[m][n] = infty; //Si la valeur est un string qui correspond à infty, alors la case mémoire d'indice [m][n] de l'invariant de la transition k de la localité i prend la valeur infty
                                else{
                                    printf("Erreur de syntaxe : Le type de la valeur de garde d'indice [%d][%d] de la transition %d de la localite %s est incorrect.\nType attendu : Int ou String infty.\n", m, n, k, locations[i]);
                                    cJSON_Delete(json);
                                    exit(EXIT_FAILURE);
                                } //Détection d'une erreur de type pour une valeur de garde de la transition k de la lcoalité i
                            }
                        }
                        break;

                    case 2:
                        if(!cJSON_IsArray(transition_value_json)){
                            printf("Erreur de syntaxe : Le type de l'ensemble reset de la transition %d de la localite %s est incorrect.\nType attendu : Array.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur de type pour l'ensemble d'horloges à reset de la transition k de la localité i

                        if(length_transition_array > nb_clocks){
                            printf("Erreur de syntaxe : La taille de l'ensemble reset pour la transition %d de la localite %s est incorrecte.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une taille de reset supérieure au nombre d'horloges pour la transition k de la localité i

                        transitions[i][k]->reset = malloc(nb_clocks * sizeof(int)); //Allocation de l'espace mémoire pour l'ensemble reset de la transition k de la localité i
                        if(!transitions[i][k]->reset){
                            printf("Erreur d'allocation memoire pour l'ensemble reset de la transition %d de la localite %s (transitions[i][j]->reset).\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur d'allocation mémoire pour l'ensemble reset de la transition k de la localité i

                        for(int m = 0; m < nb_clocks; m++) transitions[i][k]->reset[m] = infty; //Initialisation des valeurs de reset à infty pour la transition k de la localité i

                        int index_d = 0; //Index de parcours du tableau doublons_reset
                        char** doublons_reset = malloc(nb_clocks * sizeof(char*)); //Allocation de l'espace mémoire pour doublons_reset qui permettra de détecter des doublons dans l'ensemble reset déclaré
                        if(!doublons_reset){
                            printf("Erreur d'allocation memoire pour le tableau de chaine de caractère temporaire de verification de doublons pour l'ensemble reset de la transition %d de la localite %s.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur d'allocation mémoire por doublons_reset

                        for(int m = 0; m < length_transition_array; m++){
                            cJSON* transition_value_reset_json = cJSON_GetArrayItem(transition_value_json, m); //Récupération du nom de l'horloge à reset pour la transition k de la localité i
                            if(!transition_value_reset_json){
                                printf("Erreur de syntaxe : La valeur de reset de l'horloge %s de l'ensemble reset de la transition %d de la localite %s est nulle.\n", names_clocks[m], k, locations[i]);
                                cJSON_Delete(json);
                                exit(EXIT_FAILURE);
                            } //Détection d'une absence de définition d'une valeur de l'ensemble reset de la transition k de la localité i

                            if(!cJSON_IsString(transition_value_reset_json)){
                                printf("Erreur de syntaxe : Le type de la valeur de reset de l'horloge %s de la transition %d de la localite %s est incorrect.\nType attendu : Int ou String infty.\n", names_clocks[m], k, locations[i]);
                                cJSON_Delete(json);
                                exit(EXIT_FAILURE);
                            } //Détection d'une erreur de type pour le nom d'une horloge à reset pour la transition k de la localité i

                            int count_reset = 0; //Variable permetttant de déterminer si le nom d'horloge entré pour la transition k de la localité i est défini
                            for(int n = 0; n < nb_clocks; n++){
                                if(strcmp(transition_value_reset_json->valuestring, names_clocks[n]) == 0){
                                    doublons_reset[index_d] = strdup(transition_value_reset_json->valuestring);
                                    for(int o = 0; o < index_d; o++){
                                        if(strcmp(transition_value_reset_json->valuestring, doublons_reset[o]) == 0){
                                            printf("Erreur de syntaxe : L'horloge %s est definie en double dans le tableau de reset de la transition %d de la localite %s.\n", doublons_reset[index_d], k, locations[i]);
                                            cJSON_Delete(json);
                                            exit(EXIT_FAILURE);
                                        } //Détection de doublons pour reset
                                    }
                                    index_d++;
                                    transitions[i][k]->reset[n] = 0; //Modification de la valeur de l'indice de l'horloge du tableau names_clocks à 0
                                    break; //Arrêt de la boucle pour éviter des itérations inutiles
                                }
                                else count_reset++;
                            }
                            if(count_reset == nb_clocks){
                                printf("Erreur de syntaxe : Le nom d'horloge pour la transition %d de la localite %s n'est pas defini.", k, locations[i]);
                                cJSON_Delete(json);
                                exit(EXIT_FAILURE);
                            } //Détection d'une horloge non définie pour la transition k de la localité i
                        }
                        break;

                    case 3:
                        if(!cJSON_IsString(transition_value_json)){
                            printf("Erreur de syntaxe : Le type de la localite d'entree de la transition %d de la localite %s est incorrect.\nType attendu : String.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une erreur de type pour l' de la transition k de la localité i

                        int count_loc = 0;
                        for(int m = 0; m < nb_locations; m++){
                            if(strcmp(transition_value_json->valuestring, locations[m]) == 0){
                                transitions[i][k]->location_in = m; //Affectation de l'indice de la localité dans le tableau locations à la valeur de localité d'entrée de la transition k de la localité i
                                break; //Arrêt de la boucle pour éviter des itérations inutiles
                            }
                            else count_loc++;
                        }
                        if(count_loc == nb_locations){
                            printf("Erreur de syntaxe : La localite entree pour la transition %d de la localite %s n'est pas definie.\n", k, locations[i]);
                            cJSON_Delete(json);
                            exit(EXIT_FAILURE);
                        } //Détection d'une localité non définie pour la transition k de la localité i
                        break;
                }
            }
        }
    }
    
    //Parsing des informations de variables
    cJSON* variables_json = cJSON_GetObjectItemCaseSensitive(json, "variables"); //Récupération de la valeur associée à l'item variables
    if(!variables_json){
        printf("Erreur de syntaxe : Aucun objet variables detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE); 
    } //Détection de l'absence de l'objet variables dans le json

    if(!cJSON_IsObject(variables_json)){
        printf("Erreur de syntaxe : Le type de l'objet variables est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet variables

    //Parsing des informations pour le fichier structure_variable.h
    cJSON* definition_json = cJSON_GetObjectItemCaseSensitive(variables_json, "definition"); //Récupération de la valeur associée à l'item definition
    if(!definition_json){
        printf("Erreur de syntaxe : Aucun objet definition detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet definition dans l'objet variables du json

    if(!cJSON_IsObject(definition_json)){
        printf("Erreur de syntaxe : Le type de l'objet definition est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet definition

    cJSON* define_json = cJSON_GetObjectItemCaseSensitive(definition_json, "define"); //Récupération de la valeur associée à l'item define
    if(!define_json){
        printf("Erreur de syntaxe : Aucun objet define detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet define dans l'objet definition

    if(!cJSON_IsArray(define_json)){
        printf("Erreur de syntaxe : Le type de l'objet define est incorrect.\nType attendu : Array.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour la valeur de define

    int length_define = cJSON_GetArraySize(define_json);
    nb_define = length_define;
    def_variables_define = malloc(length_define * sizeof(line)); //Allocation de l'espace mémoire pour le tableau des lignes de code definissant les constantes symboliques
    if(!def_variables_define){
        printf("Erreur d'allocation memoire pour le tableau des lignes de code definissant les constantes symboliques (def_variables_define).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour def_variables_define

    for(int i = 0; i < length_define; i++){
        cJSON* define_code_line = cJSON_GetArrayItem(define_json, i); //Récupération de la valeur d'indice i du tableau define
        if(!define_code_line){
            printf("Erreur de syntaxe : La ligne de code %d de define est vide.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne de code non définie.

        if(!cJSON_IsString(define_code_line)){
            printf("Erreur de syntaxe : Le type de la ligne de code %d de define est incorrect.\nType attendu : String.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour la ligne de code i de define

        def_variables_define[i] = strdup(define_code_line->valuestring);
    }

    cJSON* typedef_json = cJSON_GetObjectItemCaseSensitive(definition_json, "typedef"); //Récupération de la valeur associée à l'item typedef
    if(!typedef_json){
        printf("Erreur de syntaxe : Aucun objet typedef detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet typedef

    if(!cJSON_IsObject(typedef_json)){
        printf("Erreur de syntaxe : Le type de l'objet typedef est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet typedef

    nb_clines_typedef = malloc(2 * sizeof(int**));
    if(!nb_clines_typedef){
        printf("Erreur d'allocation memoire pour le tableau stockant le nombre de lignes de codes pour chaque typedef (nb_clines_typedef).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour nb_clines_typdef

    label_typedef = malloc(2 * sizeof(char**));
    if(!label_typedef){
        printf("Erreur d'allocation memoire pour le tableau stockant les noms de chaque typedef (label_typedef).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour label_typedef

    def_variables_typedef = malloc(2 * sizeof(line**));
    if(!def_variables_typedef){
        printf("Erreur d'allocation memoire pour le tableau stockant les lignes de code de chaque typedef (def_variables_typedef).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour def_variables_typedef

    dim_elements_typedef_variables = malloc(2 * sizeof(int**));
    if(!dim_elements_typedef_variables){
        printf("Erreur d'allocation memoire pour le tableau stockant les dimensions de chaque champ de chaque typedef (dim_elements_typedef_variables).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour dim_elements_typedef_variables

    cJSON* struct_json = cJSON_GetObjectItemCaseSensitive(typedef_json, "struct"); //Récupération de la valeur associée à l'item struct
    if(!struct_json){
        printf("Erreur de syntaxe : Aucun objet struct detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet struct

    if(!cJSON_IsObject(struct_json)){
        printf("Erreur de syntaxe : Le type de l'objet struct est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet struct

    cJSON* item_struct = NULL;
    int length_struct = cJSON_GetArraySize(struct_json);
    nb_typedef_struct = length_struct;
    nb_clines_typedef[0] = malloc(length_struct * sizeof(int));
    if(!nb_clines_typedef[0]){
        printf("Erreur d'allocation memoire pour le tableau stockant le nombre de lignes de codes pour chaque typedef (nb_clines_typedef[0]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour nb_clines_typdef[0]

    label_typedef[0] = malloc(length_struct * sizeof(char*));
    if(!label_typedef[0]){
        printf("Erreur d'allocation memoire pour le tableau stockant les noms de chaque typedef (label_typedef[0]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour label_typedef[0]

    def_variables_typedef[0] = malloc(length_struct * sizeof(line*));
    if(!def_variables_typedef[0]){
        printf("Erreur d'allocation memoire pour le tableau stockant les lignes de code de chaque typedef (def_variables_typedef[0]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour def_variables_typedef[0]

    dim_elements_typedef_variables[0] = malloc(length_struct * sizeof(int*));
    if(!dim_elements_typedef_variables[0]){
        printf("Erreur d'allocation memoire pour le tableau stockant les dimensions de chaque champ de chaque typedef (dim_elements_typedef_variables[0]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour dim_elements_typedef_variables[0]

    int count_names_typedef = 0; //Index de parcours du tableau des noms des typedef struct
    int count_item_struct_variable = 0; //Variable pour déterminer la définition du typedef type variable
    cJSON_ArrayForEach(item_struct, struct_json){
        if(!item_struct){
            printf("Erreur de syntaxe : Un item de l'objet struct n'est pas defini.\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un item vide dans l'objet struct

        label_typedef[0][count_names_typedef] = strdup(item_struct->string);
        if(strcmp(label_typedef[0][count_names_typedef], "Variable") == 0) count_item_struct_variable = 1;
        count_names_typedef++;
    }

    if(count_item_struct_variable != 1){
        printf("Erreur de syntaxe : Le typedef type variable n'est pas defini.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence du typedef type variable

    for(int i = 0; i < count_names_typedef; i++){
        if(strcmp(label_typedef[0][i], "Variable") == 0){
            label_typedef[0][i] = label_typedef[0][count_names_typedef-1];
            label_typedef[0][count_names_typedef-1] = "Variable";
        }
    } //Placement de la structure variable en premier

    for(int i = 0; i < length_struct; i++){
        cJSON* typedef_struct_json = cJSON_GetObjectItem(struct_json, label_typedef[0][i]);
        if(!typedef_struct_json){
            printf("Erreur de syntaxe : Le typedef type %s n'est pas defini.\n", label_typedef[0][i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un typedef struct non défini

        if(!cJSON_IsArray(typedef_struct_json)){
            printf("Erreur de syntaxe : Le type du typedef type %s est incorrect.\nType attendu : Array.\n", label_typedef[0][i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour le typedef struct

        int length_typedef_struct = cJSON_GetArraySize(typedef_struct_json);
        nb_clines_typedef[0][i] = length_typedef_struct;
        dim_elements_typedef_variables[0][i] = malloc(length_typedef_struct * sizeof(int));
        if(!dim_elements_typedef_variables[0][i]){
            printf("Erreur d'allocation memoire pour le tableau stockant les dimensions de chaque champ de chaque typedef (dim_elements_typedef_variables[0][i]).\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour dim_elements_typedef_variables[0][i]

        def_variables_typedef[0][i] = malloc(length_typedef_struct * sizeof(line));
        if(!def_variables_typedef[0][i]){
            printf("Erreur d'allocation memoire pour le tableau stockant les lignes de code de chaque typedef (def_variables_typedef[0][i]).\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour def_variables_typedef[0][i]

        for(int j = 0; j < length_typedef_struct; j++){
            cJSON* struct_code_line = cJSON_GetArrayItem(typedef_struct_json, j);
            if(!struct_code_line){
                printf("Erreur de syntaxe : La ligne de code %d du typedef type %s n'est pas définie.\n", j, label_typedef[0][i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une ligne de code vide pour un typedef type

            if(!cJSON_IsString(struct_code_line)){
                printf("Erreur de syntaxe : Le type de la ligne de code %d du typedef type %s est incorrect.\nType attendu : String.\n", j, label_typedef[0][i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de type pour la ligne de code j du typedef type

            def_variables_typedef[0][i][j] = strdup(struct_code_line->valuestring);
            int count_dim = 0;
            for(int k = 0; k < strlen(def_variables_typedef[0][i][j]); k++){
                if((def_variables_typedef[0][i][j][k] == '*')||(def_variables_typedef[0][i][j][k] == '[')) count_dim++;
            } //Compte la dimension du champ du typedef type
            dim_elements_typedef_variables[0][i][j] = count_dim;      
        }
    }

    cJSON* primitive_json = cJSON_GetObjectItemCaseSensitive(typedef_json, "primitive"); 
    if(!primitive_json){
        printf("Erreur de syntaxe : Aucun objet primitive detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet primitive

    if(!cJSON_IsObject(primitive_json)){
        printf("Erreur de syntaxe : Le type de l'objet primitive est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet primitive

    cJSON* item_primitive = NULL;
    int length_primitive = cJSON_GetArraySize(primitive_json);
    nb_typedef_primitive = length_primitive;
    nb_clines_typedef[1] = malloc(length_primitive * sizeof(int));
    if(!nb_clines_typedef[1]){
        printf("Erreur d'allocation memoire pour le tableau stockant le nombre de lignes de codes pour chaque typedef (nb_clines_typedef[1]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour nb_clines_typdef[1]

    label_typedef[1] = malloc(length_primitive * sizeof(char*));
    if(!label_typedef[1]){
        printf("Erreur d'allocation memoire pour le tableau stockant les noms de chaque typedef (label_typedef[1]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour label_typedef[1]

    def_variables_typedef[1] = malloc(length_primitive * sizeof(line*));
    if(!def_variables_typedef[1]){
        printf("Erreur d'allocation memoire pour le tableau stockant les lignes de code de chaque typedef (def_variables_typedef[1]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour def_variables_typedef[1]

    dim_elements_typedef_variables[1] = malloc(length_primitive * sizeof(int*));
    if(!dim_elements_typedef_variables[1]){
        printf("Erreur d'allocation memoire pour le tableau stockant les dimensions de chaque champ de chaque typedef (dim_elements_typedef_variables[1]).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour dim_elements_typedef_variables[1]

    count_names_typedef = 0; //Index de parcours du tableau des noms des typedef primitives
    cJSON_ArrayForEach(item_primitive, primitive_json){
        if(!item_primitive){
            printf("Erreur de syntaxe : Un item de l'objet primitive n'est pas defini.\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un item vide dans l'objet struct

        label_typedef[1][count_names_typedef] = strdup(item_primitive->string);
        count_names_typedef++;
    }

    for(int i = 0; i < length_primitive; i++){
        cJSON* typedef_primitive_json = cJSON_GetObjectItem(primitive_json, label_typedef[1][i]);
        if(!typedef_primitive_json){
            printf("Erreur de syntaxe : Le typedef type %s n'est pas defini.\n", label_typedef[1][i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'un item de typedef primitive non défini

        if(!cJSON_IsArray(typedef_primitive_json)){
            printf("Erreur de syntaxe : Le type du typedef type %s est incorrect.\nType attendu : Array.\n", label_typedef[1][i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour le typedef primitive

        int length_typedef_primitive = cJSON_GetArraySize(typedef_primitive_json);
        if(length_typedef_primitive != 1){
            printf("Erreur de syntaxe : Un typedef type primitif ne peux pas avoir plusieurs lignes.\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur sur le nombre de lignes

        nb_clines_typedef[1][i] = length_typedef_primitive;
        dim_elements_typedef_variables[1][i] = malloc(length_typedef_primitive * sizeof(int));
        if(!dim_elements_typedef_variables[1][i]){
            printf("Erreur d'allocation memoire pour le tableau stockant les dimensions de chaque champ de chaque typedef (dim_elements_typedef_variables[1][i]).\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour dim_elements_typedef_variables[1][i]

        def_variables_typedef[1][i] = malloc(length_typedef_primitive * sizeof(line));
        if(!def_variables_typedef[1][i]){
            printf("Erreur d'allocation memoire pour le tableau stockant les lignes de code de chaque typedef (def_variables_typedef[1][i]).\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour def_variables_typedef[1][i]

        for(int j = 0; j < length_typedef_primitive; j++){
            cJSON* primitive_code_line = cJSON_GetArrayItem(typedef_primitive_json, j);
            if(!primitive_code_line){
                printf("Erreur de syntaxe : La ligne de code %d du typedef type %s n'est pas définie.\n", j, label_typedef[1][i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une ligne de code vide pour un typedef type

            if(!cJSON_IsString(primitive_code_line)){
                printf("Erreur de syntaxe : Le type de la ligne de code %d du typedef type %s est incorrect.\nType attendu : String.\n", j, label_typedef[1][i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de type pour la ligne de code j du typedef type

            def_variables_typedef[1][i][j] = strdup(primitive_code_line->valuestring);
            int count_dim = 0;
            for(int k = 0; k < strlen(def_variables_typedef[1][i][j]); k++){
                if((def_variables_typedef[1][i][j][k] == '*')||(def_variables_typedef[1][i][j][k] == '[')) count_dim++;
            } //Compte la dimension du champ du typedef type
            dim_elements_typedef_variables[1][i][j] = count_dim;      
        }
    }

    //Parsing des lignes de code de la fonction init_variables
    cJSON* init_variables_json = cJSON_GetObjectItemCaseSensitive(variables_json, "init_variables"); //Récupération de la valeur associée à l'item init_variables
    if(!init_variables_json){
        printf("Erreur de syntaxe : Aucun objet init_variables detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet init_variables dans l'objet variables

    if(!cJSON_IsArray(init_variables_json)){
        printf("Erreur de syntaxe : Le type de l'objet init_variables est incorrect.\nType attendu : Array.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet init_variables

    int length_init_variables = cJSON_GetArraySize(init_variables_json);
    nb_clines_init_variables = length_init_variables;
    init_variables_function = malloc(length_init_variables * sizeof(line)); //Allocation de l'espace mémoire pour le tableau des lignes de code de la fonction init_variables
    if(!init_variables_function){
        printf("Erreur d'allocation memoire pour le tableau des lignes de code de la fonction init_variables (init_variables_function).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour init_variables_function

    for(int i = 0; i < length_init_variables; i++){
        cJSON* init_variables_code_line = cJSON_GetArrayItem(init_variables_json, i); //Récupération de la valeur d'indice i du tableau init_variables
        if(!init_variables_code_line){
            printf("Erreur de syntaxe : La ligne de code %d de init_variables est vide.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une ligne de code % d'init_variables non définie

        if(!cJSON_IsString(init_variables_code_line)){
            printf("Erreur de syntaxe : Le type de la ligne de code  %d  de init_variables est incorrect.\nType attendu : String.\n", i);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour la ligne de code i de la fonction init_variables

        init_variables_function[i] = strdup(init_variables_code_line->valuestring);
    }

    //Parsing des update_functions
    cJSON* update_functions_json = cJSON_GetObjectItemCaseSensitive(variables_json, "update_functions");
    if(!update_functions_json){
        printf("Erreur de syntaxe : Aucun objet update_functions detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet update_functions

    if(!cJSON_IsObject(update_functions_json)){
        printf("Erreur de syntaxe : Le type de l'objet update_functions est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet update_functions

    nb_clines_updatef = malloc(nb_actions * sizeof(int));
    if(!nb_clines_updatef){
        printf("Erreur d'allocation memoire pour le tableau de sauvegarde du nombre de ligne de code pour chaque fonction d'update (nb_clines_updatef).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour nb_clines_updatef

    update_functions = malloc(nb_actions * sizeof(line*));
    if(!update_functions){
        printf("Erreur d'allocation memoire pour le tableau des lignes de code des fonctions d'update (update_functions).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour update_functions

    for(int i = 0; i < nb_actions; i++){
        cJSON* update_function_json = cJSON_GetObjectItemCaseSensitive(update_functions_json, actions[i]);
        if(!update_function_json){
            printf("Erreur de syntaxe : La fonction d'update pour l'action %s n'est pas definie.\n", actions[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection de l'absence de définition pour la fonction update de l'action i

        if(!cJSON_IsArray(update_function_json)){
            printf("Erreur de syntaxe : Le type de l'objet %s (fonction d'update) est incorrect.\nType attendu : Array.\n", actions[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour la valeur de la fonction d'update de l'action i

        int length_update_function = cJSON_GetArraySize(update_function_json);
        nb_clines_updatef[i] = length_update_function;
        update_functions[i] = malloc(length_update_function * sizeof(line));
        if(!update_functions[i]){
            printf("Erreur d'allocation memoire pour le tableau des lignes de code des fonctions d'update (update_functions[i]).\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour update_functions[i]

        for(int j = 0; j < length_update_function; j++){
            cJSON* update_code_line = cJSON_GetArrayItem(update_function_json, j);
            if(!update_code_line){
                printf("Erreur de syntaxe : La ligne de code %d de la fonction d'update de l'action %s n'est pas definie.\n", j, actions[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une absence de définition pour la ligne j de la fonction d'update de l'action i

            if(!cJSON_IsString(update_code_line)){
                printf("Erreur de syntaxe : Le type de la ligne de code %d de la fonction d'update de l'action %s est incorrect.\nType attendu : String.\n", j, actions[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de type pour la ligne j de la fonction d'update de l'action i

            update_functions[i][j] = strdup(update_code_line->valuestring);
        }
    }

    cJSON* constraints_json = cJSON_GetObjectItemCaseSensitive(variables_json, "constraints");
    if(!constraints_json){
        printf("Erreur de syntaxe : Aucun objet constraints detecte.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection de l'absence de l'objet pour les contraintes

    if(!cJSON_IsObject(constraints_json)){
        printf("Erreur de syntaxe : Le type de la ligne de la valeur de l'objet constraints est incorrect.\nType attendu : Object.\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur de type pour l'objet constraints

    nb_clines_constraints = malloc(nb_actions * sizeof(int));
    if(!nb_clines_constraints){
        printf("Erreur d'allocation memoire pour le tableau de sauvegarde du nombre de lignes de code pour les fonctions de contraintes (nb_clines_constraints).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour nb_clines_constraints

    constraints_functions = malloc(nb_actions * sizeof(line*));
    if(!constraints_functions){
        printf("Erreur d'allocation memoire pour le tableau des lignes de code des fonctions de contraintes (constraints_functions).\n");
        cJSON_Delete(json);
        exit(EXIT_FAILURE);
    } //Détection d'une erreur d'allocation mémoire pour constraints_functions

    for(int i = 0; i < nb_actions; i++){
        cJSON* constraint_json = cJSON_GetObjectItemCaseSensitive(constraints_json, actions[i]);
        if(!constraint_json){
            printf("Erreur de syntaxe : La fonction de contrainte pour l'action %s n'est pas definie.\n", actions[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection de l'absence de définition pour la fonction de contrainte de l'action i

        if(!cJSON_IsArray(constraint_json)){
            printf("Erreur de syntaxe : Le type de l'objet %s (fonction de contrainte) est incorrect.\nType attendu : Array.\n", actions[i]);
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur de type pour la valeur de la fonction de contrainte de l'action i

        int length_constraint_function = cJSON_GetArraySize(constraint_json);
        nb_clines_constraints[i] = length_constraint_function;
        constraints_functions[i] = malloc(length_constraint_function * sizeof(line));
        if(!constraints_functions[i]){
            printf("Erreur d'allocation memoire pour le tableau des lignes de code des fonctions de contraintes (constraints_functions[i]).\n");
            cJSON_Delete(json);
            exit(EXIT_FAILURE);
        } //Détection d'une erreur d'allocation mémoire pour constraints_functions[i]

        for(int j = 0; j < length_constraint_function; j++){
            cJSON* constraint_code_line = cJSON_GetArrayItem(constraint_json, j);
            if(!constraint_code_line){
                printf("Erreur de syntaxe : La ligne de code %d de la fonction de contrainte de l'action %s n'est pas definie.\n", j, actions[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une absence de définition pour la ligne j de la fonction de contrainte de l'action i

            if(!cJSON_IsString(constraint_code_line)){
                printf("Erreur de syntaxe : Le type de la ligne de code %d de la fonction de contrainte de l'action %s est incorrect.\nType attendu : String.\n", j, actions[i]);
                cJSON_Delete(json);
                exit(EXIT_FAILURE);
            } //Détection d'une erreur de type pour la ligne j de la fonction de contrainte de l'action i

            constraints_functions[i][j] = strdup(constraint_code_line->valuestring);
        }
    }
    cJSON_Delete(json);
}

void fill_parseInfos_struct(ParseInfos* parseInfos){
    char* json_donnee = read_model_json("json_model.json");
    if(json_donnee){
        parse_model_json(json_donnee);
        free(json_donnee);
    }

    //Affectation des structures parsées aux champs de parseInfos
    parseInfos->nb_actions = nb_actions;
    parseInfos->actions = actions;
    parseInfos->nb_clocks = nb_clocks;
    parseInfos->names_clocks = names_clocks;
    parseInfos->nb_locations = nb_locations;
    parseInfos->locations = locations;
    parseInfos->invariants = invariants;
    parseInfos->transitions = transitions;
    parseInfos->nb_transitions_locations = nb_transitions_locations;

    //Affectation des structures parsées aux champs de parseInfos pour les variables
    parseInfos->nb_define = nb_define;
    parseInfos->def_variables_define = def_variables_define;
    parseInfos->nb_clines_typedef = nb_clines_typedef;
    parseInfos->nb_typedef_struct = nb_typedef_struct;
    parseInfos->nb_typedef_primitive = nb_typedef_primitive;
    parseInfos->label_typedef = label_typedef;
    parseInfos->def_variables_typedef = def_variables_typedef;
    parseInfos->dim_elements_typedef_variables = dim_elements_typedef_variables;
    parseInfos->nb_clines_init_variables = nb_clines_init_variables;
    parseInfos->init_variables_function = init_variables_function;
    parseInfos->nb_clines_updatef = nb_clines_updatef;
    parseInfos->update_functions = update_functions;
    parseInfos->nb_clines_constraints = nb_clines_constraints;
    parseInfos->constraints_functions = constraints_functions;
}