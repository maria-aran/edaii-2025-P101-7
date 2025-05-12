#include "dataset.h"
#include "document.h"
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

    // Create a history to store the last 3 search queries.
    QueryHistory history;
    InitQueryHistory(&history);

    // Buffer to hold user input (up to 200 characters).
    char input[200];
    while (1) {
        // Print a line to separate sections for clarity.
        printf("\n------- Documents -------\n");
        printf("Available documents:\n");
        // Show all document titles with numbers (e.g., [0] Cats).
        PrintDocumentList(docs);

        // Show the last 3 searches (or "(none)" if none).
        printf("\n------- Recent Searches -------\n");
        PrintQueryHistory(&history);

        // Ask the user to type a query, a number, or "exit".
        printf("\nEnter query (or 'exit' to quit, or number to view document): ");
        if (!fgets(input, sizeof(input), stdin)) {
            break; // Exit if input fails (e.g., Ctrl+D).
        }
        input[strcspn(input, "\n")] = 0; // Remove the newline character.

        // If the user types "exit", quit the program.
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // If the input is empty (just Enter), skip to the next loop.
        if (input[0] == '\0') {
            printf("Please enter a query, number, or 'exit'.\n");
            continue;
        }

        // Check if the user typed a number (to view a document).
        int doc_index;
        if (sscanf(input, "%d", &doc_index) == 1) {
            Document *selected = docs;
            int i = 0;
            // Find the document with the given index.
            while (selected && i < doc_index) {
                selected = selected->next;
                i++;
            }
            if (selected) {
                // Show the full document details.
                printf("\n------- Selected Document -------\n");
                PrintDocument(selected);
            } else {
                printf("Invalid document index.\n");
            }
            continue;
        }

        // Create a query from the user’s input (e.g., "cat -dog").
        Query *query = CreateQuery(input);
        if (!query) {
            printf("Error: Invalid query.\n");
            continue;
        }

        // Add the query to the history (keeps last 3).
        AddQueryToHistory(&history, query);

        // Search for documents matching the query.
        printf("\n------- Search Results -------\n");
        Document *results = LinearSearchDocuments(docs, query);
        if (!results) {
            printf("No matching documents found.\n");
        } else {
            // Show up to 5 matching documents with title and body snippet.
            PrintSearchResults(results);
            FreeDocumentList(results); // Free the results list.
        }
    }

    // Free memory before exiting.
    FreeQueryHistory(&history);
    FreeDocumentList(docs);
    return 0;
}