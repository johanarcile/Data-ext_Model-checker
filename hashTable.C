#include "hashTable.h"
#include <stdlib.h>
#include <stdio.h>

#define HASH_TABLE_SIZE 10007  // Nombre premier pour une meilleure distribution

typedef struct HashNode {
    State state;
    struct HashNode* next;  // Pour gérer les collisions (chaînage)
} HashNode;

typedef struct HashTable {
    HashNode** buckets;  // Tableau de pointeurs vers HashNode
    int size;
} HashTable;


// Fonction de hachage pour un état
unsigned int hash_state(State s) {
    unsigned int hash = 0;
    
    // Hacher la valeur v
    hash = s.var.v * 2654435761U;  // Multiplication par un nombre premier
    
    // Si vous avez d'autres champs dans State, les inclure aussi
    // Par exemple, si vous avez un tableau de locations :
    // for (int i = 0; i < num_locations; i++) {
    //     hash ^= s.locations[i] * 16777619U;
    // }
    
    // Si vous avez des horloges (clock values) :
    // for (int i = 0; i < num_clocks; i++) {
    //     hash ^= s.clocks[i] * 16777619U;
    // }
    
    return hash % HASH_TABLE_SIZE;
}

bool states_equal(State s1, State s2) {
    // Comparer la valeur v
    if (s1.var.v != s2.var.v) {
        return false;
    }
    
    // Ajouter d'autres comparaisons selon votre structure State
    // Par exemple :
    // if (s1.location != s2.location) return false;
    // 
    // for (int i = 0; i < num_clocks; i++) {
    //     if (s1.clocks[i] != s2.clocks[i]) return false;
    // }
    
    return true;
}

HashTable* hash_table_create() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (table == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour HashTable\n");
        exit(1);
    }
    
    table->size = HASH_TABLE_SIZE;
    table->buckets = (HashNode**)calloc(HASH_TABLE_SIZE, sizeof(HashNode*));
    
    if (table->buckets == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour buckets\n");
        free(table);
        exit(1);
    }
    
    return table;
}

bool hash_table_add(HashTable* table, State s) {
    unsigned int index = hash_state(s);
    
    // Vérifier si l'état existe déjà (éviter les doublons)
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (states_equal(current->state, s)) {
            return false;  // État déjà présent
        }
        current = current->next;
    }
    
    // Créer un nouveau nœud
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour HashNode\n");
        exit(1);
    }
    
    new_node->state = s;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    
    return true;  // Ajout réussi
}

bool hash_table_contains(HashTable* table, State s) {
    unsigned int index = hash_state(s);
    
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (states_equal(current->state, s)) {
            return true;  // État trouvé
        }
        current = current->next;
    }
    
    return false;  // État non trouvé
}
void hash_table_destroy(HashTable* table) {
    if (table == NULL) return;
    
    // Libérer chaque bucket
    for (int i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(table->buckets);
    free(table);
}