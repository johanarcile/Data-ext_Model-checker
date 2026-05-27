#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "structure_DBM.h"

typedef int DBM[DBM_DIM][DBM_DIM];
typedef int clocks[NB_CLOCKS];

int add(int n1, int n2);
void canonize(DBM c);
void reset(DBM zone, int clock, int val);
void reset_0(DBM zone, int clock);
void reset_clocks(DBM zone,clocks values);
void inter(DBM zone1,DBM zone2);
void inter_nondiag(DBM zone1,DBM zone2);
void time_elapse(DBM zone);

int add(int n1, int n2){
    if(n1==infty || n2==infty){
        return infty;
    }
    else{
        return n1+n2;
    }
}

bool is_empty(DBM zone){
    if(zone[0][0]<0){
        return true;
    }
    return false;
}

void canonize(DBM zone){
    for(int k=0;k<DBM_DIM;k++){
        //if(is_empty(zone)){//opt //WARNING: useless if intersection guarantees detection of inconsistencies and sets zone[0][0] to a neg value
        //    return;
        //}
        for(int i=0;i<DBM_DIM;i++){
            for(int j=0;j<DBM_DIM;j++){
                int temp = add(zone[i][k],zone[k][j]);//opt
                if(zone[i][j] > temp){
                    zone[i][j] = temp;
                }
            }
        }
    }
}

void reset(DBM zone, int clock, int val){
    for(int i=0;i<DBM_DIM;i++){
        if(i!=clock){
            zone[clock][i]=add(val,zone[0][i]);
            zone[i][clock]=add(-val,zone[i][0]);
        }
    }
}

void reset_0(DBM zone, int clock){  //optimization of reset(zone,clock,0)
    for(int i=0;i<DBM_DIM;i++){
        if(i!=clock){
            zone[clock][i]=zone[0][i];
            zone[i][clock]=zone[i][0];
        }
    }
}

void reset_clocks(DBM zone, clocks values){
    for(int i=0;i<NB_CLOCKS;i++){
        if(values[i]!=infty){
            if(values[i]==0){
                reset_0(zone,i+1);
            }
            else{
                reset(zone,i+1,values[i]);
            }
        }
    }
}

void inter(DBM zone1,DBM zone2){
    bool modified = false;
    for(int i=0;i<DBM_DIM;i++){
        for(int j=0;j<DBM_DIM;j++){
            if(zone1[i][j] > zone2[i][j]){
                zone1[i][j] = zone2[i][j];
                if(!modified){
                    modified = true;
                }
                if(add(zone1[i][j],zone1[j][i])<0){//opt //if Dxy+Dyx<0 (implies that zone is empty), stop //WARNING not sure if worth the cost
                    zone1[0][0] = -1;
                    return;
                }
            }
        }
    }
    if(modified){//opt //canonization only necessary if zone1 was modified
        canonize(zone1);
    }
}

void inter_nondiag(DBM zone1,DBM zone2){    //optimization of inter(zone1, zone2) when zone2 constraints are non-diagonal
    bool modified = false;
    for(int i=1;i<DBM_DIM;i++){
        if(zone1[0][i] > zone2[0][i]){
            zone1[0][i] = zone2[0][i];
            if(!modified){
                modified = true;
            }
            if(add(zone1[0][i],zone1[i][0])<0){//opt //if Dxy+Dyx<0 (implies that zone is empty), stop //WARNING not sure if worth the cost
                zone1[0][0] = -1;
                return;
            }
        }
        if(zone1[i][0] > zone2[i][0]){
            zone1[i][0] = zone2[i][0];
            if(!modified){
                modified = true;
            }
            if(add(zone1[0][i],zone1[i][0])<0){//opt //if Dxy+Dyx<0 (implies that zone is empty), stop //WARNING not sure if worth the cost
                zone1[0][0] = -1;
                return;
            }
        }
    }
    if(modified){//opt //canonization only necessary if zone1 was modified
        canonize(zone1);
    }
}

//TODO OPT : inter with only one constraint (inter in O(1), canonization in O(2n^2)), inter with 0 constraint (skip)

void time_elapse(DBM zone){
    for(int i=1;i<DBM_DIM;i++){
        zone[i][0] = infty;
    }    
}

void time_elapse_within_invariant(DBM zone, DBM invariant){ //optimization of {time_elapse(zone); inter_nondiag(zone,invariant);} //WARNING: NOT PROVEN
    int min = infty;
    for(int i=1;i<DBM_DIM;i++){
        int temp = add(invariant[i][0],zone[0][i]);
        if(min > temp){
            min = temp;
        }
    }
    for(int i=1;i<DBM_DIM;i++){
        int temp = add(zone[i][0],min);
        if(invariant[i][0] > temp){
            zone[i][0] = temp;
        }
        else{
            zone[i][0] = invariant[i][0];
        }
    }
}

void successor_zone(DBM zone, DBM guard, clocks reset, DBM invariant){
    inter_nondiag(zone,guard); //intesection with guard
    if(is_empty(zone)){//opt //if empty, stop
        return;
    }
    reset_clocks(zone,reset); //reset of clocks
    inter_nondiag(zone,invariant); //intersection with invariant
    if(is_empty(zone)){//opt //if empty, stop
        return;
    }
    time_elapse_within_invariant(zone,invariant); //time elapsing respecting invariant //WARNING: NOT PROVEN
    //time_elapse(zone); //time elapsing
    //inter_nondiag(zone,invariant); //intersection with invariant
}

void print_dbm(DBM zone){
    for (int i = 0; i < DBM_DIM; i++){
        printf("\t");
        for (int j = 0; j < DBM_DIM; j++){
            if(zone[i][j]==infty){
                printf("| inf |\t");
            }
            else{
                printf("| %d |\t", zone[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}
