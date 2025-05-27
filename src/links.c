#include "links.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize an empty links list with a dummy head node
Link *LinksInit() {
    Link *head = malloc(sizeof(Link));
    if (!head) return NULL;
    head->id = -1; // Invalid ID for dummy node
    head->next = NULL;
    return head;
}

// Free all links in the list
void FreeLinks(Link *head) {
    Link *current = head;
    while (current) {
        Link *temp = current;
        current = current->next;
        free(temp);
    }
}

// Print all link IDs
void PrintLinks(Link *head) {
    Link *current = head ? head->next : NULL;
    if (!current) {
        printf("(none)\n");
        return;
    }
    while (current) {
        printf("%d", current->id);
        if (current->next) printf(", ");
        current = current->next;
    } 
    printf("\n");
}