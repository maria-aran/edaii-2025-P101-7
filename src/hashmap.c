#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"

// --- core: create & free --------------------------------

HashMap* create_hashmap(int size) {
    HashMap *m = malloc(sizeof(*m));
    if (!m) return NULL;
    m->size = size;
    m->table = calloc(size, sizeof(*m->table));
    if (!m->table) { free(m); return NULL; }
    return m;
}

static void _free_doclist(DocLinked *lst) {
    while (lst) {
        DocLinked *t = lst;
        lst = lst->next;
        free(t);
    }
}

static void _free_nodes(HashNode *n) {
    while (n) {
        HashNode *t = n;
        n = n->next;
        free(t->key);
        _free_doclist(t->docs);
        free(t);
    }
}

void free_hashmap(HashMap *m) {
    if (!m) return;
    for (int i = 0; i < m->size; i++) {
        _free_nodes(m->table[i]);
    }
    free(m->table);
    free(m);
}

// --- djb2 hash -----------------------------------------

unsigned int hash(const char *key, int size) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*key++))
        h = ((h << 5) + h) + c;
    return (unsigned int)(h % size);
}

// --- helpers for doc‐lists -----------------------------

static void _doclist_append(DocLinked **p, int doc_id) {
    DocLinked *n = malloc(sizeof(*n));
    n->doc_id = doc_id;
    n->next = *p;
    *p = n;
}

static DocLinked* _intersect(DocLinked *a, DocLinked *b) {
    DocLinked *out = NULL;
    for ( ; a; a = a->next )
        for ( DocLinked *x = b; x; x = x->next )
            if (a->doc_id == x->doc_id)
                _doclist_append(&out, a->doc_id);
    return out;
}

// --- normalize -----------------------------------------

void normalize_word(char *w) {
    char *o = w;
    for (char *p = w; *p; p++) {
        if (isalnum((unsigned char)*p))
            *o++ = tolower((unsigned char)*p);
    }
    *o = 0;
}

// --- build title/body indexes -------------------------

void build_reverse_indexes(Document *docs,
                           HashMap *ti,
                           HashMap *bi)
{
    char buf[1024];
    for (Document *d = docs; d; d = d->next) {
        // TITLE
        snprintf(buf, sizeof(buf), "%s", d->title ? d->title : "");
        for (char *tok = strtok(buf, " "); tok; tok = strtok(NULL, " ")) {
            normalize_word(tok);
            unsigned h = hash(tok, ti->size);
            HashNode **pp = &ti->table[h];
            while (*pp && strcmp((*pp)->key, tok) != 0)
                pp = &(*pp)->next;
            if (!*pp) {
                *pp = malloc(sizeof(**pp));
                (*pp)->key = strdup(tok);
                (*pp)->docs = NULL;
                (*pp)->next = NULL;
            }
            _doclist_append(&(*pp)->docs, d->id);
        }
        // BODY (split on whitespace)
        snprintf(buf, sizeof(buf), "%s", d->body ? d->body : "");
        for (char *tok = strtok(buf, " \t\r\n"); tok; tok = strtok(NULL, " \t\r\n")) {
            normalize_word(tok);
            unsigned h = hash(tok, bi->size);
            HashNode **pp = &bi->table[h];
            while (*pp && strcmp((*pp)->key, tok) != 0)
                pp = &(*pp)->next;
            if (!*pp) {
                *pp = malloc(sizeof(**pp));
                (*pp)->key = strdup(tok);
                (*pp)->docs = NULL;
                (*pp)->next = NULL;
            }
            _doclist_append(&(*pp)->docs, d->id);
        }
    }
}

// --- indexed search ------------------------------------

Document* search_documents_with_index(HashMap *ti,
                                      HashMap *bi,
                                      Query *q,
                                      Document *allDocs)
{
    DocLinked *res = NULL;
    int first = 1;
    for (QueryNode *n = q->head; n; n = n->next) {
        // try title‐index first
        unsigned h = hash(n->keyword, ti->size);
        HashNode *node = ti->table[h];
        while (node && strcmp(node->key, n->keyword)) node = node->next;
        DocLinked *L = node ? node->docs : NULL;
        // else body
        if (!L) {
            h = hash(n->keyword, bi->size);
            node = bi->table[h];
            while (node && strcmp(node->key, n->keyword)) node = node->next;
            L = node ? node->docs : NULL;
        }
        if (!L) { _free_doclist(res); return NULL; }
        if (first) {
            for (DocLinked *d = L; d; d = d->next)
                _doclist_append(&res, d->doc_id);
            first = 0;
        } else {
            DocLinked *t = _intersect(res, L);
            _free_doclist(res);
            res = t;
        }
    }
    // turn doc‐IDs → Document* list (up to 5)
    Document *out = NULL, *last = NULL;
    int cnt = 0;
    for (DocLinked *d = res; d && cnt < 5; d = d->next, cnt++) {
        for (Document *c = allDocs; c; c = c->next) {
            if (c->id == d->doc_id) {
                Document *dup = CreateDocument(c->id, c->title, c->body);
                if (!out) out = last = dup;
                else { last->next = dup; last = dup; }
                break;
            }
        }
    }
    _free_doclist(res);
    return out;
}

// --- snippet printer ----------------------------------

void print_snippet(const char *text, const char *kw) {
    static char lt[8192], lk[256];
    strncpy(lt, text, sizeof(lt)-1);
    for (int i = 0; lt[i]; i++) lt[i] = tolower((unsigned char)lt[i]);
    strncpy(lk, kw, sizeof(lk)-1);
    for (int i = 0; lk[i]; i++) lk[i] = tolower((unsigned char)lk[i]);

    char *p = strstr(lt, lk);
    if (!p) {
        printf("%.150s...\n", text);
        return;
    }
    int pos = p - lt;
    int start = pos > 75 ? pos - 75 : 0;
    int end = start + (int)strlen(lk) + 150;
    int tlen = strlen(text);
    if (end > tlen) end = tlen;

    // show [matched] highlighted
    printf("… %.*s[%.*s]%.*s …\n",
           pos-start, text+start,
           (int)strlen(lk), text+pos,
           end-pos-(int)strlen(lk), text+pos+strlen(lk));
}

// --- serialize / deserialize --------------------------

void serialize_index(HashMap *m, const char *filename) {
    FILE *f = fopen(filename, "w");
    for (int i = 0; i < m->size; i++) {
        for (HashNode *n = m->table[i]; n; n = n->next) {
            fprintf(f, "%s", n->key);
            for (DocLinked *d = n->docs; d; d = d->next)
                fprintf(f, " %d", d->doc_id);
            fputc('\n', f);
        }
    }
    fclose(f);
}

HashMap* deserialize_index(const char *filename, int slots) {
    HashMap *m = create_hashmap(slots);
    FILE *f = fopen(filename, "r");
    if (!f) return m;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        char *p = strtok(line, " \n");
        if (!p) continue;
        char *key = p;
        unsigned h = hash(key, m->size);
        HashNode **pp = &m->table[h];
        while (*pp && strcmp((*pp)->key, key)) pp = &(*pp)->next;
        if (!*pp) {
            *pp = malloc(sizeof(**pp));
            (*pp)->key = strdup(key);
            (*pp)->docs = NULL;
            (*pp)->next = NULL;
        }
        while ((p = strtok(NULL, " \n"))) {
            int id = atoi(p);
            _doclist_append(&(*pp)->docs, id);
        }
    }
    fclose(f);
    return m;
}
