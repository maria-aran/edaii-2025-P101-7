#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>  
#include <stdio.h>    

// Do not take into account lower/upper cases, punctuation..
//Takes input word in, writes a normalized version on out
void normalize_word(const char *in, char *out) {
    int j = 0;

    // copy only alphanumeric, in lowercase
    for (const char *p = in; *p; ++p) {
        if (isalnum((unsigned char)*p)) {
            out[j++] = tolower((unsigned char)*p); //turn to lower
        }
    }

    out[j] = '\0'; //at the end, put null-terminator

    // Strip trailing 's' for simple plural handling
    if (j > 1 && out[j - 1] == 's') {
        out[j - 1] = '\0'; //put where s used to go, to convert it to a singular word
    }
}


// DJB2 hash function: : returns an index in [0, size) for where to store/look up key.
unsigned int hash_fn(const char *key, int size) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*key++)) {
        h = ((h << 5) + h) + c;  // h * 33 + c
    }
    return (unsigned int)(h % size);
}

//  Create and free 
HashMap *create_hashmap(int size) {

  // Allocate the HashMap structure
  HashMap *m = malloc(sizeof(*m));
  if (!m){ //error detection
    return NULL;
  }

  //store the desired number of slotes
  m->size = size;

  // Allocate the table array
  m->table = malloc(size * sizeof(*m->table));
  if (!m->table) { //error detection
    free(m);
    return NULL;
  }

    // Initialize each slot to NULL 
    for (int i = 0; i < size; i++) {
        m->table[i] = NULL;
    }

    return m;
}

