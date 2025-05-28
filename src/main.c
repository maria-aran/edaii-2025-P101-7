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

int main() {
    // 1) Load all docs into the list
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");

    // 2) Init reverse index (hashmaps), try to load from disk first
    const char *title_idx_file = "title.idx";
    const char *body_idx_file  = "body.idx";

    HashMap *titleIdx = deserialize_reverse_index(title_idx_file, TITLE_MAP_SIZE);
    HashMap *bodyIdx  = deserialize_reverse_index(body_idx_file,  BODY_MAP_SIZE);

    if (!titleIdx || !bodyIdx) {
        // build missing maps
        if (!titleIdx) titleIdx = create_hashmap(TITLE_MAP_SIZE);
        if (!bodyIdx)  bodyIdx  = create_hashmap(BODY_MAP_SIZE);

        build_reverse_indexes(titleIdx, bodyIdx, docs);

        // save them for next run
        serialize_reverse_index(titleIdx, title_idx_file);
        serialize_reverse_index(bodyIdx,  body_idx_file);
    }

    // 3) Init the graph (edges from d->links)
    int numDocs = 0;
    for (Document *d = docs; d; d = d->next) ++numDocs;

    DocumentGraph *g = graph_create(numDocs);
    for (Document *d = docs; d; d = d->next) {
        for (Link *l = d->links->next; l; l = l->next) {
            graph_add_edge(g, d->id, l->id);
        }
    }

    // 4) Precompute or load PageRank scores
    const char *pr_file = "pageranks.dat";
    int    pr_count = 0;
    double *pageRanks = load_relevance_scores(pr_file, &pr_count);

    if (!pageRanks || pr_count != numDocs) {
        // compute & then save
        if (pageRanks) free(pageRanks);
        pageRanks = compute_pagerank(g, PR_ITERS, PR_DAMPING, PR_TOL);
        save_relevance_scores(pr_file, pageRanks, numDocs);
    }

    // 5) Main REPL
    char input[MAX_QUERY_LEN+1];
    while (1) {
        printf("Search: ");
        if (!fgets(input, sizeof input, stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0') break;

        Query    *q    = CreateQuery(input);
        Document *hits = search_documents_with_index(titleIdx, bodyIdx, q, docs);
        Document *ranked = sort_documents_by_relevance(hits, g);

        printf("\n-- Top %d by PageRank --\n", MAX_RESULTS);
        int printed = 0;
        for (Document *d = ranked; d && printed < MAX_RESULTS; d = d->next, ++printed) {
            printf(" [%d] %s\n", printed, d->title ? d->title : "(no title)");
            print_snippet(d->body, q);
            printf("    PageRank: %.6f\n\n", pageRanks[d->id]);
        }
        printf(" [%d results]\n\n", printed);

        printf("Choose document index for details (-1 to new search): ");
        int idx;
        if (scanf("%d", &idx) != 1 || idx < 0 || idx >= printed) {
            int c; 
            while ((c = getchar()) != '\n' && c != EOF);
            Document *cur = ranked;
            while (cur) {
                    Document *next = cur->next;
                    FreeDocument(cur);
                    cur = next;
                }
            FreeQuery(q);
        }
        while (getchar() != '\n');

        Document *cur = ranked;
        for (int i = 0; i < idx; i++) cur = cur->next;
        int selId = cur->id;

        Document *orig = docs;
        while (orig && orig->id != selId) orig = orig->next;
        if (orig) {
            PrintDocument(orig);
        } else {
            fprintf(stderr, "Error: could not find original document %d\n", selId);
        }

        FreeDocument(ranked);
        FreeQuery(q);
    }

    // tear down
    free(pageRanks);
    graph_free(g);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    FreeDocument(docs);
    return EXIT_SUCCESS;
}
