#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BODY 4000

// Create a new document with given id, title, and body
Document *CreateDocument(int id, const char *title, const char *body) {

    Document *doc = malloc(sizeof(Document)); 
    if (doc == NULL) {
        return NULL;  // if failed to allocate the Document struct -->stop
    }

    doc->id = id;

    // Copy title if is not empty
    if (title != NULL) {

        // Find length of title
        size_t len = 0;
        while (title[len] != '\0') {
            len++; //makes length greater until NULLTERMINATOR
        }

        // Allocate space for title + null terminator
        doc->title = malloc(len + 1);

        if (doc->title == NULL) {
            free(doc);
            return NULL;  // Failed to allocate title
        }

        // Copy characters one by one
        for (size_t i = 0; i <= len; i++) {
            doc->title[i] = title[i];  // includes copying the '\0'
        }

    } else {
        doc->title = NULL;
    }

    // Copy body if provided (same process as the title with the body)

    if (body != NULL) {

        // Find length of body
        size_t len = 0;
        while (body[len] != '\0') {
            len++;
        }

        // Allocate space for body + null terminator
        doc->body = malloc(len + 1);
        if (doc->body == NULL) {
            // Clean up title and doc if body allocation fails
            if (doc->title != NULL) free(doc->title);
            free(doc);
            return NULL;
        }

        // Copy characters one by one
        for (size_t i = 0; i <= len; i++) {
            doc->body[i] = body[i];  // includes copying the '\0'
        }

    } else {
        doc->body = NULL;
    }

    doc->relevance = 0.0f;        // Initial relevance score

    doc->links     = LinksInit(); // Initialize empty list of links
    
    doc->next      = NULL;       // No next document in list yet

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

    printf("---Document ID: %d---\n", doc->id);

    printf("---Title: %s---\n", doc->title ? doc->title : "(no title)");
    printf("---Links: ");
    PrintLinks(doc->links);
    printf("---");
    printf("---Body: ");
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
    printf("---");
}

// Parse a document from a file --> extract structured data
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

    for (int i = 0; title[i] != '\0'; i++) { //remove newline, if there's any 
        if (title[i] == '\n') {
            title[i] = '\0';
            break;
        }
    }

    // Read body (everything else)
    char body[4096] = ""; // Reasonable buffer for body
    char line[1024];

    /* Read every character from file into body until EOF or buffer full */
    int pos = 0;
    int c;

    while ((c = fgetc(file)) != EOF && pos < MAX_BODY - 1) {
        body[pos++] = (char)c;
    }

    body[pos] = '\0';  /* Null-terminate the string */

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