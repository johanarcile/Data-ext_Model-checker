#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include "structure_state_space_ta.h"  // Remplacer par votre fichier contenant State

#define HASH_TABLE_SIZE 10007

typedef struct HashNode {
    State state;
    struct HashNode* next;
} HashNode;

typedef struct HashTable {
    HashNode** buckets;
    int size;
} HashTable;

// Prototypes
HashTable* hash_table_create();
bool hash_table_add(HashTable* table, State s);
bool hash_table_contains(HashTable* table, State s);
void hash_table_destroy(HashTable* table);
unsigned int hash_state(State s);
bool states_equal(State s1, State s2);

#endif