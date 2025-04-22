#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "dataset.h"
#include "document.h"

//The following function loads all documents from the given folder path
Document* LoadDocumentsFromFolder(const char *folderPath) {
    DIR *dir = opendir(folderPath); //opens the folder
    if (!dir) return NULL;

    struct dirent *entry;
    Document *head = NULL; //Initialize the linked list of documents, head

    while ((entry = readdir(dir))) { //Loop throught all files
        if (entry->d_type == DT_REG) { // Regular files only, it checks if the file is regular
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);
            Document *doc = document_desserialize(path);    //Load the document from file
            if (doc) {
                doc->next = head; //Add it to the front of the list
                head = doc;
            }
        }
    }

    closedir(dir);  //CLose the directory
    return head;
}

// Frees the entire linked list of documents
void FreeDocumentList(Document *head) {
    while (head) {
        Document *temp = head;
        head = head->next;
        FreeDocument(temp); //Free each document
    }
}

// Prints the titles of all loaded documents
void PrintDocumentList(Document *head) {
    int index = 0;
    Document *current = head;
    while (current) {
        printf("Index: [%d] Title: %s\n", index++, current->title); //Prints the index and the title
        current = current->next;
    }
}