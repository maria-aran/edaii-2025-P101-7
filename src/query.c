#include "query.h"
#include "document.h"  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create and initialize an empty query linked list
Query_link_list* CreateQueryList(void) {
    Query_link_list *list = malloc(sizeof(Query_link_list));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// Add a keyword to the query list
void AddQuery(Query_link_list *list, const char *word, bool exclude) {
    Query *newNode = malloc(sizeof(Query));
    newNode->keyword = strdup(word);
    newNode->exclude = exclude;
    newNode->next = NULL;

    if (list->tail == NULL) {
        list->head = list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

// Parse a search string like "apple banana -cat" into a linked list
Query_link_list* ParseQueryString(const char *input) {
    Query_link_list *list = CreateQueryList();
    char *copy = strdup(input);
    char *token = strtok(copy, " \n\t");

    while (token != NULL) {
        bool exclude = false;
        if (token[0] == '-') {
            exclude = true;
            token++;  // Skip the '-'
        }
        if (strlen(token) > 0) {
            AddQuery(list, token, exclude);
        }
        token = strtok(NULL, " \n\t");
    }

    free(copy);
    return list;
}

// Check if a document contains a keyword (case-sensitive match)
bool ContainsKeyword(const char *text, const char *keyword) {
    const char *p = text;
    while (*p) {
        const char *t = p;
        const char *k = keyword;
        while (*k && *t == *k) {
            ++t;
            ++k;
        }
        if (*k == '\0') return true;
        ++p;
    }
    return false;
}

// Perform linear search through documents
Document* LinearSearchDocuments(Document *docs, Query_link_list *query) {
    Document *result = NULL;

    for (Document *doc = docs; doc != NULL; doc = doc->next) {
        bool matches = true;
        for (Query *q = query->head; q != NULL; q = q->next) {
            bool found = ContainsKeyword(doc->body, q->keyword);
            if ((q->exclude && found) || (!q->exclude && !found)) {
                matches = false;
                break;
            }
        }
        if (matches) {
            // Add to front of result list
            Document *copy = malloc(sizeof(Document));
            copy->id = doc->id;
            copy->body = strdup(doc->body);
            copy->next = result;
            result = copy;
        }
    }
    return result;
}

// Print the list of query keywords
void PrintQueryList(Query_link_list *list) {
    printf("Query keywords: ");
    for (Query *q = list->head; q != NULL; q = q->next) {
        printf("%s%s", q->exclude ? "-" : "", q->keyword);
        if (q->next) printf(", ");
    }
    printf("\n");
}

// Free the query list
void FreeQueryList(Query_link_list *list) {
    Query *curr = list->head;
    while (curr) {
        Query *temp = curr;
        curr = curr->next;
        free(temp->keyword);
        free(temp);
    }
    free(list);
}
