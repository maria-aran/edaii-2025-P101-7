#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"        // Added for LoadDocumentsFromFolder and FreeDocumentList
#include "document.h"       // For Document
#include "query.h"          // For Query, Query_link_list, ParseQueryString, etc.

int main() {
    // Load documents
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia270");
    if (!docs) {
        printf("No documents loaded.\n");
        return 1;
    }

    // Prompt user for a query
    char input[256];
    printf("Enter your query: ");
    if (!fgets(input, sizeof(input), stdin)) {
        printf("Failed to read input.\n");
        return 1;
    }

    // Remove newline if present
    input[strcspn(input, "\n")] = '\0';

    // Parse query string into linked list
    Query_link_list *queryList = ParseQueryString(input);
    if (!queryList) {
        printf("Failed to parse query.\n");
        return 1;
    }

    // Debug print the parsed query
    PrintQueryList(queryList);

    // Free query list memory
    FreeQueryList(queryList);

    // Free document list memory
    FreeDocumentList(docs); // Corrected from FreeDocuments to FreeDocumentList

    return 0;
}