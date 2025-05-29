#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
<<<<<<< HEAD


//query


// init query
Query *queryInit(const char *str) {

  if (!str){
    return NULL; // if input null --> return null
  }
  //Allocate the Query struct and ensure it succeeded.
  Query *q = malloc(sizeof(*q));
  assert(q);
  q->head = NULL; //initialize head to null

  char *input = strdup(str); // Duplicate the input string for in‐place tokenization
  assert(input);
  char *p = input;

  while (*p) {
    // skip spaces
    while (isspace((char)*p)) //Skip whitespace and detect end‐of‐string.
      p++;
    if (!*p)
      break;

    int exclude = 0;
    if (*p == '-') { //detect if it is an exclude word
      exclude = 1;
      p++;
    }

    // start of token
    char *start = p;
    while (*p && !isspace((char)*p)) //while whe haven't hit the end and the char ins't a space--> then p keeps increasing
      p++;
    int len = p - start; //compute lenght
    if (len <= 0)
      continue;

    char *kw = malloc(len + 1); //allocate space for the word + nullterminator
    assert(kw);
    strncpy(kw, start, len); //copy the word
    kw[len] = '\0';

    // ignore lone dashes-- >whose first character isn’t alphanumeric
    if (!isalnum(kw[0])) {
      free(kw);
      continue;
    }

    // prepend node
    QueryNode *n = malloc(sizeof(*n));
    assert(n);
    n->keyword = kw;
    n->exclude = exclude;
    n->next = q->head;
    q->head = n;
  }

  free(input);
  return q;
}


// Free a Query and all its nodes
void FreeQuery(Query *q) {
  if (!q)
    return;
  for (QueryNode *cur = q->head; cur;) {
    QueryNode *nx = cur->next;
    free(cur->keyword);
    free(cur);
    cur = nx;
  }
  free(q);
}

