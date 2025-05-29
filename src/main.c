#include "dataset.h"
#include "document.h"
#include "graph.h"
#include "hashmap.h"
#include "links.h"
#include "query.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_MAP_SIZE 100000
#define BODY_MAP_SIZE 200000
#define MAX_QUERY_LEN 200
#define MAX_RESULTS 5
#define PR_ITERS 100
#define PR_DAMPING 0.85
#define PR_TOL 1e-6


int main() {
    // 1) Load all docs into the list
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (!docs) {
        fprintf(stderr, "No documents loaded.\n");
        return EXIT_FAILURE;
    }
    // 2) Load or build reverse indexes
    HashMap *titleIdx = deserialize_reverse_index("title.idx", TITLE_MAP_SIZE);
    HashMap *bodyIdx = deserialize_reverse_index("body.idx", BODY_MAP_SIZE);
    if (!titleIdx || !bodyIdx) {
        if (!titleIdx)
        titleIdx = create_hashmap(TITLE_MAP_SIZE);
        if (!bodyIdx)
        bodyIdx = create_hashmap(BODY_MAP_SIZE);
        build_reverse_indexes(titleIdx, bodyIdx, docs);
        serialize_reverse_index(titleIdx, "title.idx");
        serialize_reverse_index(bodyIdx, "body.idx");
    }

    // 3) Init the graph
    //    iterate through all docs and add edges
  // 3) Build document graph
        int numDocs = 0;
        for (Document *d = docs; d; d = d->next)
            ++numDocs;
        DocumentGraph *g = graph_create(numDocs);
        for (Document *d = docs; d; d = d->next) {
            for (Link *l = d->links->next; l; l = l->next) {
            graph_add_edge(g, d->id, l->id);
            }
        }

    int pr_count;
    double *pageRanks = load_relevance_scores("pageranks.dat", &pr_count);
    if (!pageRanks || pr_count != numDocs) {
        free(pageRanks);
        pageRanks = compute_pagerank(g, PR_ITERS, PR_DAMPING, PR_TOL);
        save_relevance_scores("pageranks.dat", pageRanks, numDocs);
    }
      // 5) Initialize query history
  QueryHistory history;
  InitQueryHistory(&history);

  // 6) Main REPL
  char input[MAX_QUERY_LEN + 1];
  while (1) {
    // a) Show recent searches
    printf("******* recent searches ********\n");
    PrintQueryHistory(&history);
    printf("********************************\n\n");

        // b) Read user query
    printf("Search: ");
    if (!fgets(input, sizeof input, stdin))
      break;
    input[strcspn(input, "\n")] = '\0';
    if (input[0] == '\0')
      break;
    // c) Parse & record
    Query *q = CreateQuery(input);
    char *qsnap = QueryToString(q);
    AddQueryToHistory(&history, qsnap);
    free(qsnap);
    // d) Search & rank
    Document *hits = search_documents_with_index(titleIdx, bodyIdx, q, docs);
    Document *ranked = sort_documents_by_relevance(hits, g);
     // e) Display top results
    printf("\n-- Top %d by PageRank --\n", MAX_RESULTS);
    int printed = 0;
    for (Document *d = ranked; d && printed < MAX_RESULTS;
         d = d->next, ++printed) {
      printf(" [%d] %s\n", printed, d->title ? d->title : "(no title)");
      print_snippet(d->body, q);
      printf("    PageRank: %.6f\n\n", pageRanks[d->id]);
    }
    printf(" [%d results]\n\n", printed);
    // f) Prompt for detail or new search
    printf("Choose document index for details (-1 to new search): ");
    int idx;
    if (scanf("%d", &idx) != 1) {
      // Bad input: clear stdin and start new search
      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      FreeDocument(ranked);
      FreeQuery(q);
      continue;
    }
    while (getchar() != '\n') ;  // consume leftover newline

    if (idx < 0 || idx >= printed) {
      // New search
      FreeDocument(ranked);
      FreeQuery(q);
      continue;
    }

    // g) Valid index: show full document
    Document *cur = ranked;
    for (int i = 0; i < idx; i++)
      cur = cur->next;
    int selId = cur->id;

    Document *orig = docs;
    while (orig && orig->id != selId)
      orig = orig->next;
    if (orig) {
      PrintDocument(orig);
    } else {
      fprintf(stderr, "Error: document %d not found\n", selId);
    }

    FreeDocument(ranked);
    FreeQuery(q);
  }
  // 7) Cleanup
  FreeQueryHistory(&history);
  graph_free(g);
  free(pageRanks);
  free_hashmap(titleIdx);
  free_hashmap(bodyIdx);

  // free original docs list
  for (Document *d = docs, *next; d; d = next) {
    next = d->next;
    FreeDocument(d);
  }

  return EXIT_SUCCESS;

}