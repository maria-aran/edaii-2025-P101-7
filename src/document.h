#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "links.h"

// Document structure: holds document data and links to next document
typedef struct Document {
    int id;           // Document ID
    char *title;      // Document title
    char *body;       // Document body
    float relevance;  // Relevance score (not used until Lab 4)
    Link *links;      // List of links to other documents
    struct Document *next; // Next document in linked list
} Document;

// Create a new document
Document *CreateDocument(int id, const char *title, const char *body);

// Free a single document
void FreeDocument(Document *doc);

// Print a single document
void PrintDocument(Document *doc);

// Load document from file
Document *document_desserialize(char *path);


#endif