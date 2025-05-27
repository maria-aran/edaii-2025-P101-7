// main.c
#include "document.h"
#include "dataset.h"
#include "graph.h"
#include "hashmap.h"
#include "query.h"
#include "links.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_MAP_SIZE 100000
#define BODY_MAP_SIZE  200000
#define MAX_QUERY_LEN  200
#define MAX_RESULTS    5
#define PR_ITERS       100
#define PR_DAMPING     0.85
#define PR_TOL         1e-6
#define HISTORY_SIZE   3

int main(void) {
    // Load and index documents
    Document *docs     = LoadDocumentsFromFolder("datasets/wikipedia12");
    HashMap  *titleIdx = create_hashmap(TITLE_MAP_SIZE);
    HashMap  *bodyIdx  = create_hashmap(BODY_MAP_SIZE);
    build_reverse_indexes(titleIdx, bodyIdx, docs);

    // Build graph and compute PageRank
    int             numDocs = 0;
    for (Document *d = docs; d; d = d->next) ++numDocs;
    DocumentGraph *g        = graph_create(numDocs);
    for (Document *d = docs; d; d = d->next) {
        for (Link *l = d->links->next; l; l = l->next) {
            graph_add_edge(g, d->id, l->id);
        }
    }
    double *pageRanks = compute_pagerank(g, PR_ITERS, PR_DAMPING, PR_TOL);

    // History of search strings
    char *history[HISTORY_SIZE] = { NULL };
    int   h_start = 0, h_count = 0;

    char input[MAX_QUERY_LEN + 1];

    // Interactive search loop
    while (1) {
        // Print recent searches
        printf("******* recent searches ********\n");
        if (h_count == 0) {
            printf("(none)\n");
        } else {
            for (int i = 0; i < h_count; ++i) {
                int idx = (h_start + i) % HISTORY_SIZE;
                printf(" * %s *\n", history[idx]);
            }
        }
        printf("********************************\n");

        // Prompt
        printf("Search: ");
        if (!fgets(input, sizeof(input), stdin))
            break;
        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0')
            break;

        // Add string to history
        if (h_count == HISTORY_SIZE) {
            free(history[h_start]);
            history[h_start] = strdup(input);
            h_start = (h_start + 1) % HISTORY_SIZE;
        } else {
            history[(h_start + h_count) % HISTORY_SIZE] = strdup(input);
            ++h_count;
        }

        // Create and free print
        Query *q = CreateQuery(input);

        // Indexed lookup
        Document *cands = search_documents_with_index(titleIdx, bodyIdx, q, docs);

        // Sort by PageRank
        Document *ranked = NULL;
        for (Document *d = cands; d; ) {
            Document *next = d->next;
            if (!ranked || pageRanks[d->id] > pageRanks[ranked->id]) {
                d->next = ranked;
                ranked  = d;
            } else {
                Document *p = ranked;
                while (p->next && pageRanks[p->next->id] >= pageRanks[d->id]) {
                    p = p->next;
                }
                d->next = p->next;
                p->next = d;
            }
            d = next;
        }

        // Print top results
        printf("\n-- Top %d by PageRank --\n", MAX_RESULTS);
        int count = 0;
        for (Document *d = ranked; d && count < MAX_RESULTS; d = d->next, ++count) {
            printf(" (%d) %s--\n", count, d->title ? d->title : "(no title)");
            print_snippet(d->body, q);
            printf("---PageRank: %.6f--\n\n", pageRanks[d->id]);
        }
        printf(" [%d results]\n\n", count);

        // Free lists and query
        FreeDocumentList(ranked);
        FreeQuery(q);

    }

    // Cleanup
    for (int i = 0; i < h_count; ++i) {
        free(history[(h_start + i) % HISTORY_SIZE]);
    }
    graph_free(g);
    free(pageRanks);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    FreeDocumentList(docs);

    return EXIT_SUCCESS;
}
