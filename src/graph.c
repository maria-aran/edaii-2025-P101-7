#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

DocumentGraph *graph_create(int numdocs) {
    if (numdocs <= 0) {
        return NULL;
    }
    DocumentGraph *g = malloc(sizeof(DocumentGraph));
    if (g == NULL) {
        return NULL;
    }
    g->numdocs = numdocs;
    g->incoming = malloc(sizeof(Node*) * numdocs);
    if (g->incoming == NULL) {
        free(g);
        return NULL;
    }
    // initialize all slots to NULL
    for (int i = 0; i < numdocs; i = i + 1) {
        g->incoming[i] = NULL;
    }
    return g;
}

void graph_add_edge(DocumentGraph *g, int src, int dst) {
    // guard: graph must exist, dst in range
    if (g == NULL || dst < 0 || dst >= g->numdocs) {
        return;
    }
    // create a new node for src and prepend onto incoming[dst]
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        return;
    }
    n->doc_id = src;
    n->next = g->incoming[dst];
    g->incoming[dst] = n;
}

int graph_get_indegree(const DocumentGraph *g, int dst) {
    
    if (g == NULL || dst < 0 || dst >= g->numdocs) {
        return 0;
    }
    int count = 0;
    Node *cur = g->incoming[dst];
    // walk the linked list, counting entries
    while (cur != NULL) {
        count = count + 1;
        cur = cur->next;
    }
    return count;
}

void print_relevance_score(const DocumentGraph *g, int dst) {
    if (g == NULL) {
        return;
    }
    int indeg = graph_get_indegree(g, dst);
    printf("Relevance score: %d\n", indeg);
}

void graph_free(DocumentGraph *g) {
    if (g == NULL) {
        return;
    }
    // free each incoming list
    for (int i = 0; i < g->numdocs; i = i + 1) {
        Node *cur = g->incoming[i];
        while (cur != NULL) {
            Node *tmp = cur->next;
            free(cur);
            cur = tmp;
        }
    }
    free(g->incoming);
    free(g);
}
