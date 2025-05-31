#ifndef GRAPH_H
#define GRAPH_H

#include "document.h"

// nodes for a list of document IDs
typedef struct Node {
  int doc_id;
  struct Node *next;
} Node;

// a directed graph over documents
typedef struct DocumentGraph {
  int numdocs;     // total number of documents
  Node **incoming; // array[num_docs] of incoming-edge lists
} DocumentGraph;

// Create an empty graph that can hold IDs in range
DocumentGraph *graph_create(int num_docs);

void graph_add_edge(DocumentGraph *g, int src, int dst);

// Return indegree of document
int graph_get_indegree(const DocumentGraph *g, int doc_id);

void print_relevance_score(const DocumentGraph *g, int dst);

// free the graph
void graph_free(DocumentGraph *g);

// Sorts a Document* list by descending indegree in graph g.
// Sort the list by the Document->relevance score (descending) using insertion
// sort.
Document *sort_documents_by_relevance(Document *list);

double *compute_pagerank(DocumentGraph *g, int max_iters, double d, double tol);

// Save/load precomputed relevance scores
void save_relevance_scores(const char *filename, double *scores, int num_docs);
double *load_relevance_scores(const char *filename, int *out_n);

#endif
