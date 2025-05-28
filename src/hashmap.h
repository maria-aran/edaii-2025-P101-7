#ifndef HASHMAP_H
#define HASHMAP_H

#include "query.h"
#include "document.h"

// Linked list of document IDs
typedef struct DocLinked {
    int doc_id;
    struct DocLinked *next;
} DocLinked;

// One bucket in the hashmap
typedef struct HashNode {
    char *key;             // the normalized word
    DocLinked *docs;       // list of doc IDs
    struct HashNode *next; // next node in this slot (separate chaining)
} HashNode;

// The hashmap itself
typedef struct HashMap {
    HashNode **table; // array of slots
    int size;         // number of slots
} HashMap;

// Create an empty hashmap with given slot count
HashMap *create_hashmap(int size);

// Free the entire hashmap and all its nodes
void free_hashmap(HashMap *map);

// Build both a title-only index and a body-only index
void build_reverse_indexes(HashMap *titleIdx, HashMap *bodyIdx, Document *allDocs);

// Search for documents matching ALL include-keywords and NO exclude-keywords
Document *search_documents_with_index(HashMap *titleIdx, HashMap *bodyIdx, Query *q, Document *allDocs);

// Print snippet
void print_snippet(const char *body, Query *q);

//insert and get
void put_hashmap(HashMap *map, const char *key, int docId);
DocLinked *get_list(HashMap *map, const char *key);

void serialize_reverse_index(HashMap *map, const char *filename);
HashMap *deserialize_reverse_index(const char *filename, int size);

//free
void free_hashmap(HashMap *map);

//helper:
void normalize_word(const char *in, char *out);

#endif
