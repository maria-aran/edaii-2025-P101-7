#ifndef HASHMAP_H
#define HASHMAP_H
#include "document.h"  // for the document struct definition
#include "query.h"

//Structures:

// Linked list of document IDs
typedef struct DocLinked{
    int doc_id;
    struct DocLinked* next;
} DocLinked;

// Hashmap node: key + list of document IDs + next for chaining
typedef struct HashNode{
    char* key;
    DocLinked* docs;
    struct HashNode* next;
} HashNode;

// Hashmap: array of tables
typedef struct HashMap{
    HashNode** table;
    int size; //slots in table
}HashMap;

//Functions

// Create and free
HashMap* create_hashmap(int size);
void free_hashmap(HashMap *map);

//Hash function
int hash(const char* key, int size);

// Core operations
void put(HashMap *map, const char *key, int doc_id);
DocLinked *get(HashMap *map, const char *key);
int contains(HashMap *map, const char *key);
 
// Build reverse index over all documents
void build_reverse_index(HashMap *index, Document *docs);

// Search documents by Query using reverse index
Document *search_documents_with_index(HashMap *map, Query *query, Document *all_docs);

#endif