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

Document *sort_documents_by_relevance(Document *list, DocumentGraph *g) {
    Document *sorted = NULL;         // head of new, sorted list
    Document *current = list;        // node we’re inserting next

    while (current) {
        Document *next = current->next;    // save remainder of original list
        int curScore = graph_get_indegree(g, current->id);

        // If empty OR current’s score > head’s score, insert at head
        if (!sorted || graph_get_indegree(g, sorted->id) < curScore) {
            current->next = sorted;
            sorted = current;
        } else {
            // Otherwise walk until you find a node whose next has lower score
            Document *p = sorted;
            while (p->next && graph_get_indegree(g, p->next->id) >= curScore) {
                p = p->next;
            }
            // Insert current after p
            current->next = p->next;
            p->next = current;
        }

        current = next;  // move on to the next original node
    }

    return sorted;
}

double *compute_pagerank(DocumentGraph *g, int max_iters, double d, double tol){
    int n = g->numdocs;

    // 1) Allocate arrays
    double *rank   = malloc(sizeof(double) * n);
    double *newr   = malloc(sizeof(double) * n);
    int    *outdeg = malloc(sizeof(int)    * n);
    if (!rank || !newr || !outdeg) {
        // allocation failed
        free(rank); free(newr); free(outdeg);
        return NULL;
    }

    // 2) Compute out-degree for each node by scanning all incoming lists
    for (int i = 0; i < n; i++) {
        outdeg[i] = 0;
    }
    for (int dst = 0; dst < n; dst++) {
        Node *cur = g->incoming[dst];
        while (cur) {
            // edge from cur->doc_id → dst
            outdeg[cur->doc_id]++;
            cur = cur->next;
        }
    }

    // 3) Initialize all ranks to 1/n
    for (int i = 0; i < n; i++) {
        rank[i] = 1.0 / n;
    }

    // 4) Iterate PageRank
    for (int iter = 0; iter < max_iters; iter++) {
        double max_change = 0.0;

        // start each newr[i] at the “random jump” value
        for (int i = 0; i < n; i++) {
            newr[i] = (1.0 - d) / n;
        }

        // distribute rank over incoming edges
        for (int i = 0; i < n; i++) {
            Node *cur = g->incoming[i];
            while (cur) {
                int src = cur->doc_id;
                if (outdeg[src] > 0) {
                    newr[i] += d * (rank[src] / outdeg[src]);
                }
                cur = cur->next;
            }
        }

        // measure convergence and copy back
        for (int i = 0; i < n; i++) {
            double diff = newr[i] - rank[i];
            if (diff < 0) diff = -diff;
            if (diff > max_change) max_change = diff;
            rank[i] = newr[i];
        }

        // stop early if below tolerance
        if (max_change < tol) {
            break;
        }
    }

    // 5) Clean up and return
    free(newr);
    free(outdeg);
    return rank;
}

/**
 * Write out an array of relevance scores to a file.
 * Format:
 *   <num_docs>\n
 *   <score[0]>\n
 *   <score[1]>\n
 *   …\n
 */
void save_relevance_scores(const char *filename, double *scores, int num_docs){
    FILE *out = fopen(filename, "w");
    if (!out) return;
    // first line = count
    fprintf(out, "%d\n", num_docs);
    // one score per line
    for (int i = 0; i < num_docs; i++) {
        fprintf(out, "%lf\n", scores[i]);
    }
    fclose(out);
}

/**
 * Load relevance scores from disk.
 * Returns a malloc'd array of length *out_n, or NULL on failure.
 * Caller must free() the returned array.
 */
double *load_relevance_scores(const char *filename, int *out_n){
    FILE *in = fopen(filename, "r");
    if (!in) return NULL;

    int n;
    if (fscanf(in, "%d\n", &n) != 1) {
        fclose(in);
        return NULL;
    }
    double *scores = malloc(sizeof(double) * n);
    if (!scores) {
        fclose(in);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        if (fscanf(in, "%lf\n", &scores[i]) != 1) {
            free(scores);
            fclose(in);
            return NULL;
        }
    }
    fclose(in);
    *out_n = n;
    return scores;
}