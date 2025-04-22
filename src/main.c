#include <stdio.h>
#include <stdlib.h>
#include "document.h"
#include "dataset.h"

int main() {
    // Load all documents from the dataset folder
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");

    if (!docs) {
        printf("No documents found.\n");
        return 1;
    }

    printf("Documents loaded:\n");
    PrintDocumentList(docs); //Print the list of tittles

    //Asks users what document to view
    int choice;
    printf("Enter document index to view: ");
    scanf("%d", &choice);

    
    Document *selected = docs;
    for (int i = 0; i < choice && selected != NULL; i++) {
        selected = selected->next;
    }

    // Print the selected document or error if index is invalid
    if (selected) {
        PrintDocument(selected);
    } else {
        printf("Invalid selection.\n");
    }

    // Free memory
    FreeDocumentList(docs);
    return 0;
}
