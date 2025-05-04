#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create a query from a string (e.g., "cute cat -dog")
Query *CreateQuery(const char *str) {
    Query *query = malloc(sizeof(Query));
    if (!query) return NULL;
    query->head = NULL;

    // Copy input to modify safely
    char *input = strdup(str);
    if (!input) {
        free(query);
        return NULL;
    }

    // Tokenize input by spaces
    char *token = strtok(input, " ");
    while (token) {
        int exclude = 0;
        char *keyword = token;

        // Check if keyword starts with '-'
        if (token[0] == '-') {
            exclude = 1;
            keyword = token + 1; // Skip the '-'
            if (!keyword[0]) {   // Handle case like "- " (empty keyword)
                token = strtok(NULL, " ");
                continue;
            }
        }

        // Create new query node
        QueryNode *node = malloc(sizeof(QueryNode));
        if (!node) {
            free(input);
            FreeQuery(query);
            return NULL;
        }
        node->keyword = strdup(keyword);
        node->exclude = exclude;
        node->next = query->head;
        query->head = node;

        token = strtok(NULL, " ");
    }

    free(input);
    return query;
}

// Free a query and its nodes
void FreeQuery(Query *query) {
    if (!query) return;
    QueryNode *current = query->head;
    while (current) {
        QueryNode *temp = current;
        current = current->next;
        free(temp->keyword);
        free(temp);
    }
    free(query);
}

// Check if text contains keyword as a whole word (case-sensitive)
static int string_contains(const char *text, const char *keyword) {
    if (!text || !keyword || !keyword[0]) return 0;

    size_t keyword_len = strlen(keyword);
    const char *pos = text;

    while (*pos) {
        // Find potential match
        pos = strstr(pos, keyword);
        if (!pos) return 0;

        // Check word boundaries
        int is_word_start = (pos == text) || !isalnum(*(pos - 1));
        int is_word_end = !pos[keyword_len] || !isalnum(pos[keyword_len]);

        if (is_word_start && is_word_end) {
            return 1; // Found a whole-word match
        }

        // Move to next character
        pos++;
    }

    return 0;
}

// Perform linear search: find up to 5 documents matching all include keywords and none of exclude keywords
Document *LinearSearchDocuments(Document *docs, Query *query) {
    Document *results = NULL;
    int count = 0; // Track number of results

    // Separate include and exclude keywords
    int has_include = 0;
    for (QueryNode *node = query->head; node; node = node->next) {
        if (!node->exclude) {
            has_include = 1;
            break;
        }
    }

    // Iterate through all documents
    for (Document *doc = docs; doc && count < 5; doc = doc->next) {
        int keep = 1; // Assume document is a match until proven otherwise

        // Check each query keyword
        for (QueryNode *node = query->head; node; node = node->next) {
            int found = string_contains(doc->title, node->keyword) ||
                        string_contains(doc->body, node->keyword);

            // Exclude keyword found: skip document
            if (node->exclude && found) {
                keep = 0;
                break;
            }
            // Include keyword not found (and there are include keywords): skip document
            if (!node->exclude && has_include && !found) {
                keep = 0;
                break;
            }
        }

        // If document matches criteria, add to results
        if (keep && (has_include || !query->head)) {
            Document *copy = CreateDocument(doc->id, doc->title, doc->body);
            if (copy) {
                copy->links = LinksInit(); // Initialize empty links for copy
                copy->next = results;
                results = copy;
                count++;
            }
        }
    }

    return results;
}

// Initialize query history
void InitQueryHistory(QueryHistory *history) {
    history->front = 0;
    history->count = 0;
    for (int i = 0; i < HISTORY_CAPACITY; i++) {
        history->slots[i] = NULL;
    }
}

// Add a query to history (circular buffer)
void AddQueryToHistory(QueryHistory *history, Query *query) {
    // If history is full, free the oldest query
    if (history->count == HISTORY_CAPACITY) {
        FreeQuery(history->slots[history->front]);
        history->slots[history->front] = NULL;
        history->front = (history->front + 1) % HISTORY_CAPACITY;
        history->count--;
    }

    // Add new query
    int index = (history->front + history->count) % HISTORY_CAPACITY;
    history->slots[index] = query;
    history->count++;
}

// Print query history (oldest to newest)
void PrintQueryHistory(const QueryHistory *history) {
    if (history->count == 0) {
        printf("(none)\n");
        return;
    }
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

// Free query history
void FreeQueryHistory(QueryHistory *history) {
    for (int i = 0; i < history->count; i++) {
        int idx = (history->front + i) % HISTORY_CAPACITY;
        FreeQuery(history->slots[idx]);
        history->slots[idx] = NULL;
    }
    history->front = 0;
    history->count = 0;
}