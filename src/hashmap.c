#include "hashmap.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// Do not take into account lower/upper cases, punctuation..
void normalize_word(const char *in, char *out) {
  int j = 0;
  // copy only alphanumeric, in lowercase
  for (const char *p = in; *p; ++p) {
    if (isalnum((unsigned char)*p)) {
      out[j++] = tolower((unsigned char)*p);
    }
  }
  out[j] = '\0';

  // Strip trailing 's' for simple plural handling
  if (j > 1 && out[j - 1] == 's') {
    out[j - 1] = '\0';
  }

  // Null-terminate the output
  out[j] = '\0';

  // Strip trailing 's' for basic plural normalization (e.g., "cats" -> "cat")
  int len = j;
  if (len > 1 && out[len - 1] == 's') {
    out[len - 1] = '\0';
  }
}

// DJB2 hash function
unsigned int hash_fn(const char *key, int size) {
  unsigned long h = 5381;
  int c;
  while ((c = *key++))
    h = ((h << 5) + h) + c;
  return (unsigned int)(h % size);
}

//  Create and free
HashMap *create_hashmap(int size) {

  // Allocate the HashMap structure
  HashMap *m = malloc(sizeof(*m));
  if (!m)
    return NULL;

  m->size = size;

  // Allocate the table array
  m->table = malloc(size * sizeof(*m->table));
  if (!m->table) {
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
  // iterate over the slots
  for (int i = 0; i < map->size; i++) {

    // start with the head
    HashNode *hashnode = map->table[i];

    // go throught
    while (hashnode) {
      // save next node foreve freeing the current
      HashNode *tmp = hashnode->next;
      free(hashnode->key);

      // free the whole list of document for the corresponding node
      for (DocLinked *doclinked = hashnode->docs, *nx; doclinked;
           doclinked = nx) {
        nx = doclinked->next;
        free(doclinked);
      }
      free(hashnode);
      hashnode = tmp;
    }
  }
  free(map->table);
  free(map);
}

//  Insert one
// helper that returns 1 if found, and 0 if it does not exists
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
void put_hashmap(HashMap *m, const char *key, int docId) {
  char norm[128];

  // Normalize the input word
  normalize_word(key, norm);
  if (norm[0] == '\0') {
    return;
  }

  // compute slot index via hash
  unsigned int slot = hash_fn(norm, m->size);

  // search the chain at m->table[slot] for an existing node
  HashNode *h = m->table[slot];
  while (h) {
    if (strcmp(h->key, norm) == 0) {
      // found the bucket for this word
      if (!doc_exists(h->docs, docId)) {
        // only add if not already present
        DocLinked *dl = malloc(sizeof(*dl));
        dl->doc_id = docId;
        dl->next = h->docs;
        h->docs = dl;
      }
      return; // done
    }
    h = h->next;
  }

  // if word not found then create a fresh HashNode and prepend it
  h = malloc(sizeof(*h));

  // allocate and copy the normalized key
  h->key = malloc(strlen(norm) + 1);
  strcpy(h->key, norm);

  // initialize the docs list with exactly this one docId
  h->docs = malloc(sizeof(*h->docs));
  h->docs->doc_id = docId;
  h->docs->next = NULL;

  // link into the slot’s chain
  h->next = m->table[slot];
  m->table[slot] = h;
}

// Build both indexes, for every document, extract the words form the title and
// body into hashmaps
void build_reverse_indexes(HashMap *titleIdx, HashMap *bodyIdx, Document *allDocs) {

  Document *d = allDocs;
  while (d) {
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
        }
        // if it was the end, break out
        if (c == '\0')
          break;
      } else {
        // normal character → accumulate in buf
        buf[pos++] = c;
      }
    }

    // same with the body

    text = d->body ? d->body : "";
    pos = 0; // reuse buffer

    // Same loop over the body text
    for (int i = 0;; i++) {
      char c = text[i];
      if (c == ' ' || c == '\t' || c == '\n' || c == '\0') {
        if (pos > 0) {
          buf[pos] = '\0';
          put_hashmap(bodyIdx, buf, d->id);
          pos = 0;
        }
        if (c == '\0')
          break;
      } else {
        buf[pos++] = c;
      }
    }

    // move to next document
    d = d->next;
  }
}

