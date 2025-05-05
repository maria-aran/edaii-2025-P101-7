#include "dataset.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load all documents from a folder into a list.
Document *LoadDocumentsFromFolder(const char *folderPath) {
    // Open the folder (e.g., "datasets/wikipedia12").
    DIR *dir = opendir(folderPath);
    if (!dir) {
        printf("Error: Cannot open directory %s\n", folderPath);
        return NULL;
    }

    Document *head = NULL; // Start with an empty list.

    // Read each file in the folder.
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        // Only process regular files (not directories).
        if (entry->d_type == DT_REG) {
            // Create the full file path (e.g., "datasets/wikipedia12/doc1.txt").
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);

            // Load the document from the file.
            Document *doc = document_desserialize(path);
            if (doc) {
                // Add the document to the front of the list.
                doc->next = head;
                head = doc;
            }
        }
    }

    closedir(dir); // Close the folder.
    return head; // Return the list of documents.
}

// Free all documents in the list.
void FreeDocumentList(Document *head) {
    Document *current = head;
    while (current) {
        Document *temp = current; // Save current document.
        current = current->next; // Move to next document.
        FreeDocument(temp); // Free the saved document.
    }
}

// Print all document titles with numbers (e.g., [0] Cats).
void PrintDocumentList(Document *head) {
    Document *current = head;
    int index = 0;
    if (!current) {
        printf("(no documents)\n");
        return;
    }
    while (current) {
        // Print the index and title (or "(no title)" if missing).
        printf("[%d] %s\n", index++, current->title ? current->title : "(no title)");
        current = current->next;
    }
}

// Print all search results with title and first 150 characters of body.
void PrintSearchResults(Document *head) {
    Document *current = head;
    int count = 0;

    if (!current) {
        printf("(no results)\n");
        return;
    }

    while (current) {
        // Print the result number and title.
        printf("[%d] %s\n", count, current->title ? current->title : "(no title)");
        printf("Body: ");
        if (current->body) {
            // If body is longer than 150 characters, show only 150 with "...".
            if (strlen(current->body) > 150) {
                char snippet[151];
                strncpy(snippet, current->body, 150);
                snippet[150] = '\0';
                printf("%s...\n", snippet);
            } else {
                printf("%s\n", current->body);
            }
        } else {
            printf("(no body)\n");
        }
        printf("\n"); // Add a blank line between results.

        current = current->next;
        count++;
    }
}