void free_hashmap(HashMap *map) {
    if (!map) return;
    //iterate over the slots
    for (int i = 0; i < map->size; i++) {

        //start with the head
        HashNode *hashnode = map->table[i];
        
        //go through the chain and free everything
        while (hashnode) {
            HashNode *tmp = hashnode->next;
            free(hashnode->key);

    // go throught
    while (hashnode) {
      HashNode *tmp = hashnode->next;
      free(hashnode->key);

      // free the whole list of document for the corresponding node
      for (DocLinked *doclinked = hashnode->docs, *nx; doclinked; doclinked = nx) {
        nx = doclinked->next;
        free(doclinked);
      }

      free(hashnode);
      hashnode = tmp;
    
    }
}
            // free the whole list of document IDs for this node
            for (DocLinked *doclinked = hashnode->docs, *nx; doclinked; doclinked = nx) {
}

//  Insert one
//helper that returns 1 if found, and 0 if it does not exist
int doc_exists(DocLinked *list, int id) {
    
    // Walk the list node by node
    while (list) {
        if (list->doc_id == id) {
            // Found a matching ID
            return 1;
        }
        list = list->next;
    }
    // Never saw that ID
    return 0;
}

// Insert a (word → docId) mapping into the hashmap.
// you either add id to an existing word entry 
// or create a brand-new word entry. 
void put_hashmap(HashMap *m, const char *key, int docId) {
    char norm[128];

<<<<<<< Updated upstream
  // Normalize the input word
  normalize_word(key, norm);
  if (norm[0] == '\0') {
    return; //if the normalization produced and empty word --> stop
  }

  // compute slot index via hash function
  unsigned int slot = hash_fn(norm, m->size);

  // search the chain at m->table[slot] for an existing node
  HashNode *h = m->table[slot];
  while (h) {

    if (strcmp(h->key, norm) == 0) { //compare the key to the normalized word

      // found the bucket for this word
      if (!doc_exists(h->docs, docId)) { //to see if this document is already recorded for that word.
        // only add if not already present
        DocLinked *dl = malloc(sizeof(*dl));
        dl->doc_id = docId;
        dl->next = h->docs;
        h->docs = dl;
      }
      return; // done
=======
    // Normalize the input word
    normalize_word(key, norm);
    if (norm[0] == '\0') {
        return; // empty after normalization
>>>>>>> Stashed changes
    }

    // compute slot index via hash
    unsigned int slot = hash_fn(norm, m->size);

    // search the chain at m->table[slot] for an existing node
    HashNode *h = m->table[slot];
    while (h) {
        if (strcmp(h->key, norm) == 0) { // found the bucket for this word
            if (!doc_exists(h->docs, docId)) { // avoid duplicates
                DocLinked *dl = malloc(sizeof(*dl));
                dl->doc_id = docId;
                dl->next   = h->docs;
                h->docs    = dl;
            }
            return; // done
        }
        h = h->next;
    }

    // word not found: create a fresh HashNode and prepend it
    h = malloc(sizeof(*h));
    h->key = malloc(strlen(norm) + 1);
    strcpy(h->key, norm);

    // initialize the docs list with this one docId
    h->docs = malloc(sizeof(*h->docs));
    h->docs->doc_id = docId;
    h->docs->next   = NULL;

    // link into the slot’s chain
    h->next         = m->table[slot];
    m->table[slot]  = h;
}

<<<<<<< Updated upstream
//reverse‐index: for each normalized word, it stores a linked list of document IDs containing that word. (optimizating the code)
// transform the collection of Documents into two hashmaps that tell you, for any given word, exactly which documents contain it
void build_reverse_indexes(HashMap *titleIdx, HashMap *bodyIdx, Document *allDocs) {

  Document *d = allDocs;
  while (d) { //iterate thought all documents

    // first we will index the title
    //  Pointer to the title string (or empty if NULL)
    const char *text = d->title ? d->title : "";
    char buf[128];
    int pos = 0;

    // Iterate throught each letter of the title
    for (int i = 0;; i++) {
      char c = text[i];

      // If we hit whitespace or end-of-string...
      if (c == ' ' || c == '\t' || c == '\n' || c == '\0') {

        if (pos > 0) {
          // terminate the token and insert it
          buf[pos] = '\0';
          put_hashmap(titleIdx, buf, d->id);
          pos = 0; // reset for next token
=======
// reverse‐index: for each normalized word, it stores a linked list of document IDs
// containing that word
void build_reverse_indexes(HashMap *titleIdx, HashMap *bodyIdx, Document *allDocs) {

    Document *d = allDocs;
    while (d) {
        // index the title
        const char *text = d->title ? d->title : "";
        char buf[128];
        int pos = 0;
        for (int i = 0;; i++) {
            char c = text[i];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\0') {
                if (pos > 0) {
                    buf[pos] = '\0';
                    put_hashmap(titleIdx, buf, d->id);
                    pos = 0;
                }
                if (c == '\0') break;
            } else {
                buf[pos++] = c;
            }
>>>>>>> Stashed changes
        }

        // same with the body
        text = d->body ? d->body : "";
        pos = 0;
        for (int i = 0;; i++) {
            char c = text[i];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\0') {
                if (pos > 0) {
                    buf[pos] = '\0';
                    put_hashmap(bodyIdx, buf, d->id);
                    pos = 0;
                }
                if (c == '\0') break;
            } else {
                buf[pos++] = c;
            }
        }

        d = d->next;
    }
}

void serialize_reverse_index(HashMap *map, const char *filename) {
    FILE *out = fopen(filename, "w");
    if (!out) return;

    for (int i = 0; i < map->size; i++) {
        HashNode *n = map->table[i];
        while (n) {
            fprintf(out, "%s", n->key);
            for (DocLinked *d = n->docs; d; d = d->next) {
                fprintf(out, " %d", d->doc_id);
            }
            fprintf(out, "\n");
            n = n->next;
        }
    }

    fclose(out);
}

HashMap *deserialize_reverse_index(const char *filename, int size) {
    HashMap *map = create_hashmap(size);
    FILE *in = fopen(filename, "r");
    if (!in) return map;

    char line[1024];
    while (fgets(line, sizeof(line), in)) {
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        char *tok = strtok(line, " ");
        if (!tok) continue;
        char *term = strdup(tok);

        while ((tok = strtok(NULL, " "))) {
            int docId = atoi(tok);
            put_hashmap(map, term, docId);
        }
        free(term);
    }

    fclose(in);
    return map;
}

DocLinked *get_list(HashMap *m, const char *key) {
    char norm[128];
    normalize_word(key, norm);
    if (!norm[0]) return NULL;
    unsigned int slot = hash_fn(norm, m->size);
    for (HashNode *h = m->table[slot]; h; h = h->next)
        if (strcmp(h->key, norm) == 0)
            return h->docs;
    return NULL;
}

DocLinked *intersect(DocLinked *a, DocLinked *b) {
    DocLinked *out = NULL;
    for (DocLinked *pa = a; pa; pa = pa->next) {
        for (DocLinked *pb = b; pb; pb = pb->next) {
            if (pa->doc_id == pb->doc_id) {
                DocLinked *n = malloc(sizeof(*n));
                n->doc_id = pa->doc_id;
                n->next   = out;
                out       = n;
                break;
            }
        }
    }
    return out;
}

DocLinked *subtract(DocLinked *base, DocLinked *rem) {
    DocLinked *out = NULL;
    for (DocLinked *b = base; b; b = b->next) {
        if (!doc_exists(rem, b->doc_id)) {
            DocLinked *n = malloc(sizeof(*n));
            n->doc_id = b->doc_id;
            n->next   = out;
            out       = n;
        }
    }
    return out;
}

Document *make_doc_list(DocLinked *idList, Document *allDocs) {
    Document *resultHead = NULL;
    Document **resultTail = &resultHead;

    for (DocLinked *curId = idList; curId; curId = curId->next) {
        for (Document *d = allDocs; d; d = d->next) {
            if (d->id == curId->doc_id) {
                Document *copy = CreateDocument(d->id, d->title, d->body);
                copy->links = NULL;
                copy->next  = NULL;
                *resultTail = copy;
                resultTail  = &copy->next;
                break;
            }
        }
    }
    return resultHead;
}

// Snippet printer(short piece of the body around the matching word). If no match,
// just prints the first 100 chars.
void print_snippet(const char *body, Query *q){
    int bodylen = strlen(body);
    int kwlen = 0;
    int match = -1;

    // first we will need to find the first match of keyword in body
    for (QueryNode *n = q->head; n; n = n->next) {
        if (n->exclude) continue;
        kwlen = strlen(n->keyword);
        for (int i = 0; i + kwlen <= bodylen; i++) {
            int j;
            for (j = 0; j < kwlen; j++) {
                char c1 = tolower((unsigned char)body[i+j]);
                char c2 = tolower((unsigned char)n->keyword[j]);
                if (c1 != c2) break;
            }
            if (j == kwlen) { match = i; break; }
        }
        if (match >= 0) break;
    }

    int start = (match >= 0 ? match - 50 : 0);
    if (start < 0) start = 0;

    int end;
    if (match >= 0) {
        end = match + kwlen + 50;
        if (end > bodylen) end = bodylen;
    } else {
        end = bodylen < 100 ? bodylen : 100;
    }

    char buf[128];
    int pos = 0;

    if (match >= 0 && start > 0 && pos < (int)sizeof(buf) - 1) {
        buf[pos++] = '.';
        buf[pos++] = '.';
        buf[pos++] = '.';
    }

    for (int i = start; i < end && pos < (int)sizeof(buf) - 1; i++) {
        buf[pos++] = body[i];
    }

<<<<<<< Updated upstream
    if (match >= 0) {
        // suffix ellipsis if we didn’t reach the end
        if (end < bodylen && pos < (int)sizeof(buf) - 1) {
            buf[pos++] = '.'; dv ,
            buf[pos++] = '.';
            buf[pos++] = '.';
        }
=======
    if (match >= 0 && end < bodylen && pos < (int)sizeof(buf) - 1) {
        buf[pos++] = '.';
        buf[pos++] = '.';
        buf[pos++] = '.';
>>>>>>> Stashed changes
    }

    buf[pos] = '\0';
    printf("%s\n", buf);
}

Document *search_documents_with_index(HashMap *titleIdx, HashMap *bodyIdx, Query *q, Document *allDocs) {
  DocLinked *res = NULL; // intermediate list of matching doc IDs
  int first = 1;

  // --- 1) INCLUDE phase ---
  for (QueryNode *n = q->head; n; n = n->next) {
    if (n->exclude)
      continue;

    // Build 'hits' for this keyword (with OR‐logic)
    DocLinked *hits = NULL;
    if (strchr(n->keyword, '|')) {
      // OR‐group: split on '|' after stripping '(' ')' and spaces
      char *kw = strdup(n->keyword), *save = kw;
      char *r = kw, *p = kw;
      while (*p) {
        if (*p != '(' && *p != ')' && *p != ' ')
          *r++ = *p;
        p++;
      }
      *r = '\0';

      char *tok = strtok(kw, "|");
      while (tok) {
        // pick title hits if present, else body hits
        DocLinked *t = get_list(titleIdx, tok);
        DocLinked *b = get_list(bodyIdx, tok);
        DocLinked *src = t ? t : b;

        // union into 'hits', avoiding duplicates
        for (DocLinked *d = src; d; d = d->next) {
          int exists = 0;
          for (DocLinked *u = hits; u; u = u->next) {
            if (u->doc_id == d->doc_id) {
              exists = 1;
              break;
            }
          }
          if (!exists) {
            DocLinked *node = malloc(sizeof(*node));
            node->doc_id = d->doc_id;
            node->next = hits;
            hits = node;
          }
        }
        tok = strtok(NULL, "|");
      }
      free(save);
    } else {
      // simple keyword: title or body
      DocLinked *t = get_list(titleIdx, n->keyword);
      hits = t ? t : get_list(bodyIdx, n->keyword);
    }

    // If nothing matches this include → empty result
    if (!hits) {
      for (DocLinked *x = res, *nx; x; x = nx) {
        nx = x->next;
        free(x);
      }
      res = NULL;
      first = 0;
      break;
    }

    if (first) {
      // seed res = copy of hits
      for (DocLinked *d = hits; d; d = d->next) {
        DocLinked *node = malloc(sizeof(*node));
        node->doc_id = d->doc_id;
        node->next = res;
        res = node;
      }
      first = 0;
    } else {
      // intersect res ∩ hits
      DocLinked *inter = intersect(res, hits);
      // free old res
      for (DocLinked *x = res, *nx; x; x = nx) {
        nx = x->next;
        free(x);
      }
      res = inter;
    }
  }

  // --- 2) If no includes at all, start with *all* docs ---
  if (first) {
    for (Document *d = allDocs; d; d = d->next) {
      DocLinked *node = malloc(sizeof(*node));
      node->doc_id = d->id;
      node->next = res;
      res = node;
    }
  }

  // --- 3) EXCLUDE phase ---
  for (QueryNode *n = q->head; n; n = n->next) {
    if (!n->exclude)
      continue;

    // Build hits list exactly as above
    DocLinked *hits = NULL;
    if (strchr(n->keyword, '|')) {
      char *kw = strdup(n->keyword), *save = kw;
      char *r = kw, *p = kw;
      while (*p) {
        if (*p != '(' && *p != ')' && *p != ' ')
          *r++ = *p;
        p++;
      }
      *r = '\0';

      char *tok = strtok(kw, "|");
      while (tok) {
        DocLinked *t = get_list(titleIdx, tok);
        DocLinked *b = get_list(bodyIdx, tok);
        DocLinked *src = t ? t : b;
        for (DocLinked *d = src; d; d = d->next) {
          int exists = 0;
          for (DocLinked *u = hits; u; u = u->next) {
            if (u->doc_id == d->doc_id) {
              exists = 1;
              break;
            }
          }
          if (!exists) {
            DocLinked *node = malloc(sizeof(*node));
            node->doc_id = d->doc_id;
            node->next = hits;
            hits = node;
          }
        }
        tok = strtok(NULL, "|");
      }
      free(save);
    } else {
      DocLinked *t = get_list(titleIdx, n->keyword);
      hits = t ? t : get_list(bodyIdx, n->keyword);
    }

    if (!hits)
      continue;

    // subtract hits from res
    DocLinked *pruned = subtract(res, hits);
    for (DocLinked *x = res, *nx; x; x = nx) {
      nx = x->next;
      free(x);
    }
    res = pruned;

    // clean up hits
    for (DocLinked *x = hits, *nx; x; x = nx) {
      nx = x->next;
      free(x);
    }
  }

  // --- 4) Convert to Document* list & final cleanup ---
  Document *out = make_doc_list(res, allDocs);
  for (DocLinked *x = res, *nx; x; x = nx) {
    nx = x->next;
    free(x);
  }
  return out;
}

 dv ,