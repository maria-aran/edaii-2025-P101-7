#ifndef HASHMAP_H
#define HASHMAP_H

#include "document.h"
#include "query.h"

// linked‐list of document IDs
typedef struct DocLinked {
    int doc_id;
    struct DocLinked *next;
} DocLinked;

// one bucket in the hashtable
typedef struct HashNode {
    char *key;
    DocLinked *docs;
    struct HashNode *next;
} HashNode;

// the hashtable itself
typedef struct HashMap {
    HashNode **table;
    int size; // number of slots
} HashMap;

// create & free
HashMap*          create_hashmap(int size);
void              free_hashmap(HashMap *m);

// djb2 hash
unsigned int      hash(const char *key, int size);

// build & query
void              build_reverse_indexes(Document *docs, HashMap *titleIdx, HashMap *bodyIdx);
Document*         search_documents_with_index(HashMap *titleIdx, HashMap *bodyIdx,  Query *q, Document *allDocs);

// normalize in‐place
void normalize_word(char *w);

// snippet printer
void              print_snippet(const char *text, const char *kw);

// persistence
void              serialize_index(HashMap *m, const char *filename);
HashMap*          deserialize_index(const char *filename, int slots);

#endif
