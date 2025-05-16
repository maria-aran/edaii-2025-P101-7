#include "dataset.h"
#include "document.h"
#include "hashmap.h"
#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is the main function that runs the search engine program.
int main() {
    // Load all documents from the folder "datasets/wikipedia12".
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");
    if (!docs) {
        printf("Error: Could not load documents.\n");
        return 1; // Exit if loading fails.
    }

    // Lab3: build reverse index
    HashMap *index = create_hashmap(1000);
    build_reverse_index(index, docs);

    // Create a history to store the last 3 search queries.
    QueryHistory history;
    InitQueryHistory(&history);

    char input[200];
    int running = 1;

    while (running) {
        printf("\n------- Documents -------\n");
        PrintDocumentList(docs);
        printf("\n------- Recent Searches -------\n");
        PrintQueryHistory(&history);
        printf("\nEnter query (or 'exit'): ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "exit") == 0) { running = 0; break; }
        if (input[0] == '\0') { printf("Please enter a query.\n"); continue; }

        int idx;
        if (sscanf(input, "%d", &idx) == 1) {
            Document *sel = docs;
            for (int i = 0; sel && i < idx; i++) sel = sel->next;
            if (sel) {
                printf("\n------- Selected Document -------\n");
                PrintDocument(sel);
            } else printf("Invalid index.\n");
            continue;
        }

        Query *q = CreateQuery(input);
        if (!q) { printf("Invalid query.\n"); continue; }
        AddQueryToHistory(&history, q);

        printf("\n------- Search Results -------\n");
        Document *results = search_documents_with_index(index, q, docs);
        if (!results) printf("No matching documents.\n");
        else { PrintSearchResults(results); FreeDocumentList(results); }
    }

    free_hashmap(index);
    FreeQueryHistory(&history);
    FreeDocumentList(docs);
    return 0;
}
