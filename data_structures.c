#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "structure_state_space_ta.h"
#include "uthash.h"
#include "nested_queries.h"



/*----------------------Pour les structures des données ----------------*/


/* ================================================================== */
/*  Visited hash table (table pour enregistrer les états visités)     */
/* ================================================================== */



void visit_add(visit** table, State s) {
    
        visit* e = NULL;
      
        HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
        if (e == NULL) {
        e = malloc(sizeof(visit));
        e->key    = s.var;
        //e->explored  = true;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);}
 
}

visit* visit_find(visit** table, State s) {
    visit* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

void visit_destroy(visit** table) {
    visit *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}

/* ------------------------------------------------------------------ */
/*  Structure stateweight pour table visiting (état + weight)                          */
/* ------------------------------------------------------------------ */

void sw_add(StateWeight** table, State s, int w) {
   
    StateWeight* e = NULL;
   // HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
   // if (e == NULL) {
        e = malloc(sizeof(StateWeight));
        e->key    = s.var;
        e->state  = s;
        e->weight = w;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);
   // }
}

StateWeight* sw_find(StateWeight** table, State s) {
    StateWeight* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

void sw_destroy(StateWeight** table) {
    StateWeight *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}


/* ================================================================================= */
/*  Min-Heap (priority queue)   pour sauvegarder les états ordonés selon le weight   */
/* ================================================================================ */



 MinHeap* heap_create(int capacity) {
    MinHeap* h = malloc(sizeof(MinHeap));
    h->data     = malloc(sizeof(HeapNode) * capacity);
    h->size     = 0;
    h->capacity = capacity;
    return h;
}

 void heap_destroy(MinHeap* h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

 void heap_swap(MinHeap* h, int i, int j) {
    HeapNode tmp  = h->data[i];
    h->data[i]    = h->data[j];
    h->data[j]    = tmp;
}

 void heap_sift_up(MinHeap* h, int i) { // pour insérer nouveau élémnt dans sa place
    while (i > 0) { // jusqu'au premier élément
        int parent = (i - 1) / 2; // rend la partie décimal parent de 4 =>3/2=1
        if (h->data[parent].weight <= h->data[i].weight) break; // c ordonné
        heap_swap(h, parent, i);// swap si pas pas ordonné
        i = parent; // passer à l'élément au dessus
    }
}

void heap_sift_down(MinHeap* h, int i) { // pour réordonner après pop du premier élément 
    while (1) {
        int smallest = i;
        int left     = 2 * i + 1;
        int right    = 2 * i + 2;

        if (left  < h->size && h->data[left].weight  < h->data[smallest].weight) // si le fils gauche existe et est plus petit que la parent 
            smallest = left;
        if (right < h->size && h->data[right].weight < h->data[smallest].weight)
            smallest = right;

        if (smallest == i) break; // i est donc plus petit que ses deux fils donc c cordonné
        heap_swap(h, i, smallest);
        i = smallest; // continuer avec l'element prochain
    }
}

void heap_push(MinHeap* h, State s, int w) {
    /* Grow if needed */
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->data = realloc(h->data, sizeof(HeapNode) * h->capacity);
    }
    h->data[h->size].state  = s;
    h->data[h->size].weight = w;
    heap_sift_up(h, h->size); // on le place comme dernier élément et on monte
    h->size++;
}

 HeapNode heap_pop(MinHeap* h) {
    HeapNode best  = h->data[0];// on retourne le premier élément

    h->data[0] = h->data[h->size - 1];// on met le dernier element comme premier 
    h->size--;
    if (h->size > 0)
        heap_sift_down(h, 0);//réordonner
    return best;
}

/* ================================================================================= */
/*  Min-Heap avec pointeur vers le state                                            */
/* =============================================================================== */


MinHeapP* heap_createP(int capacity) {
    MinHeapP* h = malloc(sizeof(MinHeapP));
    h->data     = malloc(sizeof(HeapNodeP) * capacity);
    h->size     = 0;
    h->capacity = capacity;
    return h;
}

void heap_destroyP(MinHeapP* h) {
    for (int i = 0; i < h->size; i++) {
        free(h->data[i].state);    // libère chaque State pointé
    }
    free(h->data);
    free(h);
}

void heap_swapP(MinHeapP* h, int i, int j) {
    HeapNodeP tmp = h->data[i];    //  FIX #2 : était HeapNode, doit être HeapNodeP
    h->data[i]    = h->data[j];
    h->data[j]    = tmp;
}

void heap_sift_upP(MinHeapP* h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].weight <= h->data[i].weight) break;
        heap_swapP(h, parent, i);
        i = parent;
    }
}

