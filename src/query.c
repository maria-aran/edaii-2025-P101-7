#include "query.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

Query* CreateQuery(const char* str) {
    Query* query = malloc(sizeof(Query));
    query->head = NULL;

    char* copy = strdup(str);
    char* token = strtok(copy, " ");

    while (token) {
        QueryNode* node = malloc(sizeof(QueryNode));
        node->is_excluded = (token[0] == '-');
        node->keyword = strdup(node->is_excluded ? token + 1 : token);
        node->next = query->head;
        query->head = node;
        token = strtok(NULL, " ");
    }

    free(copy);
    return query;
}

void FreeQuery(Query* query) {
    QueryNode* current = query->head;
    while (current) {
        QueryNode* next = current->next;
        free(current->keyword);
        free(current);
        current = next;
    }
    free(query);
}

QueryQueue* CreateQueryQueue(int capacity) {
    QueryQueue* queue = malloc(sizeof(QueryQueue));
    queue->queries = malloc(sizeof(Query*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = -1;
    return queue;
}

void EnqueueQuery(QueryQueue* queue, Query* query) {
    if (queue->size == queue->capacity) {
        queue->front = (queue->front + 1) % queue->capacity;
        queue->size--;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->queries[queue->rear] = query;
    queue->size++;
}

void PrintQueryQueue(QueryQueue* queue) {
    printf("Últimas %d consultas:\n", queue->size);
    for (int i = 0; i < queue->size; i++) {
        int index = (queue->front + i) % queue->capacity;
        printf("- ");
        Query* q = queue->queries[index];
        QueryNode* curr = q->head;
        while (curr) {
            if (curr->is_excluded)
                printf("-%s ", curr->keyword);
            else
                printf("%s ", curr->keyword);
            curr = curr->next;
        }
        printf("\n");
    }
}

void FreeQueryQueue(QueryQueue* queue) {
    free(queue->queries);
    free(queue);
}

