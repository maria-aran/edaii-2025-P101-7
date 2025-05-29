#include "links.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize an empty links list with a dummy head node
Link *LinksInit() {
<<<<<<< HEAD

  Link *head = malloc(sizeof(Link));

  if (!head){ //error detection
    return NULL;
  }

  head->id = -1; // Invalid ID for dummy node --> helps making the code easier

  head->next = NULL;

  return head;
=======
    Link *head = malloc(sizeof(Link));
    if (!head) return NULL;
    head->id = -1; // Invalid ID for dummy node
    head->next = NULL;
    return head;
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
}

// Free all links in the list
void FreeLinks(Link *head) {
<<<<<<< HEAD

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
    return; // allocation failed

  new_link->id = id;
  new_link->next = NULL;

  // walk to the end of the list
  Link *cur = head;
  while (cur->next) {
    cur = cur->next;
  }
  cur->next = new_link;
=======
    Link *current = head;
    while (current) {
        Link *temp = current;
        current = current->next;
        free(temp);
    }
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
}

// Print all link IDs

void PrintLinks(Link *head) {
<<<<<<< HEAD

  Link *current = head ? head->next : NULL;

  if (!current) {
    printf("(none)\n");
    return;
  }

  while (current) {
    printf("%d", current->id);
    if (current->next)
      printf(", "); // if there is another link, print a comma 
    current = current->next;
  }
  
  printf("\n");
=======
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
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
}