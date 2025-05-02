#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "links.h"

<<<<<<< Updated upstream
<<<<<<< Updated upstream
// Structure of Document:
typedef struct Document {
  int id;
  char *title;
  float revelance;
  char *body;
  Link *links;
  struct Document *next;
=======
=======
>>>>>>> Stashed changes
//Structure of Document:
typedef struct Document{
    int id;
    char *title;
    float revelance;
    char *body;
    Link *links;
    struct Document *next;
>>>>>>> Stashed changes
} Document;

// functions for documents:
Document *document_desserialize(char *path);
Document *CreateDocument(int id, const char *title, const char *body);
void FreeDocument(Document *doc);
void PrintDocument(Document *doc);

#endif