void heap_sift_downP(MinHeapP* h, int i) {
    while (1) {
        int smallest = i;
        int left     = 2 * i + 1;
        int right    = 2 * i + 2;

        if (left  < h->size && h->data[left].weight  < h->data[smallest].weight)
            smallest = left;
        if (right < h->size && h->data[right].weight < h->data[smallest].weight)
            smallest = right;

        if (smallest == i) break;
        heap_swapP(h, i, smallest);
        i = smallest;
    }
}

void heap_pushP(MinHeapP* h, State* s, int w) {
     if (s == NULL) {
        printf(" heap_pushP : state NULL poussé !\n");
        return;
    }
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->data = realloc(h->data, sizeof(HeapNodeP) * h->capacity); 
    }
    h->data[h->size].state  = s;   
    h->data[h->size].weight = w;
    h->size++;
    heap_sift_upP(h, h->size-1);
    
}

HeapNodeP heap_popP(MinHeapP* h) { 
    HeapNodeP best = h->data[0];    
    h->data[0] = h->data[h->size - 1];
    h->size--;
    if (h->size > 0)
        heap_sift_downP(h, 0);
    
    return best;
}



/* ================================================================================= */
/*  VisitState for memory in layers                                         */
/* =============================================================================== */

void visitState_add(StateHash** table, State s) {
    StateHash* entry = malloc(sizeof(StateHash));
     StateKey key = {s};
    entry->key = key;
    entry ->index = HASH_COUNT(*table) + 1; // soit ça soit idex ++ dans les parametres de la fonction

    HASH_ADD_KEYPTR(hh, *table, &entry->key, sizeof(State), entry);
}

StateHash* visitState_find(StateHash** table, State s) {
    StateHash* entry = NULL;
    HASH_FIND(hh, *table, &s, sizeof(State), entry);
    return entry;
}

void visitState_destroy(StateHash** table) {
    StateHash *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}



 void mark_add(mark** table, State s, int w) {
    mark* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    if (e == NULL) {
        e = malloc(sizeof(mark));
        e->key   = s.var;
       // e->state = s;
        e->mark  = w;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);
    }
}

 mark* mark_find(mark** table, State s) {
    mark* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

 void mark_destroy(mark** table) {
    mark *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        free(cur);
    }
}



/* ------------------------------------------------------------------ */
/*  Structure unique : frontière avec booleen explored                   */
/* ------------------------------------------------------------------ */


 void swe_add(StateWeightExp** table, State s, int w) {
    StateWeightExp* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    if (e == NULL) {
        e = malloc(sizeof(StateWeightExp));
        e->key      = s.var;
        e->state    = s;
        e->weight   = w;
        e->explored = false;
        HASH_ADD(hh, *table, key, sizeof(Variable), e);
       

    } 
}

StateWeightExp* swe_find(StateWeightExp** table, State s) {
  StateWeightExp* e = NULL;
    HASH_FIND(hh, *table, &s.var, sizeof(Variable), e);
    return e;
}

 void swe_destroy(StateWeightExp** table) {
   StateWeightExp *cur, *tmp;
    HASH_ITER(hh, *table, cur, tmp) {  //Pourquoi tmp ? Parce que HASH_DEL modifie les pointeurs internes de cur. Sans tmp, on perdrait le lien vers le reste de la liste.
        HASH_DEL(*table, cur);
        free(cur);
    }
}