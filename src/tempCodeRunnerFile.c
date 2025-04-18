#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "document.h"

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}


int main() {
    // Path to a specific file (fixed to point to a file, not a directory)
    char *path = "datasets/wikipedia12/2.txt";

    // Call document_desserialize
    Document *doc = document_desserialize(path);
    if (doc == NULL) {
        printf("Failed to deserialize document from %s\n", path);
        return 1; // Changed to return 1 to indicate error
    }

    // Print Document fields
    printf("Document ID: %d\n", doc->id);
    printf("Document Title: %s\n", doc->title);
    printf("Document Body: %s\n", doc->body);

    // Print Links
    printf("Document Links: ");
    Link *current = doc->links->next; // Skip head node (assuming LinksInit creates a head node)
    if (current == NULL) {
        printf("(none)\n");
    } else {
        while (current != NULL) {
            printf("%d", current->id);
            if (current->next != NULL) {
                printf(", ");
            }
            current = current->next;
        }
        printf("\n");
    }

    // Free allocated memory
    free(doc->title);
    free(doc->body);
    Link *temp;
    current = doc->links;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    free(doc);

    // Uncomment to test memory leaks
    // createaleak();

    return 0;
}