
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

int main(void) {
    // 1) Load all docs into the list
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");

    // 2) Init reverse index (hashmaps)
    HashMap *titleIdx = create_hashmap(TITLE_MAP_SIZE);
    HashMap *bodyIdx  = create_hashmap(BODY_MAP_SIZE);
    build_reverse_indexes(titleIdx, bodyIdx, docs);

    // 3) Init the graph
    //    iterate through all docs and add edges
    int numDocs = 0;
    for (Document *d = docs; d; d = d->next) ++numDocs;
    DocumentGraph *g = graph_create(numDocs);
    for (Document *d = docs; d; d = d->next) {
        for (Link *l = d->links->next; l; l = l->next)
            graph_add_edge(g, d->id, l->id);
    }
    // precompute PageRank scores once
    double *pageRanks = compute_pagerank(g, PR_ITERS, PR_DAMPING, PR_TOL);

    char input[MAX_QUERY_LEN+1];
    while (1) {
        // 4.1) Ask query
        printf("Search: ");
        if (!fgets(input, sizeof input, stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (input[0]=='\0')      break;

        // 4.2) Search documents using hashmap
        Query *q = CreateQuery(input);
        Document *hits = search_documents_with_index(titleIdx, bodyIdx, q, docs);

        // 4.3) Order documents by PageRank score (highest first)
        Document *ranked = sort_documents_by_relevance(hits, g);

        printf("\n-- Top %d by PageRank --\n", MAX_RESULTS);
        int printed = 0;
        for (Document *d = ranked; d && printed < MAX_RESULTS; d = d->next, ++printed) {
            // 1) Title
            printf(" [%d] %s\n", printed, d->title ? d->title : "(no title)");
            // 2) Snippet
            print_snippet(d->body, q);
            // 3) PageRank score
            printf("    PageRank: %.6f\n\n", pageRanks[d->id]);
        }
        printf(" [%d results]\n\n", printed);

        // 4.5) Ask user for an index (0..printed-1)
        printf("Choose document index for details (-1 to new search): ");
        int idx;
        if (scanf("%d", &idx)!=1 || idx<0 || idx>=printed) {
            // flush newline and continue
            int c; while ((c=getchar())!='\n' && c!=EOF);
            FreeDocumentList(ranked);
            FreeQuery(q);
            continue;
        }
        // flush leftover newline
        while (getchar()!='\n');

        // 4.6) Print all info about the selected document
        Document *cur = ranked;
        for (int i = 0; i < idx; i++) cur = cur->next;
        PrintDocument(cur);

        // cleanup
        FreeDocumentList(ranked);
        FreeQuery(q);
    }

    // tear down
    free(pageRanks);
    graph_free(g);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    FreeDocumentList(docs);
    return EXIT_SUCCESS;
}
