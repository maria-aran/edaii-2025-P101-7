#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new document with given id, title, and body
Document *CreateDocument(int id, const char *title, const char *body) {
    Document *doc = malloc(sizeof(Document));
    if (!doc) return NULL;
    doc->id = id;
    doc->title = title ? strdup(title) : NULL;
    doc->body = body ? strdup(body) : NULL;
    doc->relevance = 0.0f; // Initialize relevance (used later)
    doc->links = LinksInit(); // Initialize empty links list
    doc->next = NULL;
    return doc;
}

// Free a document and its resources
void FreeDocument(Document *doc) {
    if (!doc) return;
    free(doc->title);
    free(doc->body);
    FreeLinks(doc->links);
    free(doc);
}

// Print a document's details
void PrintDocument(Document *doc) {
    if (!doc) {
        printf("No document to display.\n");
        return;
    }
    printf("Document ID: %d\n", doc->id);
    printf("Title: %s\n", doc->title ? doc->title : "(no title)");
    printf("Links: ");
    PrintLinks(doc->links);
    printf("Body: ");
    if (doc->body) {
        // Print first 150 chars of body with ellipsis if longer
        if (strlen(doc->body) > 150) {
            char snippet[151];
            strncpy(snippet, doc->body, 150);
            snippet[150] = '\0';
            printf("%s...\n", snippet);
        } else {
            printf("%s\n", doc->body);
        }
    } else {
        printf("(no body)\n");
    }
}

// Parse a document from a file
Document *document_desserialize(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return NULL;

    // Read document ID
    int id;
    if (fscanf(file, "%d\n", &id) != 1) {
        fclose(file);
        return NULL;
    }

    // Read title (first line after ID)
    char title[1024] = "";
    if (!fgets(title, sizeof(title), file)) {
        fclose(file);
        return NULL;
    }
    title[strcspn(title, "\n")] = 0; // Remove newline

    // Read body (everything else)
    char body[4096] = ""; // Reasonable buffer for body
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Check for links in format [text](id)
        char *link_start = strchr(line, '[');
        char *link_end = link_start ? strchr(link_start, ']') : NULL;
        char *id_start = link_end ? strchr(link_end, '(') : NULL;
        char *id_end = id_start ? strchr(id_start, ')') : NULL;

        if (link_start && link_end && id_start && id_end && id_start == link_end + 1) {
            // Extract link ID
            int link_id;
            if (sscanf(id_start + 1, "%d", &link_id) == 1) {
                // Add link to document later (store temporarily)
                strncat(body, line, sizeof(body) - strlen(body) - 1);
                continue;
            }
        }
        // Append line to body
        strncat(body, line, sizeof(body) - strlen(body) - 1);
    }

    // Create document
    Document *doc = CreateDocument(id, title, body);
    if (!doc) {
        fclose(file);
        return NULL;
    }

    // Rewind file to parse links
    fseek(file, 0, SEEK_SET);
    fscanf(file, "%d\n", &id); // Skip ID
    fgets(line, sizeof(line), file); // Skip title

    // Parse links
    while (fgets(line, sizeof(line), file)) {
        char *link_start = strchr(line, '[');
        char *link_end = link_start ? strchr(link_start, ']') : NULL;
        char *id_start = link_end ? strchr(link_end, '(') : NULL;
        char *id_end = id_start ? strchr(id_start, ')') : NULL;

        if (link_start && link_end && id_start && id_end && id_start == link_end + 1) {
            int link_id;
            if (sscanf(id_start + 1, "%d", &link_id) == 1) {
                // Add link to links list
                Link *new_link = malloc(sizeof(Link));
                new_link->id = link_id;
                new_link->next = doc->links->next;
                doc->links->next = new_link;
            }
        }
    }

    fclose(file);
    return doc;
}