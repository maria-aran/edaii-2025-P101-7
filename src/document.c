#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "document.h"
#include "links.h"

Document *document_desserialize(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    Document *document = malloc(sizeof(Document));
    char buffer[262144];
    int bufferIdx = 0;
    char ch;

    // ID
    while ((ch = fgetc(f)) != '\n') buffer[bufferIdx++] = ch;
    buffer[bufferIdx] = '\0';
    document->id = atoi(buffer);

    // Title
    bufferIdx = 0;
    while ((ch = fgetc(f)) != '\n') buffer[bufferIdx++] = ch;
    buffer[bufferIdx] = '\0';
    document->title = strdup(buffer);

    // Body and Links
    document->links = LinksInit();
    Link *links = document->links;
    char linkBuffer[64];
    int linkBufferIdx = 0;
    bool parsingLink = false;
    bufferIdx = 0;

    while ((ch = fgetc(f)) != EOF) {
        buffer[bufferIdx++] = ch;
        if (parsingLink) {
            if (ch == ')') {
                parsingLink = false;
                linkBuffer[linkBufferIdx] = '\0';
                int linkId = atoi(linkBuffer);
                Link *newLink = malloc(sizeof(Link));
                newLink->id = linkId;
                newLink->next = links->next;
                links->next = newLink;
                linkBufferIdx = 0;
            } else if (ch != '(') {
                linkBuffer[linkBufferIdx++] = ch;
            }
        } else if (ch == ']') {
            parsingLink = true;
        }
    }
    buffer[bufferIdx] = '\0';
    document->body = strdup(buffer);
    document->next = NULL;

    fclose(f);
    return document;
}

Document* CreateDocument(int id, const char* title, const char* body) {
    Document* doc = malloc(sizeof(Document));
    doc->id = id;
    doc->title = strdup(title);
    doc->body = strdup(body);
    doc->links = LinksInit();
    doc->next = NULL;
    return doc;
}

void FreeDocument(Document *doc) {
    if (!doc) return;
    free(doc->title);
    free(doc->body);
    FreeLinks(doc->links);
    free(doc);
}

void PrintDocument(Document *doc) {
    printf("Document ID: %d\n", doc->id);
    printf("Title: %s\n", doc->title);
    printf("Body: %s\n", doc->body);
    printf("Links: ");
    PrintLinks(doc->links);
}