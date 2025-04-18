#include <stdio.h>


typedef struct{
    int id;                 
    struct Link* next;      
} Link;

typedef struct{
    int id;                 
    char* title;            
    char* body;             
    Link* links;            
} Document;