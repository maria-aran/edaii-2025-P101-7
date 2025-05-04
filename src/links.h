#ifndef LINKS_H
#define LINKS_H

// Link structure: holds a link ID and pointer to next link
typedef struct Link {
    int id;           // Document ID this link points to
    struct Link *next; // Next link in the list
} Link;

// Initialize an empty links list
Link *LinksInit();
// Free all links
void FreeLinks(Link *head);
// Print all link IDs
void PrintLinks(Link *head);

#endif