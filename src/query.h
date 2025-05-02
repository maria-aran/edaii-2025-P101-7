#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>

// Forward declaration to avoid circular include
struct Document;
typedef struct Document Document;

typedef struct Query {
    char *keyword;
    struct Query *next;
    bool exclude;
} Query;

typedef struct {
    Query *head;
    Query *tail;
} Query_link_list;

// Function declarations
Query_link_list* CreateQueryList(void);
void FreeQueryList(Query_link_list *list);
Query_link_list* ParseQueryString(const char *input);
bool ContainsKeyword(const char *text, const char *keyword);
Document* LinearSearchDocuments(Document *docs, Query_link_list *query);
void PrintQueryList(Query_link_list *list);

#endif

