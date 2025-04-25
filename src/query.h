#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>
#include "document.h" 

typedef struct Query {
    char *keyword;        // Keyword in the query
    struct Query *next;   // Pointer to next query in the list
    bool exclude;         // Whether the keyword is an exclusion (e.g., -cat)
} Query;

typedef struct {
    Query *head;          // Head of the linked list
    Query *tail;          // Tail of the linked list
} Query_link_list;

// Function declarations
Query_link_list* CreateQueryList(void);
void FreeQueryList(Query_link_list *list);
Query_link_list* ParseQueryString(const char *input);
bool ContainsKeyword(const char *text, const char *keyword);
Document* LinearSearchDocuments(Document *docs, Query_link_list *query);
void PrintQueryList(Query_link_list *list);

#endif
