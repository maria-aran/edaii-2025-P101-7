#include "dataset.h"
#include "document.h"
#include "graph.h"
#include "hashmap.h"
#include "links.h"
#include "query.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_MAP_SIZE 1024
#define MAX_LENGTH 500
#define BODY_MAP_SIZE 200000
#define MAX_QUERY_LEN 200
#define MAX_RESULTS 5
#define PR_ITERS 100
#define PR_DAMPING 0.85
#define PR_TOL 1e-6

int main() {

  // printf("Openning....\n\n");

  // printf("How big do you want the data set to be\n");
  // printf("Options: 12, 270, 540, 5400 (12 recommended): ");

  // int datasetinput;

  // //scanf("%d", &datasetinput);

  // const char *dataset;

  // if (datasetinput == 12) {
  //   dataset = "datasets/wikipedia12";

  // } else if (datasetinput == 270) {
  //   dataset = "datasets/wikipedia270";

  // } else if (datasetinput == 540) {
  //   dataset = "datasets/wikipedia540";
  // }

  // Load all docs into the list
  Document *docs = LoadDocumentsFromFolder("datasets/wikipedia5400");
  if (!docs) { // er
    fprintf(stderr, "No documents loaded.\n");
    return EXIT_FAILURE;
  }
  //printf("Dataset with %d documents loaded\n\n", datasetinput);
  {
    int _c;
    while ((_c = getchar()) != '\n' && _c != EOF);
  }

  // load reverse indexes
  HashMap *titleIdx = deserialize_reverse_index("title.idx", TITLE_MAP_SIZE);
  HashMap *bodyIdx = deserialize_reverse_index("body.idx", BODY_MAP_SIZE);
  if (!titleIdx || !bodyIdx) { // if either loading couldn't be done (missing
                               // file or corrupted)
    if (!titleIdx)
      titleIdx = create_hashmap(TITLE_MAP_SIZE); // create the hasham (for
                                                 // title)
    if (!bodyIdx)
      bodyIdx = create_hashmap(BODY_MAP_SIZE); // create the hashmap (for body)

    // Build reverse indexes from scratch by scanning all documents:
    // - titleIdx: for fast title-only lookups
    // - bodyIdx:  for full‐text lookups
  
    build_reverse_indexes(titleIdx, bodyIdx, docs);

    // Save the newly built indexes for faster startup next time
    serialize_reverse_index(titleIdx, "title.idx");
    serialize_reverse_index(bodyIdx, "body.idx");
  }

  // Build the document link graph
  //  Each document becomes a node, and every outbound link adds a directed
  //  edge.
  int numDocs = 0;
  for (Document *d = docs; d; d = d->next)
    ++numDocs; // first coumpute the number of docs

  // Now create the graph and populate it
  DocumentGraph *g =
      graph_create(numDocs); // create the graph with the number of docs
  for (Document *d = docs; d; d = d->next) {
    // Skip the dummy head in d->links; start from the first real link
    for (Link *l = d->links->next; l; l = l->next) {
      graph_add_edge(g, d->id, l->id);
      // add a directed edge from document d->id to the linked page l->id
    }
  }

  // Loading (or computing) Page scores!
  int pr_count;
  double *pageRanks = load_relevance_scores("pageranks", &pr_count); // pageranks should be replaced by actual docs

  // If loading failed or the saved count doesn’t match current docs, compute
  if (!pageRanks || pr_count != numDocs) {

    // clean up (just in case)
    free(pageRanks);

    // compute them and save them

    // Compute PageRank over the link graph with given parameters:
    // - PR_ITERS: maximum iterations
    // - PR_DAMPING: probability of following links vs. random jump
    // - PR_TOL: convergence tolerance

    pageRanks = compute_pagerank(g, PR_ITERS, PR_DAMPING, PR_TOL);
    save_relevance_scores("pageranks", pageRanks, numDocs);
  }

  // Initialize query history
  QueryHistory history;
  InitQueryHistory(&history);

  char input[MAX_QUERY_LEN + 1];

  // repeatedly prompt for a search, show results, and optionally display a
  // document’s full text.
  while (1) {

    // Show recent searches
    printf("******* recent searches ********\n");
    PrintQueryHistory(&history);
    printf("********************************\n\n");

    // Read user prompt
    printf("Search (\"exit\" to leave): ");

    // read the full line including spaces
    if (!fgets(input, sizeof(input), stdin)) {
      break; // EOF or error
    }
    // strip trailing newline
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "exit") == 0) { // if user enters exit --> stop
      break;
    }

    // Query:

    // create it
    Query *q = CreateQuery(input);

    // turn it into a string ( to prevent segmentation errors)
    char *qsnap = QueryToString(q);
    // enqueu new query
    AddQueryToHistory(&history, qsnap);
    free(qsnap);

    // Execute the search using reverse indexes, then sort by PageRank
    Document *hits = search_documents_with_index(titleIdx, bodyIdx, q, docs);
    // assign PageRank scores to each result
    for (Document *d = hits; d; d = d->next) {
      d->relevance = pageRanks[d->id];
    }

    // sort by that relevance field
    Document *ranked = sort_documents_by_relevance(hits);

    // Display top results
    printf("\n-- Top %d by PageRank --\n", MAX_RESULTS);

    int printed = 0;
    for (Document *d = ranked; d && printed < MAX_RESULTS;
         d = d->next, ++printed) {
      printf(" [%d] %s\n", printed, d->title ? d->title : "(no title)");
      print_snippet(d->body, q);
      printf("    PageRank: %.6f\n\n", pageRanks[d->id]);
    }

    printf(" [%d results]\n\n", printed);

    // Prompt for detail or new search
    printf("Choose document index for details (-1 to new search): \n");

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

    while (getchar() != '\n')
      ; // consume leftover newline

    if (idx < 0 || idx >= printed) {
      // New search
      FreeDocument(ranked);
      FreeQuery(q);
      continue;
    }

    // Valid index: show full document
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
  // Cleanup
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