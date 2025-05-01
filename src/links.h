#ifndef LINKS_H
#define LINKS_H

typedef struct Link {
  int id;
  struct Link *next;
} Link;

// Initialize an empty Link list (with head node)
Link *LinksInit();

// Free a linked list of Links
void FreeLinks(Link *head);

// Print all link IDs
void PrintLinks(Link *head);

#endif