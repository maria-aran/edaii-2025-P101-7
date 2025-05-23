#include "dataset.h"
#include "document.h"
#include "query.h"
#include "hashmap.h"
#include "graph.h"  

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Load all documents
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (docs == NULL) {
        printf("Error loading documents\n");
        return 1;
    }
    // Find the maximum document ID so we know how big to make the graph
    int max_id = -1;
    for (Document *d = docs; d != NULL; d = d->next) {
        if (d->id > max_id) {
            max_id = d->id;
        }
    }



    DocumentGraph *graph = graph_create(max_id + 1);
    if (graph == NULL) {
        printf("Error: could not create graph\n");
        FreeDocumentList(docs);
        return 1;
    }

    // Add an edge src→dst for every link in each document
    for (Document *d = docs; d != NULL; d = d->next) {
        for (Link *l = d->links->next; l != NULL; l = l->next) {
            graph_add_edge(graph, d->id, l->id);
        }
    }

    //Print each document with its relevance score (indegree)
    printf("Document relevance scores:\n");
    for (Document *d = docs; d != NULL; d = d->next) {
        printf("[%2d] %-40s  →  ", d->id, d->title ? d->title : "(no title)");
        print_relevance_score(graph, d->id);
    }

    // Clean up
    graph_free(graph);
    FreeDocumentList(docs);
    return 0;
}
