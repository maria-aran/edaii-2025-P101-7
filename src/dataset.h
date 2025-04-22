#ifndef DATASET_H
#define DATASET_H
#include "document.h"

Document* LoadDocumentsFromFolder(const char *folderPath);

void FreeDocumentList(Document *head);

void PrintDocumentList(Document *head);

#endif