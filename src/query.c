#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create a query from a string (e.g., "cute cat -dog").
Query *CreateQuery(const char *str) {
    // Allocate memory for the query.
    Query *query = malloc(sizeof(Query));
    if (!query) return NULL;
    query->head = NULL; // Start with an empty list of keywords.

    // Copy the input string so we can modify it.
    char *input = strdup(str);
    if (!input) {
        free(query);
        return NULL;
    }

    // Split the input into words (keywords) using spaces.
    char *token = strtok(input, " ");
    while (token) {
        int exclude = 0; // 0 means include, 1 means exclude.
        char *keyword = token;

        // If the word starts with '-', it’s an exclude keyword.
        if (token[0] == '-') {
            exclude = 1;
            keyword = token + 1; // Skip the '-'.
            if (!keyword[0]) {   // Skip empty keywords (e.g., "- ").
                token = strtok(NULL, " ");
                continue;
            }
        }

        // Create a new node for this keyword.
        QueryNode *node = malloc(sizeof(QueryNode));
        if (!node) {
            free(input);
            FreeQuery(query);
            return NULL;
        }
        node->keyword = strdup(keyword); // Copy the keyword.
        node->exclude = exclude;
        node->next = query->head; // Add to front of list.
        query->head = node;

        token = strtok(NULL, " "); // Get next word.
    }

    free(input); // Free the copied input string.
    return query;
}

// Free a query and all its keywords.
void FreeQuery(Query *query) {
    if (!query) return;
    QueryNode *current = query->head;
    while (current) {
        QueryNode *temp = current;
        current = current->next;
        free(temp->keyword); // Free the keyword string.
        free(temp); // Free the node.
    }
    free(query); // Free the query.
}

// Check if a text contains a keyword as a whole word (case-sensitive).
int string_contains(const char *text, const char *keyword) {
    if (!text || !keyword || !keyword[0]) return 0;

    // Go through the text character by character.
    int text_len = strlen(text);
    int keyword_len = strlen(keyword);
    int i = 0;

    while (i <= text_len - keyword_len) {
        // Check if the keyword matches at this position.
        int match = 1;
        for (int j = 0; j < keyword_len; j++) {
            if (text[i + j] != keyword[j]) {
                match = 0;
                break;
            }
        }

        // If we found a match, check if it’s a whole word.
        if (match) {
            // Check start: either beginning of text or not a letter/number.
            int is_word_start = (i == 0) || !isalnum(text[i - 1]);
            // Check end: either end of text or not a letter/number.
            int is_word_end = (i + keyword_len == text_len) || !isalnum(text[i + keyword_len]);

            if (is_word_start && is_word_end) {
                return 1; // Found the keyword as a whole word.
            }
        }

        i++; // Move to next character.
    }

    return 0; // Keyword not found.
}

// Search for up to 5 documents that match the query.
Document *LinearSearchDocuments(Document *docs, Query *query) {
    Document *results = NULL; // List of matching documents.
    int count = 0; // Number of matches found.

    // Go through each document.
    for (Document *doc = docs; doc && count < 5; doc = doc->next) {
        int include_ok = 1; // Assume all include keywords are found.
        int exclude_ok = 1; // Assume no exclude keywords are found.

        // Check each keyword in the query.
        for (QueryNode *node = query->head; node; node = node->next) {
            // Check if the keyword is in the title or body.
            int found = string_contains(doc->title, node->keyword) ||
                        string_contains(doc->body, node->keyword);

            if (node->exclude) {
                // If it’s an exclude keyword (e.g., -dog) and found, reject the document.
                if (found) {
                    exclude_ok = 0;
                    // printf("Debug: Excluding '%s' because it contains '%s'\n", doc->title, node->keyword);
                    break;
                }
            } else {
                // If it’s an include keyword (e.g., cat) and not found, reject the document.
                if (!found) {
                    include_ok = 0;
                    // printf("Debug: Excluding '%s' because it lacks '%s'\n", doc->title, node->keyword);
                    break;
                }
            }
        }

        // If the document has all include keywords and no exclude keywords, add it.
        if (include_ok && exclude_ok) {
            // printf("Debug: Including '%s'\n", doc->title);
            Document *copy = CreateDocument(doc->id, doc->title, doc->body);
            if (copy) {
                copy->links = LinksInit(); // Add empty links list.
                copy->next = results; // Add to front of results.
                results = copy;
                count++;
            }
        }
    }

    return results;
}

// Set up the query history to store up to 3 queries.
void InitQueryHistory(QueryHistory *history) {
    history->front = 0;
    history->count = 0;
    for (int i = 0; i < HISTORY_CAPACITY; i++) {
        history->slots[i] = NULL; // Empty slots.
    }
}

// Add a query to the history (keeps only the last 3).
void AddQueryToHistory(QueryHistory *history, Query *query) {
    // If history is full, remove the oldest query.
    if (history->count == HISTORY_CAPACITY) {
        FreeQuery(history->slots[history->front]);
        history->slots[history->front] = NULL;
        history->front = (history->front + 1) % HISTORY_CAPACITY;
        history->count--;
    }

    // Add the new query to the end.
    int index = (history->front + history->count) % HISTORY_CAPACITY;
    history->slots[index] = query;
    history->count++;
}

// Show the last 3 queries (or "(none)" if none).
void PrintQueryHistory(const QueryHistory *history) {
    if (history->count == 0) {
        printf("(none)\n");
        return;
    }
    // Print queries from oldest to newest.
    for (int i = 0; i < history->count; i++) {
        int idx = (history->front + i) % HISTORY_CAPACITY;
        Query *q = history->slots[idx];
        for (QueryNode *node = q->head; node; node = node->next) {
            printf("%s%s", node->exclude ? "-" : "", node->keyword);
            if (node->next) printf(" ");
        }
        printf("\n");
    }
}

// Free all queries in the history.
void FreeQueryHistory(QueryHistory *history) {
    for (int i = 0; i < history->count; i++) {
        int idx = (history->front + i) % HISTORY_CAPACITY;
        FreeQuery(history->slots[idx]);
        history->slots[idx] = NULL;
    }
    history->front = 0;
    history->count = 0;
}