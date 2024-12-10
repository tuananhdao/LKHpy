#ifndef _HASHING_H
#define _HASHING_H

/*
 * This header specifies the interface for hashing.   
 */

#include "GainType.h"

#define HashTableSize 65521     /* Largest prime less than USHRT_MAX */
#define MaxLoadFactor 0.75

typedef struct HashTableEntry {
    unsigned Hash;
    GainType Cost;
} HashTableEntry;

typedef struct HashTable {
    HashTableEntry Entry[HashTableSize];
    int Count; /* Number of occupied entries */
} HashTable;

void HashInitialize(HashTable * T);

void HashInsert(HashTable * T, unsigned Hash, GainType Cost);

int HashSearch(HashTable * T, unsigned Hash, GainType Cost);

typedef struct MinNodeHashTableEntry {
    int Id;
    int PrevId;
    GainType PrevCostSum;
    struct MinNodeHashTableEntry *next; // Pointer to handle collisions
} MinNodeHashTableEntry;

typedef struct MinNodeHashTable {
    MinNodeHashTableEntry *Entry[HashTableSize];
    int Count; /* Number of occupied entries */
} MinNodeHashTable;

void MinNodeHashInitialize(MinNodeHashTable * T);

void MinNodeHashInsert(MinNodeHashTable * T, int Id, int PrevId, GainType PrevCostSum);

GainType MinNodeHashSearch(MinNodeHashTable *T, int Id, int PrevId);

#endif
