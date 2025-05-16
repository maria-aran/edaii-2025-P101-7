// src/hashmap.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"

// ---- helpers for DocLinked lists ----

static int doc_id_exists(DocLinked *list, int id) {
    for (; list; list = list->next)
        if (list->doc_id == id)
            return 1;
    return 0;
}

static DocLinked *copy_list(DocLinked *in) {
    DocLinked *out = NULL;
    for (DocLinked *p = in; p; p = p->next) {
        DocLinked *n = malloc(sizeof *n);
        n->doc_id = p->doc_id;
        n->next   = out;
        out       = n;
    }
    return out;
}

// intersection = docs containing *all* include keywords
static DocLinked *intersect_lists(DocLinked *a, DocLinked *b) {
    DocLinked *r = NULL;
    for (DocLinked *p = a; p; p = p->next) {
        if (doc_id_exists(b, p->doc_id)) {
            DocLinked *n = malloc(sizeof *n);
            n->doc_id = p->doc_id;
            n->next   = r;
            r         = n;
        }
    }
    return r;
}

// union = docs containing *any* exclude keyword
static DocLinked *union_lists(DocLinked *a, DocLinked *b) {
    DocLinked *r = copy_list(a);
    for (DocLinked *p = b; p; p = p->next) {
        if (!doc_id_exists(r, p->doc_id)) {
            DocLinked *n = malloc(sizeof *n);
            n->doc_id = p->doc_id;
            n->next   = r;
            r         = n;
        }
    }
    return r;
}

// difference = include minus exclude
static DocLinked *difference_lists(DocLinked *incl, DocLinked *excl) {
    DocLinked *r = NULL;
    for (DocLinked *p = incl; p; p = p->next) {
        if (!doc_id_exists(excl, p->doc_id)) {
            DocLinked *n = malloc(sizeof *n);
            n->doc_id = p->doc_id;
            n->next   = r;
            r         = n;
        }
    }
    return r;
}

// ---- tokenization & normalization ----
// splits on spaces, lowercases & strips non-alphanumeric
static void index_text(HashMap *map, const char *text, int doc_id) {
    char *copy = strdup(text);
    char *tok  = strtok(copy, " \t\r\n");
    while (tok) {
        char norm[256];
        int  j = 0;
        for (int i = 0; tok[i] && j + 1 < (int)sizeof(norm); i++) {
            if (isalnum((unsigned char)tok[i])) {
                norm[j++] = tolower((unsigned char)tok[i]);
            }
        }
        norm[j] = '\0';
        if (j > 0) put(map, norm, doc_id);
        tok = strtok(NULL, " \t\r\n");
    }
    free(copy);
}

// ---- public API ----

HashMap *create_hashmap(int size) {
    HashMap *m = malloc(sizeof *m);
    if (!m) return NULL;
    m->size  = size;
    m->table = calloc(size, sizeof *m->table);
    if (!m->table) { free(m); return NULL; }
    return m;
}

void free_hashmap(HashMap *m) {
    if (!m) return;
    for (int i = 0; i < m->size; i++) {
        for (HashNode *n = m->table[i]; n; ) {
            HashNode *tmp = n;
            n = n->next;
            free(tmp->key);
            for (DocLinked *d = tmp->docs; d; ) {
                DocLinked *td = d;
                d = d->next;
                free(td);
            }
            free(tmp);
        }
    }
    free(m->table);
    free(m);
}

int hash(const char *key, int size) {
    unsigned long h = 5381;
    int           c;
    while ((c = (unsigned char)*key++))
        h = ((h << 5) + h) + c;
    return (int)(h % size);
}

void put(HashMap *m, const char *key, int doc_id) {
    int idx = hash(key, m->size);
    for (HashNode *n = m->table[idx]; n; n = n->next) {
        if (strcmp(n->key, key) == 0) {
            if (!doc_id_exists(n->docs, doc_id)) {
                DocLinked *d = malloc(sizeof *d);
                d->doc_id = doc_id;
                d->next   = n->docs;
                n->docs   = d;
            }
            return;
        }
    }
    // new bucket
    HashNode *n    = malloc(sizeof *n);
    n->key         = strdup(key);
    n->docs        = malloc(sizeof *n->docs);
    n->docs->doc_id = doc_id;
    n->docs->next   = NULL;
    n->next         = m->table[idx];
    m->table[idx]   = n;
}

DocLinked *get(HashMap *m, const char *key) {
    int idx = hash(key, m->size);
    for (HashNode *n = m->table[idx]; n; n = n->next) {
        if (strcmp(n->key, key) == 0)
            return n->docs;
    }
    return NULL;
}

int contains(HashMap *m, const char *key) {
    return get(m, key) != NULL;
}

void build_reverse_index(HashMap *m, Document *docs) {
    for (Document *d = docs; d; d = d->next) {
        index_text(m, d->title ? d->title : "", d->id);
        index_text(m, d->body  ? d->body  : "", d->id);
    }
}

Document *search_documents_with_index(HashMap *m,
                                      Query   *q,
                                      Document *all_docs)
{
    // 1) gather include-list intersection
    DocLinked *inc = NULL;
    for (QueryNode *n = q->head; n; n = n->next) {
        if (!n->exclude) {
            DocLinked *list = get(m, n->keyword);
            if (!list) { inc = NULL; break; }
            inc = inc ? intersect_lists(inc, list) : copy_list(list);
        }
    }
    if (!inc) return NULL;

    // 2) gather exclude-list union
    DocLinked *exc = NULL;
    for (QueryNode *n = q->head; n; n = n->next) {
        if (n->exclude) {
            DocLinked *list = get(m, n->keyword);
            if (list) exc = exc ? union_lists(exc, list) : copy_list(list);
        }
    }

    // 3) final candidates = inc minus exc (if any)
    DocLinked *cands = exc ? difference_lists(inc, exc) : inc;

    // 4) build resulting Document* list in original order
    Document *res = NULL, *last = NULL;
    for (Document *d = all_docs; d; d = d->next) {
        if (doc_id_exists(cands, d->id)) {
            Document *c = CreateDocument(d->id, d->title, d->body);
            c->next = NULL;
            if (!res) res = last = c;
            else {
                last->next = c;
                last = c;
            }
        }
    }
    return res;
}
