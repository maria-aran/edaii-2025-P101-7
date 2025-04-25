#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "query.h"
#include "document.h"

// Create an empty query list
Query_link_list* CreateQueryList(void) {
    Query_link_list *list = malloc(sizeof(Query_link_list));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// Free the memory allocated for the query list
void FreeQueryList(Query_link_list *list) {
    Query *current = list->head;
    while (current) {
        Query *temp = current;
        current = current->next;
        free(temp->keyword);
        free(temp);
    }
    free(list);
}

// Parse the query string into a linked list of Query structs
Query_link_list* ParseQueryString(const char *input) {
    Query_link_list *list = CreateQueryList();
    char *queryCopy = strdup(input); // Duplicate input string
    char *token = strtok(queryCopy, " ");
    
    while (token != NULL) {
        Query *newQuery = malloc(sizeof(Query));
        newQuery->keyword = strdup(token);
        newQuery->exclude = (newQuery->keyword[0] == '-');
        
        // If it's an exclusion keyword, remove the leading minus sign
        if (newQuery->exclude) {
            newQuery->keyword++; // Skip the minus sign
        }
        
        newQuery->next = NULL;

        // Add to the list
        if (list->tail) {
            list->tail->next = newQuery;
            list->tail = newQuery;
        } else {
            list->head = newQuery;
            list->tail = newQuery;
        }
        
        token = strtok(NULL, " ");
    }

    free(queryCopy);
    return list;
}

// Function to check if the document contains the given keyword
bool ContainsKeyword(const char *text, const char *keyword) {
    return strstr(text, keyword) != NULL;  // Simple substring search
}

// Perform linear search for documents matching the query
Document* LinearSearchDocuments(Document *docs, Query_link_list *query) {
    Document *resultHead = NULL;

    while (docs) {
        bool matchesAll = true;

        // Traverse through each query in the list
        for (Query *q = query->head; q != NULL; q = q->next) {
            const char *kw = q->keyword;

            // Handle exclusion keyword
            if (q->exclude) {
                if (ContainsKeyword(docs->title, kw) || ContainsKeyword(docs->body, kw)) {
                    matchesAll = false;
                    break;
                }
            } else {
                // Regular keyword must appear
                if (!ContainsKeyword(docs->title, kw) && !ContainsKeyword(docs->body, kw)) {
                    matchesAll = false;
                    break;
                }
            }
        }

        // If document matches the query, add it to the result list
        if (matchesAll) {
            Document *copy = CreateDocument(docs->id, docs->title, docs->body);
            copy->next = resultHead;
            resultHead = copy;
        }

        docs = docs->next;
    }

    return resultHead;
}

// Print the query list
void PrintQueryList(Query_link_list *list) {
    Query *current = list->head;
    while (current) {
        printf("Keyword: %s, Exclude: %s\n", current->keyword, current->exclude ? "Yes" : "No");
        current = current->next;
    }
}