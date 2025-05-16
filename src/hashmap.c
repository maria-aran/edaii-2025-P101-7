#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.h"


//create empty hashmap
HashMap* create_hashmap(int size){
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) return NULL;

    map->size=size; 
    map->table=(HashNode**)malloc(sizeof(HashNode*)*size);
    if(!map->table){
        free(map);
        return NULL;
    }

    for(int i=0; i<size; i++){
        map->table[i]=NULL;
    }
    return map;
}

//hash function (we will use -> djb2)

int hash(const char* key, int size){
        int hash=5281;
        int c;

        while(c=*key++)
            hash = ((hash << 5) + hash) + c; //* hash * 33 + c *
        return hash%size;
}
