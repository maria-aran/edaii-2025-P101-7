#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "query.h"

// Initialize a dummy head for the query list
Query *QueryInit() {
    Query *head = malloc(sizeof(Query));
    head->keyword = NULL;
    head->next = NULL;
    return head;
}

// Add a keyword to the end of the list
void QueryAdd(Query *head, const char *keyword) {
    Query *newNode = malloc(sizeof(Query));
    newNode->keyword = strdup(keyword);
    newNode->next = NULL;

    Query *curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = newNode;
}

// Parse a string like "apple banana -cat" into a linked list
Query *ParseQueryString(const char *input) {
    Query *head = QueryInit();
    char *copy = strdup(input);
    char *token = strtok(copy, " \n\t");

    while (token != NULL) {
        if (strlen(token) > 0) {
            QueryAdd(head, token);
        }
        token = strtok(NULL, " \n\t");
    }

    free(copy);
    return head;
}

// Print the list of keywords (for debugging)
void PrintQuery(Query *head) {
    Query *curr = head->next;
    printf("Query keywords: ");
    while (curr != NULL) {
        printf("%s", curr->keyword);
        if (curr->next) printf(", ");
        curr = curr->next;
    }
    printf("\n");
}

// Free all memory associated with the query list
void FreeQuery(Query *head) {
    Query *curr = head;
    while (curr != NULL) {
        Query *temp = curr;
        curr = curr->next;
        free(temp->keyword);
        free(temp);
    }
}

