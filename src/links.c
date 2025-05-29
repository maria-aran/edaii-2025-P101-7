#include "links.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize an empty links list with a dummy head node
Link *LinksInit() {

  Link *head = malloc(sizeof(Link));

  if (!head)
    return NULL;

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

// Append a new link with given id to the end of the list
void LinksAppend(Link *head, int id) {
  Link *new_link = malloc(sizeof(Link));
  if (!new_link)
    return; // allocation failed → no-op
  new_link->id = id;
  new_link->next = NULL;

  // walk to the end of the list
  Link *cur = head;
  while (cur->next) {
    cur = cur->next;
  }
  cur->next = new_link;
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
    if (current->next)
      printf(", ");
    current = current->next;
  }
  printf("\n");
}