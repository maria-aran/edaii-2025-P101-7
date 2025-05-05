#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create a query from a string (e.g., "cute cat -dog").
Query *CreateQuery(const char *str) {
    if (!str) return NULL;

    Query *query = malloc(sizeof(Query));
    if (!query) return NULL;
    query->head = NULL;

    // Make a modifiable copy of the input string
    char *input = strdup(str);
    if (!input) {
        free(query);
        return NULL;
    }

    char *ptr = input;
    while (*ptr) {
        // Skip leading spaces
        while (isspace(*ptr)) ptr++;
        if (*ptr == '\0') break;

        int exclude = 0;
        if (*ptr == '-') {
            exclude = 1;
            ptr++;
        }

        // Start of the word
        char *start = ptr;

        // Move ptr to the end of the word
        while (*ptr && !isspace(*ptr)) ptr++;

        int len = ptr - start;
        if (len == 0) continue;

        // Allocate and copy the keyword into a new string
        char *word = malloc(len + 1);
        if (!word) {
            free(input);
            FreeQuery(query);
            return NULL;
        }
        strncpy(word, start, len);
        word[len] = '\0';

        // Skip invalid empty words (like just "-")
        if (!isalnum(word[0])) {
            free(word);
            continue;
        }

        // Create the query node
        QueryNode *node = malloc(sizeof(QueryNode));
        if (!node) {
            free(word);
            free(input);
            FreeQuery(query);
            return NULL;
        }

        node->keyword = word;
        node->exclude = exclude;
        node->next = query->head;
        query->head = node;
    }

    free(input);
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
            if (tolower(text[i + j]) != tolower(keyword[j])) {
                match = 0;
                break;
            }
        }

        // If we found a match, check if it’s a whole word.
       
        if (match) {
            // Check start: either beginning of text or not a letter/number
            int is_word_start = (i == 0) || !isalnum(text[i - 1]);
            // Check end: either end of text, not a letter/number, or followed by 's'
            int is_word_end = (i + keyword_len == text_len) || !isalnum(text[i + keyword_len]);
            // Allow plural: if followed by 's', check next character for word boundary
            int is_plural = 0;
            if (i + keyword_len < text_len && text[i + keyword_len] == 's') {
                is_plural = 1;
                is_word_end = (i + keyword_len + 1 == text_len) || !isalnum(text[i + keyword_len + 1]);
            }

            if (is_word_start && (is_word_end || is_plural)) {
                return 1; // Found the keyword as a whole word or plural
            }
        }

        i++; // Move to next character.
    }

    return 0; // Keyword not found.
}

// Search for up to 5 documents that match the query.
Document *LinearSearchDocuments(Document *docs, Query *query) {
    Document *results = NULL;
    Document *last = NULL;

    for (Document *doc = docs; doc; doc = doc->next) {
        int include_ok = 0;
        int exclude_ok = 1;

        for (QueryNode *node = query->head; node; node = node->next) {
            int found = string_contains(doc->title, node->keyword) ||
                        string_contains(doc->body, node->keyword);

            if (node->exclude && found) {
                exclude_ok = 0;
                break;
            } else if (!node->exclude && found) {
                include_ok = 1;
            }
        }

        if (include_ok && exclude_ok) {
            Document *copy = CreateDocument(doc->id, doc->title, doc->body);
            if (!copy) continue;
            copy->links = LinksInit();
            copy->next = NULL;

            if (!results) {
                results = copy;
                last = copy;
            } else {
                last->next = copy;
                last = copy;
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