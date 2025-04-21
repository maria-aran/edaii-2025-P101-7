#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "dataset.h"
#include "document.h"

Document* LoadDocumentsFromFolder(const char *folderPath) {
    DIR *dir = opendir(folderPath);
    if (!dir) return NULL;

    struct dirent *entry;
    Document *head = NULL;

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) { // Regular files only
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);
            Document *doc = document_desserialize(path);
            if (doc) {
                doc->next = head;
                head = doc;
            }
        }
    }

    closedir(dir);
    return head;
}

void FreeDocumentList(Document *head) {
    while (head) {
        Document *temp = head;
        head = head->next;
        FreeDocument(temp);
    }
}

void PrintDocumentList(Document *head) {
    int index = 0;
    Document *current = head;
    while (current) {
        printf("[%d] %s\n", index++, current->title);
        current = current->next;
    }
}