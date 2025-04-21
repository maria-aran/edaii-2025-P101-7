#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "links.h"

typedef struct{
    int id;
    char *title;
    char *body;
    Link *links;
    struct Document *next;
} Document;

Document *document_desserialize(char *path);
Document *CreateDocument(int id, const char *title, const char *body);
void FreeDocument(Document *doc);
void PrintDocument(Document *doc);

#endif