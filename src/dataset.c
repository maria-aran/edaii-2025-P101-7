#include "dataset.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load all documents from a folder into a linked list
Document *LoadDocumentsFromFolder(const char *folderPath) {
    DIR *dir = opendir(folderPath);
    if (!dir) {
        printf("Error: Cannot open directory %s\n", folderPath);
        return NULL;
    }

    Document *head = NULL; // Head of document list
    struct dirent *entry;

    // Read each file in the directory
    while ((entry = readdir(dir))) {
        // Only process regular files
        if (entry->d_type == DT_REG) {
            // Build full file path
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);

            // Load document
            Document *doc = document_desserialize(path);
            if (doc) {
                // Add to front of list
                doc->next = head;
                head = doc;
            }
        }
    }

    closedir(dir);
    return head;
}

// Free the entire document list
void FreeDocumentList(Document *head) {
    Document *current = head;
    while (current) {
        Document *temp = current;
        current = current->next;
        FreeDocument(temp);
    }
}

// Print all document titles with indices
void PrintDocumentList(Document *head) {
    Document *current = head;
    int index = 0;
    if (!current) {
        printf("(no documents)\n");
        return;
    }
    while (current) {
        printf("[%d] %s\n", index++, current->title ? current->title : "(no title)");
        current = current->next;
    }
}

// Print up to 5 search results with title and first 150 chars of body
void PrintSearchResults(Document *head) {
    Document *current = head;
    int count = 0;
    if (!current) {
        printf("(no results)\n");
        return;
    }
    while (current && count < 5) {
        printf("[%d] %s\n", count, current->title ? current->title : "(no title)");
        printf("Body: ");
        if (current->body) {
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
        printf("\n"); // Add space between results
        current = current->next;
        count++;
    }
}