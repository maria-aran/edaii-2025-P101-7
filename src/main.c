#include "dataset.h"
#include "document.h"
#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main function: runs the search engine CLI
int main() {
    // Load all documents from the dataset folder
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia270");
    if (!docs) {
        printf("Error: Could not load documents.\n");
        return 1;
    }

    // Initialize query history (stores last 3 queries)
    QueryHistory history;
    InitQueryHistory(&history);

    char input[200]; // Buffer for user input (max 200 chars per query spec)
    while (1) {
        // Print separator for clarity
        printf("\n------- Documents -------\n");
        // Print all documents with indices
        printf("Available documents:\n");
        PrintDocumentList(docs);

        // Print separator for clarity
        printf("\n------- Recent Searches -------\n");
        // Show recent searches
        PrintQueryHistory(&history);

        // Get user query
        printf("\nEnter query (or 'exit' to quit, or number to view document): ");
        if (!fgets(input, sizeof(input), stdin)) {
            break; // Handle EOF or input error
        }
        input[strcspn(input, "\n")] = 0; // Remove newline

        // Check if user wants to exit
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Check if input is a number (to view a document)
        int doc_index;
        if (sscanf(input, "%d", &doc_index) == 1) {
            Document *selected = docs;
            int i = 0;
            // Find document by index
            while (selected && i < doc_index) {
                selected = selected->next;
                i++;
            }
            if (selected) {
                printf("\n------- Selected Document -------\n");
                PrintDocument(selected);
            } else {
                printf("Invalid document index.\n");
            }
            continue;
        }

        // Process query
        Query *query = CreateQuery(input);
        if (!query) {
            printf("Error: Invalid query.\n");
            continue;
        }

        // Add query to history
        AddQueryToHistory(&history, query);

        // Perform linear search
        printf("\n------- Search Results -------\n");
        Document *results = LinearSearchDocuments(docs, query);
        if (!results) {
            printf("No matching documents found.\n");
        } else {
            PrintSearchResults(results);
            FreeDocumentList(results);
        }
    }

    // Clean up
    FreeQueryHistory(&history);
    FreeDocumentList(docs);
    return 0;
}