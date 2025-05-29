#include "dataset.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 1000

// Load all documents from a folder into a linked list.
Document *LoadDocumentsFromFolder(const char *folderPath) { 

    //the directory is openned
  DIR *dir = opendir(folderPath);
  if (dir == NULL) { //error detection
    printf("Error: cannot open \"%s\"\n", folderPath);
    return NULL;
  }

  Document *head = NULL;
  struct dirent *entry;
  char path[MAX_PATH];

 //iterate thought the whole folder
  while ((entry = readdir(dir)) != NULL) { //reads directory

    /* Build the full path: folderPath + "/" + entry->d_name */
    strcpy(path, folderPath);
    strcat(path, "/"); // string concatenate
    strcat(path, entry->d_name);

    /* deserialize whatever is at this path */
    Document *doc = document_desserialize(path);

    if (doc != NULL) {
      /* Prepend the new document to our list */
      doc->next = head;
      head = doc;
    }
  }

  closedir(dir);
  return head;
}