void serialize_reverse_index(HashMap *map, const char *filename) {
  FILE *out = fopen(filename, "w");
  if (!out)
    return;

  for (int i = 0; i < map->size; i++) {
    HashNode *n = map->table[i];
    while (n) {
      fprintf(out, "%s", n->key);
      DocLinked *d = n->docs;
      while (d) {
        fprintf(out, " %d", d->doc_id);
        d = d->next;
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
  if (!in)
    return map;

  char line[1024];
  while (fgets(line, sizeof(line), in)) {
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    // First token is the term
    char *tok = strtok(line, " ");
    if (!tok)
      continue;
    char *term = strdup(tok);

    // Remaining tokens are document IDs
    while ((tok = strtok(NULL, " "))) {
      int docId = atoi(tok);
      put_hashmap(map, term, docId);
    }

    free(term);
  }

  fclose(in);
  return map;
}

// Lookup one list by key
DocLinked *get_list(HashMap *m, const char *key) {
  char norm[128];
  normalize_word(key, norm);
  if (!norm[0])
    return NULL;
  unsigned int slot = hash_fn(norm, m->size);
  for (HashNode *h = m->table[slot]; h; h = h->next)
    if (strcmp(h->key, norm) == 0)
      return h->docs;
  return NULL;
}

// Intersection helper
// Returns a new list containing only the doc_ids that appear in *both* lists a
// and b.
DocLinked *intersect(DocLinked *a, DocLinked *b) {
  DocLinked *out = NULL; // head of our output list

  // Walk every node in list a
  for (DocLinked *pa = a; pa != NULL; pa = pa->next) {
    // For each pa, check every node in list b
    for (DocLinked *pb = b; pb != NULL; pb = pb->next) {
      if (pa->doc_id == pb->doc_id) {
        // We found a match! Make a new node at the front of out
        DocLinked *n = (DocLinked *)malloc(sizeof(DocLinked));
        n->doc_id = pa->doc_id;
        n->next = out;
        out = n;
        // Once matched, no need to keep scanning b for this pa
        break;
      }
    }
  }

  // Return the head of the intersection list (possibly NULL)
  return out;
}

// subtraction helper
DocLinked *subtract( DocLinked *base, DocLinked *rem) { // rem is list of documents you want to exclude from base
  DocLinked *out = NULL;
  for (DocLinked *b = base; b; b = b->next) {

    // if b->doc_id is *not* in the exclude list, keep it
    if (!doc_exists(rem, b->doc_id)) {
      DocLinked *n = malloc(sizeof(*n));
      n->doc_id = b->doc_id;
      n->next = out;
      out = n;
    }
  }
  return out;
}

// Convert DocLinked* → Document* list

// Given a linked list of doc IDs (`idList`) and the full list of Documents
// (`allDocs`), build and return a new linked list of freshly-allocated Document
// copies in the same order as the IDs in `idList`.
Document *make_doc_list(DocLinked *idList, Document *allDocs) {
  Document *resultHead = NULL;         // start of our new Document list
  Document **resultTail = &resultHead; // pointer-to-next-field for appending

  // For each ID in the idList...
  for (DocLinked *curId = idList; curId; curId = curId->next) {

    // Scan the full document list to find the matching Document
    for (Document *d = allDocs; d; d = d->next) {
      if (d->id == curId->doc_id) {

        // Found it: make a copy (allocates title/body anew)
        Document *copy = CreateDocument(d->id, d->title, d->body);
        copy->links = NULL; // we only need id,title,body here
        copy->next = NULL;

        // Append this copy to the end of our result list
        *resultTail = copy;
        resultTail = &copy->next;
        break; // stop scanning allDocs, move to next ID
      }
    }
  }

  return resultHead;
}


//  Search using two reverse-index hashmaps (one for titles, one for bodies).
//  Returns a newly built list of Document* that satisfy the Query q.
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

// Snippet printer(short piece of the body arounf the matching word). If no
// match, just prints the first 100 chars.
void print_snippet(const char *body, Query *q) {
  int bodylen = strlen(body);
  int kwlen = 0;
  int match = -1;

  // first we will need to find the first match of keyword in body
  for (QueryNode *n = q->head; n; n = n->next) {
    if (n->exclude)
      continue;
    kwlen = strlen(n->keyword);
    // simple case-insensitive search
    for (int i = 0; i + kwlen <= bodylen; i++) {
      int j;
      for (j = 0; j < kwlen; j++) {
        char c1 = tolower((unsigned char)body[i + j]);
        char c2 = tolower((unsigned char)n->keyword[j]);
        if (c1 != c2)
          break;
      }
      if (j == kwlen) {
        match = i;
        break;
      }
    }
    if (match >= 0)
      break;
  }

  // 2)snipped: 50 chars before, 50 after match
  int start = (match >= 0 ? match - 50 : 0);
  if (start < 0)
    start = 0;

  int end;
  if (match >= 0) {
    end = match + kwlen + 50;
    if (end > bodylen)
      end = bodylen;
  } else {
    // no match: show first 60 chars
    end = bodylen < 100 ? bodylen : 100;
  }

  // the snippet in a fixed buffer, adding “...” if we clipped
  char buf[128];
  int pos = 0;

  if (match >= 0) {
    // prefix ellipsis if we started > 0
    if (start > 0 && pos < (int)sizeof(buf) - 1) {
      buf[pos++] = '.';
      buf[pos++] = '.';
      buf[pos++] = '.';
    }
  }

  // copy the body[start..end-1] into buf
  for (int i = start; i < end && pos < (int)sizeof(buf) - 1; i++) {
    buf[pos++] = body[i];
  }

  if (match >= 0) {
    // suffix ellipsis if we didn’t reach the end
    if (end < bodylen && pos < (int)sizeof(buf) - 1) {
      buf[pos++] = '.';
      buf[pos++] = '.';
      buf[pos++] = '.';
    }
  }

  buf[pos] = '\0'; // null-terminate
  printf("%s\n", buf);
}
