#ifndef STRUCTURE_DBM_H
#define STRUCTURE_DBM_H

#include <limits.h>
#include <stdbool.h>

#define NB_CLOCKS 2
#define DBM_DIM (NB_CLOCKS + 1)
//#define DBM_SIZE (DBM_DIM * DBM_DIM * sizeof(int))
#define infty INT_MAX

typedef int DBM[DBM_DIM][DBM_DIM];
typedef int clocks[NB_CLOCKS];

bool is_empty(DBM zone);
void time_elapse_within_invariant(DBM zone, DBM invariant);
void successor_zone(DBM zone, DBM guard, clocks reset, DBM invariant);
void print_dbm(DBM zone);

#endif