// Create a query from a string (e.g., "cute cat -dog").
Query *CreateQuery(const char *str) {

  if (!str) //error detection
    return NULL;

  Query *query = malloc(sizeof(Query));

  if (!query) //error detection
    return NULL;
  query->head = NULL;

  // Make a modifiable copy of the input string
  char *input = strdup(str);

  if (!input) { //error detection
    free(query);
    return NULL;
  }

  char *ptr = input;

  while (*ptr) {
    // Skip leading spaces
    while (isspace(*ptr))
      ptr++;//increase pointer so it points to next character
    if (*ptr == '\0') //if the pointer points to null-terminator --> end of the word
      break;

    int exclude = 0; // reset exclude value
    if (*ptr == '-') { // if a dash is found --> NOT --> exclude = 1
      exclude = 1;
      ptr++; //go to the next charaacter's address
=======
#include <ctype.h>

// Create a query from a string (e.g., "cute cat -dog").
Query *CreateQuery(const char *str){
    if (!str) return NULL;

    Query *query = malloc(sizeof(Query));
    if (!query) return NULL;
    query->head = NULL;

    // Make a modifiable copy of the input string
    char *input = strdup(str);
    if (!input) {
        free(query);
        return NULL;
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
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

<<<<<<< HEAD
    int len = ptr - start; //compute length end-start
    if (len == 0)
      continue; //no word if length = 0

    // Allocate and copy the keyword into a new string
    char *word = malloc(len + 1); //length + /0

    if (!word) { //error detection
      free(input);
      FreeQuery(query);
      return NULL;
    }

    strncpy(word, start, len); //copy word
    word[len] = '\0'; //manually add null-terminator
=======
        // Start of the word
        char *start = ptr;

        // Move ptr to the end of the word
        while (*ptr && !isspace(*ptr)) ptr++;
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba

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
<<<<<<< HEAD

  if (!text || !keyword || !keyword[0])//error detection
    return 0;

  // Go through the text character by character --> first we need to compute lengths
  int text_len = strlen(text);
  int keyword_len = strlen(keyword);
  int i = 0;

  while (i <= text_len - keyword_len) {
    // Check if the keyword matches at this position.
    int match = 1;
    for (int j = 0; j < keyword_len; j++) {
      if (tolower(text[i + j]) != tolower(keyword[j])) { //compares each character if they are different:
        match = 0;                                       //match = 0, so doesn't 'enter' the next loop
        break;
      }
    }

    // If we found a match, check if it’s a whole word.
    if (match) { //checking and taking into account plurals...

      // Check start: either beginning of text or not a letter/number

      int is_word_start = 1;               // assume it’s the start of a word
      if (i > 0 && isalnum((unsigned char)text[i - 1])) {
          // if we’re not at the very beginning and the previous char is alphanumeric,
          // then we’re actually in the middle of a word
          is_word_start = 0; 
      }

      // Check end: either end of text, not a letter/number, or followed by 's'
      int is_word_end = (i + keyword_len == text_len) || !isalnum(text[i + keyword_len]);

      // Allow plural: if followed by 's', check next character for word
      // boundary
      int is_plural = 0;
      if (i + keyword_len < text_len && text[i + keyword_len] == 's') {
        is_plural = 1;
        is_word_end = (i + keyword_len + 1 == text_len) || isalnum(text[i + keyword_len + 1]);
      }

      //If both start and end checks pass (or we allowed a plural), we conclude the keyword appears correctly and return 1.
      if (is_word_start && (is_word_end || is_plural)) { 
        return 1;
      }
    }

    i++; // Move to next character.
  }

  return 0; // Keyword not found.
=======
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
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
}

// Search for all documents matching the query,
// with support for OR-groups "(a|b|c)", case-insensitive,
// and simple singular/plural matching.
Document *LinearSearchDocuments(Document *docs, Query *query) {
    // Detect if we have at least one non-excluded keyword
    int has_include = 0;
    for (QueryNode *n = query->head; n; n = n->next) {
        if (!n->exclude) { has_include = 1; break; }
    }
    Document *results = NULL, *last = NULL;

   // Examine each document in the list
    for (Document *doc = docs; doc; doc = doc->next) {
        // If there were no include-terms, start with include_ok = 1
        int include_ok = has_include ? 0 : 1;
        int exclude_ok = 1;  // no forbidden terms found?

        // Check each query node
        for (QueryNode *node = query->head; node; node = node->next) {
            const char *k = node->keyword;

<<<<<<< HEAD
      // 1) OR-group
      if (!node->exclude && k[0] == '(') {
        char term[128];
        int pos = 0, any = 0;
=======
            // 1) OR-group: keywords like "(foo|bar|baz)"
            if (!node->exclude && k[0] == '(') {
                char term[128];
                int pos = 0, any = 0;
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba

                // parse between '(' and ')', splitting on '|'
                for (int i = 1; k[i] && k[i] != ')'; i++) {
                    if (k[i] == '|') {
                        term[pos] = '\0';
                        if (match_term(doc->title, term) || match_term(doc->body, term)) {
                            any = 1;
                        }
                        pos = 0;
                    } else {
                        term[pos++] = k[i];
                    }
                }
                // check last alternative
                term[pos] = '\0';
                if (match_term(doc->title, term) || match_term(doc->body, term)) {
                    any = 1;
                }

                // if none matched, reject this document
                if (!any) {
                    exclude_ok = 0;
                    break;
                }
                // count as a positive match
                include_ok = 1;
                continue;
            }

            // 2) Normal include/exclude
            int found = match_term(doc->title, k)
                     || match_term(doc->body,  k);

            if (node->exclude && found) {
                exclude_ok = 0;  // forbidden term found
                break;
            } else if (!node->exclude && found) {
                include_ok = 1;  // required term found
            }
        }

        // If doc passed both tests, append to results
        if (include_ok && exclude_ok) {
            Document *copy = CreateDocument(doc->id, doc->title, doc->body);
            copy->links = LinksInit();  // we only need id/title/body here
            copy->next = NULL;
            if (last) {
                last->next = copy;
            } else {
                results = copy;
            }
            last = copy;
        }
    }

    return results;
}

<<<<<<< HEAD

//query history

/* Initialize to empty */
void InitQueryHistory(QueryHistory *history) {

  history->front = 0;
  history->count = 0;
  for (int i = 0; i < HISTORY_CAPACITY; i++) {
    history->slots[i] = NULL;
  }
}

/* Enqueue, dropping oldest if full */
void AddQueryToHistory(QueryHistory *history, const char *queryStr) {

  if (history->count == HISTORY_CAPACITY) { //check for overflow
    free(history->slots[history->front]);   //frees memory
    history->slots[history->front] = NULL;
    history->front = (history->front + 1) % HISTORY_CAPACITY;
    history->count--;
  }

  //Actually adding query
  int idx = (history->front + history->count) % HISTORY_CAPACITY; //hystory -> front points to the slot holding the oldest entry in the buffer.
  history->slots[idx] = strdup(queryStr);                         //history->count is how many valid entries are currently stored.
  history->count++; //when we add one --> increase count

  //when count reaches capacity, the code first increments front (dropping the oldest) before inserting, so you always maintain the most recent HISTORY_CAPACITY items
}

/* Print from oldest to newest; “-” prefix for exclude terms */
void PrintQueryHistory(const QueryHistory *history) {

  if (history->count == 0) {
    printf("(none)\n"); //error detection
    return;
  }

  for (int i = 0; i < history->count; i++) {
    int idx = (history->front + i) % HISTORY_CAPACITY;
    printf("%s\n", history->slots[idx]);
  }
}

/* Free any remaining queries */
void FreeQueryHistory(QueryHistory *history) {

  for (int i = 0; i < history->count; i++) {
    int idx = (history->front + i) % HISTORY_CAPACITY;
    free(history->slots[idx]);
  }
  history->count = history->front = 0;
}



// Helper to not take into account upper and lower cases and plurals or
// singulars --> used in linear search
=======
//Helper to not take into account upper and lower cases and plurals or singulars
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
int match_term(const char *text, const char *term) {
    int text_len = strlen(text);
    int term_len = strlen(term);

    // 1) Try exact term (case‐insensitive)
    for (int i = 0; i + term_len <= text_len; i++) {
        int j;
        for (j = 0; j < term_len; j++) {
            if (tolower((unsigned char)text[i + j]) 
                != tolower((unsigned char)term[j])) {
                break;
            }
        }
        if (j == term_len) {
            return 1;
        }
    }

    // 2) If term ends in 's', also try without the 's'
    if (term_len > 1 && (term[term_len - 1] == 's' || term[term_len - 1] == 'S')) {
        int base_len = term_len - 1;
        for (int i = 0; i + base_len <= text_len; i++) {
            int j;
            for (j = 0; j < base_len; j++) {
                if (tolower((unsigned char)text[i + j]) 
                    != tolower((unsigned char)term[j])) {
                    break;
                }
            }
            if (j == base_len) {
                return 1;
            }
        }
    }
    // 3) Otherwise, try term + 's'
    else {
        int new_len = term_len + 1;
        for (int i = 0; i + new_len <= text_len; i++) {
            int j;
            for (j = 0; j < term_len; j++) {
                if (tolower((unsigned char)text[i + j]) 
                    != tolower((unsigned char)term[j])) {
                    break;
                }
            }
            if (j == term_len && tolower((unsigned char)text[i + term_len]) == 's') {
                return 1;
            }
        }
    }

    return 0;
}

<<<<<<< HEAD
//query to string (prevent segmentation error)
char *QueryToString(const Query *q) {

  // first, compute total length
  int len = 0;
  for (const QueryNode *n = q->head; n; n = n->next) {
    len += strlen(n->keyword) + 2; // for possible '-' + space
  }
  if (len == 0) {
    return strdup("");
  }
  // allocate result
  char *out = malloc(len + 1);
  char *p = out;
  for (const QueryNode *n = q->head; n; n = n->next) {
      // optional '-'
        if (n->exclude) { //if we had later marked as exclude word 
            *p++ = '-';
        }
        // finally, copy keyword character by character
        const char *kw = n->keyword;
        for (int i = 0; kw[i] != '\0'; i++) {
            *p++ = kw[i];
        }
        // c) append a space
        *p++ = ' ';
    }
  // overwrite final space with '\0'
  p[-1] = '\0';
  return out;
=======
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
>>>>>>> 1bd974b0f5cb5c033e33752f131d2032f61440ba
}