#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataset.h"
#include "document.h"
#include "links.h"
#include "query.h"
#include "hashmap.h"
#include "graph.h"

// Configuration constants
#define TITLE_MAP_SIZE 100000
#define BODY_MAP_SIZE  200000
#define MAX_INPUT      256

int main() {
    // 1) Load all documents from the dataset folder
    Document *allDocs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (!allDocs) {
        fprintf(stderr, "Error: could not load documents.\n");
        return EXIT_FAILURE;
    }

    // 2) Build reverse-indexes for title and body
    HashMap *titleIdx = create_hashmap(TITLE_MAP_SIZE);
    HashMap *bodyIdx  = create_hashmap(BODY_MAP_SIZE);
    build_reverse_indexes(titleIdx, bodyIdx, allDocs);

    // 3) Build directed graph for link-based relevance
    int numDocs = 0;
    for (Document *d = allDocs; d; d = d->next) ++numDocs;
    DocumentGraph *g = graph_create(numDocs);
    for (Document *d = allDocs; d; d = d->next) {
        for (Link *l = d->links->next; l; l = l->next) {
            graph_add_edge(g, d->id, l->id);
        }
    }

    // 4) Initialize query history (stores last 3 queries)
    QueryHistory history;
    InitQueryHistory(&history);

    char input[MAX_INPUT];
    while (1) {
        // Display recent searches
        printf("******* Recent Searches *******\n");
        PrintQueryHistory(&history);
        printf("********************************\n");

        // Prompt user for query ("close" or empty line to exit)
        printf("Search (type 'close' to exit): ");
        if (!fgets(input, sizeof(input), stdin))
            break;
        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0' || strcmp(input, "close") == 0)
            break;

        // Parse and record the query
        Query *q = CreateQuery(input);
        AddQueryToHistory(&history, q);

        // Perform indexed search
        Document *candidates = search_documents_with_index(titleIdx, bodyIdx, q, allDocs);
        if (!candidates) {
            printf("No results found.\n\n");
            continue;
        }

        // Sort candidates by cached relevance
        Document *sorted = sort_documents_by_relevance(candidates, g);

        // Display top 5 results with snippet + relevance
        int count = 0;
        for (Document *d = sorted; d && count < 5; d = d->next, ++count) {
            printf("(%d) %s\n", count, d->title);
            print_snippet(d->body ? d->body : "", q);
            printf("Relevance: %d\n\n", graph_get_indegree(g, d->id));
        }
        int total = 0;
        for (Document *d = sorted; d; d = d->next) ++total;
        printf("[%d results]\n\n", total);

        // Allow user to view full document
        printf("Select document index (or blank to skip): ");
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = '\0';
            if (input[0] != '\0') {
                int idx = atoi(input);
                Document *sel = sorted;
                while (sel && idx-- > 0) sel = sel->next;
                if (sel) {
                    PrintDocument(sel);
                    printf("Relevance Score: %d\n\n", graph_get_indegree(g, sel->id));
                } else {
                    printf("Invalid index.\n\n");
                }
            }
        }

        // Free this round's candidate list
        FreeDocumentList(sorted);
    }

    // Cleanup all resources
    FreeQueryHistory(&history);
    graph_free(g);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    FreeDocumentList(allDocs);

    return EXIT_SUCCESS;
}
