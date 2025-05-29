#ifndef QUERY_H
#define QUERY_H

#include "document.h"

#define HISTORY_CAPACITY 3

// Query node: holds a keyword and whether it's excluded
typedef struct QueryNode {
  char *keyword;            // The keyword
  int exclude;              // 1 if excluded, 0 otherwise
  int or_flag;              // 1 if keyword came from an (a|b|c) group --> OR condition
  struct QueryNode *next;   // Next keyword in query.
} QueryNode;

// Query: list of query nodes
typedef struct Query {
    QueryNode *head; // Head of query nodes
} Query;

// Query history: stores last 3 queries
typedef struct {
  char *slots[HISTORY_CAPACITY]; // Array of queries --> changed into an array of string to prevent segmentation error
  int front;                     // Index of oldest query
  int count;                     // Number of queries stored
} QueryHistory;

// Create a query from a string
Query *CreateQuery(const char *str);
// Free a query
void FreeQuery(Query *query);
// Perform linear search with query
Document *LinearSearchDocuments(Document *docs, Query *query);
// Initialize query history
void InitQueryHistory(QueryHistory *history);
// Add query to history
void AddQueryToHistory(QueryHistory *history, const char *queryStr);
// Print query history
void PrintQueryHistory(const QueryHistory *history);
// Free query history
void FreeQueryHistory(QueryHistory *history);

int match_term(const char *text, const char *term);

#endif