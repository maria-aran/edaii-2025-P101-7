#include "document.h"
#include "links.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 4096
#define BODY_BUF_SIZE 262144
#define LINK_BUF_SIZE 64

#define LBRACK '['
#define RBRACK ']'
#define LPAR '('
#define RPAR ')'

#define MAX_BODY 4000

// Create a new document with given id, title, and body
Document *CreateDocument(int id, const char *title, const char *body) {

  Document *doc = malloc(sizeof(Document));
  if (doc == NULL) {
    return NULL; // if failed to allocate the Document struct -->stop
  }

  // first set the id
  doc->id = id;

  // Copy title if is not empty
  if (title != NULL) {

    // Find length of title
    int len = 0;
    while (title[len] != '\0') {
      len++; // makes length greater until NULLTERMINATOR
    }

    // Allocate space for title + null terminator
    doc->title = malloc(len + 1);

    if (doc->title == NULL) {
      free(doc);
      return NULL; // Failed to allocate title
    }

    // Copy characters one by one
    for (int i = 0; i <= len; i++) {
      doc->title[i] = title[i]; // includes copying the '\0'
    }

  } else {
    doc->title = NULL;
  }

  // Copy body if provided (same process as the title with the body)

  if (body != NULL) {

    // Find length of body
    size_t len = 0;
    while (body[len] != '\0') {
      len++;
    }

    // Allocate space for body + null terminator
    doc->body = malloc(len + 1);
    if (doc->body == NULL) {
      // Clean up title and doc if body allocation fails
      if (doc->title != NULL) {
        free(doc->title);
        free(doc);
        return NULL;
      }
    }

    // Copy characters one by one
    for (size_t i = 0; i <= len; i++) {
      doc->body[i] = body[i]; // includes copying the '\0'
    }

  } else {
    doc->body = NULL;
  }

  doc->relevance = 0.0; // Initial relevance score

  doc->links = LinksInit(); // Initialize empty list of links

  doc->next = NULL; // No next document in list yet

  return doc;
}

// Free a document and its resources
void FreeDocument(Document *doc) {

  if (!doc)
    return;

  free(doc->title);
  free(doc->body);
  FreeLinks(doc->links);
  free(doc);
}

// Print a document's details
void PrintDocument(Document *doc) {

  if (!doc) {
    printf("No document to display.\n");
    return;
  }

  printf(" ▏Document ID: [%d]      ▏\n", doc->id);

  printf(" ▏Title: %s ▏\n", doc->title ? doc->title : "(no title)");
  printf("Links: ");

  PrintLinks(doc->links);

  printf("Body: ");
  if (doc->body) {
    // Print first 150 chars of body with ellipsis if longer
    if (strlen(doc->body) > 150) {
      char snippet[151];
      strncpy(snippet, doc->body, 150);
      snippet[150] = '\0';
      printf("%s\n", snippet);
    } else {
      printf("%s\n", doc->body);
    }
  } else {
    printf("(no body)\n");
  }
  printf("\n");
}

// get the corresponding information out of the datasets and classify it
Document *document_desserialize(char *path) {
  FILE *f = fopen(path, "r");
  assert(f != NULL);

  /* allocate Document */
  Document *document = malloc(sizeof(Document));
  assert(document != NULL);

  /* scratch buffer for id/title/body */
  char buffer[BODY_BUF_SIZE];
  int bufferIdx = 0;
  int ch;

  /* 1) parse ID */
  while ((ch = fgetc(f)) != EOF && ch != '\n') {
    assert(bufferIdx < BODY_BUF_SIZE);
    buffer[bufferIdx++] = (char)ch;
  }
  assert(bufferIdx < BODY_BUF_SIZE);
  buffer[bufferIdx] = '\0';
  document->id = atoi(buffer);

  /* 2) parse Title */
  bufferIdx = 0;
  while ((ch = fgetc(f)) != EOF && ch != '\n') {
    assert(bufferIdx < BODY_BUF_SIZE);
    buffer[bufferIdx++] = (char)ch;
  }
  assert(bufferIdx < BODY_BUF_SIZE);
  buffer[bufferIdx] = '\0';
  document->title = strdup(buffer);

  /* 3) parse Body + extract links */
  bufferIdx = 0;
  char linkBuffer[LINK_BUF_SIZE];
  int linkBufferIdx = 0;
  bool parsingLink = false;
  Link *links = LinksInit();

  while ((ch = fgetc(f)) != EOF) {
    /* always copy every char into body */
    assert(bufferIdx < BODY_BUF_SIZE);
    buffer[bufferIdx++] = (char)ch;

    if (parsingLink) {
      if (ch == ')') {
        /* end of link ID */
        parsingLink = false;
        assert(linkBufferIdx < LINK_BUF_SIZE);
        linkBuffer[linkBufferIdx] = '\0';
        int linkId = atoi(linkBuffer);
        LinksAppend(links, linkId);
        linkBufferIdx = 0;
      } else if (isdigit(ch)) {
        /* accumulate only digits */
        assert(linkBufferIdx < LINK_BUF_SIZE);
        linkBuffer[linkBufferIdx++] = (char)ch;
      }
      /* else skip any other char (like the '(') */
    } else if (ch == ']') {
      /* peek ahead: only start link parsing if we really see "(digits)" */
      int peek = fgetc(f);
      if (peek == '(') {
        /* copy the '(' into body too */
        assert(bufferIdx < BODY_BUF_SIZE);
        buffer[bufferIdx++] = '(';
        parsingLink = true;
      } else if (peek != EOF) {
        /* not a link: put it back so body copy is correct */
        ungetc(peek, f);
      }
    }
  }

  /* 4) finalize body string */
  assert(bufferIdx < BODY_BUF_SIZE);
  buffer[bufferIdx] = '\0';
  document->body = malloc(bufferIdx + 1);
  assert(document->body != NULL);
  strcpy(document->body, buffer);

  /* 5) attach links & default relevance */
  document->links = links;
  document->relevance = 0.0;

  fclose(f);
  return document;
}