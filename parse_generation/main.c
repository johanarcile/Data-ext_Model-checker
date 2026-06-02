#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "parse.h"
#include "generation.h"

int main(int argc, char *argv[]){
    char *modelesPath = getenv("ModelesLocation");
    char newModelesPath[4000];
    int change = 0;
    if(modelesPath == NULL){
        printf("Erreur de syntaxe : La variable d'environnement ModelesLocation n'a pas ete definie. Veuillez entrer le chemin d'acces au dossier modeles_generation.\n");
        if(fgets(newModelesPath, sizeof(newModelesPath), stdin) != NULL) newModelesPath[strcspn(newModelesPath, "\n")] = '\0';
#ifdef _WIN32
        if(_putenv_s("ModelesLocation", newModelesPath) != 0){
#else
        if(setenv("ModelesLocation", newModelesPath, 1) != 0){
#endif
            printf("Erreur setenv.\n");
            free(modelesPath);
            exit(EXIT_FAILURE);
        }
        modelesPath = getenv("ModelesLocation");
        change = 1;
    } //Création de la variable d'environnement ModelesLocation si elle n'existe pas

    else{
        char testPath[4096];
        strcpy(testPath, modelesPath);
        strcat(testPath, "/test.txt");
        FILE* test = fopen(testPath, "w");
        if(!test){
            printf("Erreur de syntaxe : Le chemin d'acces du dossier modeles_generation n'est pas a jour. Veuillez entrer le nouveau chemin.\n");
            if(fgets(newModelesPath, sizeof(newModelesPath), stdin) != NULL) newModelesPath[strcspn(newModelesPath, "\n")] = '\0';
#ifdef _WIN32
            if(_putenv_s("ModelesLocation", newModelesPath) != 0){
#else
            if(setenv("ModelesLocation", newModelesPath, 1) != 0){
#endif
                printf("Erreur setenv.\n");
                free(modelesPath);
                fclose(test);
                exit(EXIT_FAILURE);
            }
            modelesPath = getenv("ModelesLocation");
            change = 1;
        }
        fclose(test);
    } //Modification de la variable d'environnement ModelesLocation si elle est incorrecte

    ParseInfos parseInfos;
    fill_parseInfos_structure(argv[1], &parseInfos);
    generation(argv[2], modelesPath, &parseInfos);
    if(change == 1){
#ifdef _WIN32
        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_SET_VALUE, &hKey);
        if(result != ERROR_SUCCESS){
            printf("Erreur d'acces aux cles pour enregistrer la variable d'environnement.\n");
            free(modelesPath);
            exit(EXIT_FAILURE);
        }
        result = RegSetValueExA(hKey, "ModelesLocation", 0, REG_SZ, (const BYTE *)modelesPath, (DWORD)(strlen(modelesPath) + 1));
        RegCloseKey(hKey);
        SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
#else
        char path[512];
        snprintf(path, sizeof(path), "%s/.bashrc", getenv("HOME"));
        FILE *f = fopen(path, "a");
        if(f == NULL){
            printf("Erreur d'ouverture du fichier contenant les variables d'environement.\n");
            free(modelesPath);
            fclose(f);
            exit(EXIT_FAILURE);
        }
        fprintf(f, "\nexport ModelesLocation=\"%s\"\n", modelesPath);
        fclose(f);
#endif
    }
    free(modelesPath);
    return 0;
}