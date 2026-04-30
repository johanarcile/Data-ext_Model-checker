#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "generation.h"

int main(void){
    ParseInfos parseInfos;
    fill_parseInfos_struct(&parseInfos);

    generation(&parseInfos);
    
    printf("Proprietes du TA (Timed Automata) passees en entree : \n");
    printf("Nombre d'actions : %d\n", parseInfos.nb_actions);
    printf("Nombre de clocks : %d\n", parseInfos.nb_clocks);
    printf("Nombre de localites : %d\n", parseInfos.nb_locations);

    if(parseInfos.nb_actions != 0){
        printf("Tableau des actions : [");
        for(int i = 0; i < parseInfos.nb_actions; i++){
            if(i == (parseInfos.nb_actions - 1)) printf("%s]\n", parseInfos.actions[i]);
            else printf("%s, ", parseInfos.actions[i]);
        }
    }
    else printf("Tableau des actions : []\n");

    if(parseInfos.nb_clocks != 0){
        printf("Tableau des horloges : [");
        for(int i = 0; i < parseInfos.nb_clocks; i++){
            if(i == (parseInfos.nb_clocks - 1)) printf("%s]\n", parseInfos.names_clocks[i]);
            else printf("%s, ", parseInfos.names_clocks[i]);
        }
    }
    else printf("Tableau des horloges : []\n");

    printf("Tableau des localites : [");
    for(int i = 0; i < parseInfos.nb_locations; i++){
        if(i == (parseInfos.nb_locations - 1)) printf("%s]\n", parseInfos.locations[i]);
        else printf("%s, ", parseInfos.locations[i]);
    }

    printf("Tableau des invariants par localite : [");
    for(int i = 0; i < parseInfos.nb_locations; i++){
        printf("[");
        for(int j = 0; j < (parseInfos.nb_clocks+1); j++){
            printf("[");
            for(int k = 0; k < (parseInfos.nb_clocks+1); k++){
                if(k == parseInfos.nb_clocks){
                    if(j == parseInfos.nb_clocks){
                        if(i == (parseInfos.nb_locations-1)) printf("%d]]]\n", parseInfos.invariants[i][j][k]);
                        else printf("%d]], ", parseInfos.invariants[i][j][k]);
                    }
                    else printf("%d], ", parseInfos.invariants[i][j][k]);
                }
                else printf("%d, ", parseInfos.invariants[i][j][k]);
            }
        }
    }

    printf("Structure des transitions : \n");
    for(int i = 0; i < parseInfos.nb_locations; i++){
        if(parseInfos.nb_transitions_locations[i] != 0){
            printf("Transitions sortantes de la localite %s : \n[", parseInfos.locations[i]);
            for(int j = 0; j < parseInfos.nb_transitions_locations[i]; j++){
                printf("[");
                for(int k = 0; k < 4; k++){
                    switch(k){
                        case 0:
                            if(parseInfos.transitions[i][j]->label_action == -1) printf("null, ");
                            else printf("%s, ", parseInfos.actions[parseInfos.transitions[i][j]->label_action]);
                            break;
                        case 1:
                            printf("[");
                            for(int l = 0; l < (parseInfos.nb_clocks+1); l++){
                                printf("[");
                                for(int m = 0; m < (parseInfos.nb_clocks+1); m++){
                                    if(m == parseInfos.nb_clocks){
                                        if(l == parseInfos.nb_clocks) printf("%d]], ", parseInfos.transitions[i][j]->guard[l][m]);
                                        else printf("%d], ", parseInfos.transitions[i][j]->guard[l][m]);
                                    }
                                    else printf("%d, ", parseInfos.transitions[i][j]->guard[l][m]);
                                }
                            }
                            break;
                        case 2:
                            printf("[");
                            for(int l = 0; l < parseInfos.nb_clocks; l++){
                                if(l == (parseInfos.nb_clocks-1)) printf("%s: %d], ", parseInfos.names_clocks[l], parseInfos.transitions[i][j]->reset[l]);
                                else printf("%s: %d, ", parseInfos.names_clocks[l], parseInfos.transitions[i][j]->reset[l]);
                            }
                            break;
                        case 3:
                            if(j == (parseInfos.nb_transitions_locations[i]-1)) printf("%s]]\n", parseInfos.locations[parseInfos.transitions[i][j]->location_in]);
                            else printf("%s],\n", parseInfos.locations[parseInfos.transitions[i][j]->location_in]);
                            break;
                    }
                }
            }
        }
        else printf("Transitions sortantes de la localite %s : []\n", parseInfos.locations[i]);
    }
    return 0;
}