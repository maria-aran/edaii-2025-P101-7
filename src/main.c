#include "dataset.h"
#include "document.h"
#include "query.h"
#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Load all documents
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (!docs) {
        fprintf(stderr, "Error loading documents\n");
        return 1;
    }

    // Recent-search history
    QueryHistory history;
    InitQueryHistory(&history);

    // Build title & body reverse indexes
    HashMap *titleIdx = create_hashmap(10007);
    HashMap *bodyIdx  = create_hashmap(20011);
    build_reverse_indexes(titleIdx, bodyIdx, docs);

    char line[256];
    while (1) {
        // ——— Documents ———
        printf("\n------- Documents -------\n");
        PrintDocumentList(docs);

        // ——— Recent Searches ———
        printf("\n------- Recent Searches -------\n");
        PrintQueryHistory(&history);

        // Prompt
        printf("\nEnter search (or \"exit\"): ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "exit") == 0) break;
        if (line[0] == '\0') continue;

        // Parse & record
        Query *q = CreateQuery(line);
        if (!q) {
            fprintf(stderr, "Invalid query\n");
            continue;
        }
        AddQueryToHistory(&history, q);

        // Indexed search
        Document *res = search_documents_with_index(
            titleIdx, bodyIdx, q, docs
        );

        // ——— Search Results ———
        printf("------- Search Results -------\n");
        int idx = 0;
        for (Document *d = res; d; d = d->next) {
            // Print every matching document
            printf("[%d] %s\n", idx++, d->title);
            print_snippet(d->body, q);
            printf("\n");
        }
        if (idx == 0) {
            printf("(no matches)\n");
        }

        // Cleanup this round
        FreeDocumentList(res);
    }

    //  cleanup
    FreeQueryHistory(&history);
    free_hashmap(titleIdx);
    free_hashmap(bodyIdx);
    FreeDocumentList(docs);
    return 0;
}
