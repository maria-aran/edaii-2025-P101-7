#include <stdlib.h>
#include <stdio.h>
#include "links.h"

Link* LinksInit() {
    Link *head = (Link *)malloc(sizeof(Link));
    head->id = -1;  // Dummy head node
    head->next = NULL;
    return head;
}

void FreeLinks(Link *head) {
    Link *current = head;
    while (current != NULL) {
        Link *temp = current;
        current = current->next;
        free(temp);
    }
}

void PrintLinks(Link *head) {
    Link *current = head->next;
    if (!current) {
        printf("(none)");
    }
    while (current) {
        printf("%d", current->id);
        if (current->next)
            printf(", ");
        current = current->next;
    }
    printf("\n");
}
