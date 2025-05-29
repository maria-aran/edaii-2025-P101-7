#ifndef DATASET_H
#define DATASET_H

#include "document.h"

// Load all documents from a folder
Document *LoadDocumentsFromFolder(const char *folderPath);

// Free the document list
void FreeDocumentList(Document *head);

// Print all document titles with indices
void PrintDocumentList(Document *head);

// Print search results with title and body snippet
void PrintSearchResults(Document *head);

#endif