#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataset.h"
#include "document.h"
#include "links.h"
#include "query.h"
#include "hashmap.h"
#include "graph.h"

// adjust these to your needs
#define TITLE_MAP_SIZE 100000
#define BODY_MAP_SIZE  200000
#define MAX_INPUT      256

int main() {
    // 1) Load documents
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (!docs) {
        fprintf(stderr, "Error loading documents\n");
        return 1;
    }

    // 2) Build reverse indexes
    HashMap *titleIdx = create_hashmap(TITLE_MAP_SIZE);
    HashMap *bodyIdx  = create_hashmap(BODY_MAP_SIZE);
    build_reverse_indexes(titleIdx, bodyIdx, docs);

    // 3) Build the directed graph
    //    assume IDs go from 0..N-1 with no gaps
    int numDocs = 0;
    for (Document *d = docs; d; d = d->next) numDocs++;
    DocumentGraph *g = graph_create(numDocs);
    for (Document *d = docs; d; d = d->next) {
        for (Link *l = d->links->next; l; l = l->next) {
            graph_add_edge(g, d->id, l->id);
        }
    }

    // 4) Prepare query history
    QueryHistory history;
    InitQueryHistory(&history);

    char input[MAX_INPUT];
    while (1) {
        // 4a) Show recent searches
        printf("******* recent searches ********\n");
        PrintQueryHistory(&history);
        printf("********************************\n");

        // 4b) Read a new search (empty = exit)
        printf("Search: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0') break;

        // 4c) Build & record the query
        Query *q = CreateQuery(input);
        AddQueryToHistory(&history, q);

        // 4d) Indexed search
        Document *raw = search_documents_with_index(titleIdx, bodyIdx, q, docs);
        if (!raw) {
            printf("No results found\n\n");
            continue;
        }

        // 4e) Sort by relevance = indegree
        Document *sorted = NULL;
        for (Document *d = raw; d; ) {
            Document *next = d->next;
            int score = graph_get_indegree(g, d->id);

            // insertion into sorted list by descending score
            if (!sorted || score > graph_get_indegree(g, sorted->id)) {
                d->next   = sorted;
                sorted    = d;
            } else {
                Document *p = sorted;
                while (p->next &&
                       graph_get_indegree(g, p->next->id) >= score)
                    p = p->next;
                d->next   = p->next;
                p->next   = d;
            }

            d = next;
        }

        // 4f) Print top 5 with snippet + relevance
        int count = 0;
        for (Document *d = sorted; d && count < 5; d = d->next, count++) {
            const char *body = d->body ? d->body : "";
            char snippet[151];
            int i = 0;
            while (i < 150 && body[i]) {
                snippet[i] = body[i++];
            }
            snippet[i] = '\0';

            int rel = graph_get_indegree(g, d->id);
            printf(" (%d) %s--\n", count, d->title);
            printf("%s--\n", snippet);
            printf("relevance score: %d\n\n", rel);
        }
        // total count
        int total = 0;
        for (Document *d = sorted; d; d = d->next) total++;
        printf(" [%d results]\n\n", total);

        // 4g) Let user select one to view full detail
        printf("Select document: ");
        if (fgets(input, sizeof(input), stdin)) {
            int idx = atoi(input);
            Document *d = sorted;
            for (int i = 0; i < idx && d; i++) d = d->next;
            if (d) {
                // full display
                PrintDocument(d);
                printf("RELEVANCE SCORE: %d\n\n",
                       graph_get_indegree(g, d->id));
            } else {
                printf("Invalid index\n\n");
            }
        }

        // 4h) Cleanup this round
        FreeDocumentList(sorted);
    }

    // 5) Final cleanup
    FreeQueryHistory(&history);
    graph_free(g);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    FreeDocumentList(docs);
    return 0;
}
