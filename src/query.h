#ifndef QUERY_H
#define QUERY_H

typedef struct Query {
    char *keyword;
    struct Query *next;
} Query;

// Create an empty query list (dummy head)
Query *QueryInit();

// Add a keyword to the query list
void QueryAdd(Query *head, const char *keyword);

// Parse a string into a linked list of Query nodes
Query *ParseQueryString(const char *input);

// Print all keywords in the query (for debugging)
void PrintQuery(Query *head);

// Free the query list
void FreeQuery(Query *head);

#endif
