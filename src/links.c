#include "links.h"
#include <stdio.h>
#include <stdlib.h>

Link *LinksInit() {
  Link *head = (Link *)malloc(sizeof(Link));
  head->id = -1; // Dummy head node (with invalid id)
  head->next = NULL;
  return head;
}

// Frees all links from memory
void FreeLinks(Link *head) {
  Link *current = head;
  while (current != NULL) {
    Link *temp = current;
    current = current->next;
    free(temp);
  }
}

// Prints the list of link IDs
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
