#include "dataset.h"
#include "document.h"
#include "query.h"
#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDEX_SIZE 10007

int main(void) {
    // 1) load documents
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (!docs) {
        fprintf(stderr, "Error: could not load documents\n");
        return 1;
    }

    // 2) build two reverse‐indexes: one for titles, one for bodies
    HashMap *titleIdx = create_hashmap(INDEX_SIZE);
    HashMap *bodyIdx  = create_hashmap(INDEX_SIZE);
    if (!titleIdx || !bodyIdx) {
        fprintf(stderr, "Error: could not allocate hashmap\n");
        FreeDocumentList(docs);
        return 1;
    }

    // index titles
    for (Document *d = docs; d; d = d->next) {
        index_text(titleIdx, d->title ? d->title : "", d->id);
    }
    // index bodies
    for (Document *d = docs; d; d = d->next) {
        index_text(bodyIdx,  d->body  ? d->body  : "", d->id);
    }

    // 3) init query history
    QueryHistory history;
    InitQueryHistory(&history);

    char line[200];
    while (1) {
        // show docs
        printf("\n------- Documents -------\n");
        PrintDocumentList(docs);

        // show last 3 queries
        printf("\n------- Recent Searches -------\n");
        PrintQueryHistory(&history);

        // prompt
        printf("\nEnter query (or 'exit' to quit, or number to view): ");
        if (!fgets(line, sizeof(line), stdin))
            break;
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "exit") == 0)
            break;
        if (line[0] == '\0') {
            printf("Please enter something or 'exit'.\n");
            continue;
        }

        // if number, show document
        int idx;
        if (sscanf(line, "%d", &idx) == 1) {
            Document *cur = docs;
            for (int i = 0; cur && i < idx; i++)
                cur = cur->next;
            if (cur) {
                printf("\n------- Selected Document -------\n");
                PrintDocument(cur);
            } else {
                printf("Invalid document index.\n");
            }
            continue;
        }

        // otherwise it's a keyword query
        Query *q = CreateQuery(line);
        if (!q) {
            printf("Error: invalid query.\n");
            continue;
        }
        AddQueryToHistory(&history, q);

        // 4) search using BOTH indexes
        printf("\n------- Search Results -------\n");
        Document *results =
          search_documents_with_index(titleIdx, bodyIdx, q, docs);
        if (!results) {
            printf("(no results)\n");
        } else {
            PrintSearchResults(results);
            FreeDocumentList(results);
        }
        FreeQuery(q);
    }

    // tear down
    FreeQueryHistory(&history);
    FreeDocumentList(docs);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    return 0;
}
