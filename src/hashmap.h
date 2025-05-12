// #ifndef HASHMAP_H
// #define HASHMAP_H

// typedef struct HashNode{
//     char* key;
//     DocLinked* docs;
//     struct HashNode* next;
// } HashNode;

// typedef struct DocLinked{
//     int doc_id;
//     struct DocLinked* next;
// } DocLinked;

typedef struct HashMap{
    HashNode** table;
    int size; //slots in table
}HashMap;


HashMap* create_hashmap(int size);
unsigned int hash(const char* key, int size);

#endif
