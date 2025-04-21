#include <stdio.h>
#include <stdlib.h>
#include "document.h"
#include "dataset.h"

int main() {
    Document *docs = LoadDocumentsFromFolder("datasets/wikipedia12");

    if (!docs) {
        printf("No documents found.\n");
        return 1;
    }

    printf("Documents loaded:\n");
    PrintDocumentList(docs);

    int choice;
    printf("Enter document index to view: ");
    scanf("%d", &choice);

    Document *selected = docs;
    for (int i = 0; i < choice && selected != NULL; i++) {
        selected = selected->next;
    }

    if (selected) {
        PrintDocument(selected);
    } else {
        printf("Invalid selection.\n");
    }

    FreeDocumentList(docs);

    // Uncomment to test memory leaks
    // createaleak();

    return 0;

}

