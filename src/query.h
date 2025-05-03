#ifndef QUERY_H
#define QUERY_H

typedef struct QueryNode {
    char* keyword;
    int is_excluded; // 1 si está excluido con '-', 0 si no
    struct QueryNode* next;
} QueryNode;

typedef struct Query {
    QueryNode* head;
} Query;

Query* CreateQuery(const char* str);
void FreeQuery(Query* query);

// ------------------------------
// Cola de las últimas queries
// ------------------------------

typedef struct QueryQueue {
    Query** queries;
    int capacity;
    int size;
    int front;
    int rear;
} QueryQueue;

QueryQueue* CreateQueryQueue(int capacity);
void EnqueueQuery(QueryQueue* queue, Query* query);
void PrintQueryQueue(QueryQueue* queue);
void FreeQueryQueue(QueryQueue* queue);

#endif
