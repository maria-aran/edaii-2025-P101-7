#include "document.h"
#include "links.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Deserialize a document from a file path and return a Document struct
Document *document_desserialize(char *path) {

  FILE *f = fopen(path, "r"); // Open the document file in "read" mode
  if (!f)
    return NULL; // Checks that the document exists

  Document *document =
      malloc(sizeof(Document)); // Allocate memory for the document
  char buffer[262144];
  int bufferIdx = 0;
  char ch;

  // The following lines will read:

  // the ID (from the first line)
  while ((ch = fgetc(f)) != '\n')
    buffer[bufferIdx++] = ch;
  buffer[bufferIdx] = '\0';
  document->id = atoi(buffer);

  // the title (from the second line)
  bufferIdx = 0;
  while ((ch = fgetc(f)) != '\n')
    buffer[bufferIdx++] = ch;
  buffer[bufferIdx] = '\0';
  document->title = strdup(buffer);

  // Initialize the links list
  document->links = LinksInit();
  Link *links = document->links;
  // include buffersize
  char linkBuffer[64];
  int linkBufferIdx = 0;
  bool parsingLink = false;
  bufferIdx = 0;

  while ((ch = fgetc(f)) != EOF) {
    buffer[bufferIdx++] = ch;
    if (parsingLink) {
      if (ch == ')') {
        parsingLink = false;
        linkBuffer[linkBufferIdx] = '\0';
        int linkId = atoi(linkBuffer);
        Link *newLink = malloc(sizeof(Link));
        newLink->id = linkId;
        newLink->next = links->next;
        links->next = newLink;
        linkBufferIdx = 0;
      } else if (ch != '(') {
        linkBuffer[linkBufferIdx++] = ch;
      }
    } else if (ch == ']') {
      parsingLink = true;
    }
  }
  buffer[bufferIdx] = '\0';
  document->body = strdup(buffer);
  document->next = NULL;

  fclose(f);
  return document;
}

// Creates a new Document struct with provided values
Document *CreateDocument(int id, const char *title, const char *body) {
  Document *doc = malloc(sizeof(Document));
  doc->id = id;
  doc->title = strdup(title);
  doc->body = strdup(body);
  doc->links = LinksInit();
  doc->next = NULL;
  return doc;
}

// Frees all memory used by a single Document
void FreeDocument(Document *doc) {
  if (!doc)
    return;
  free(doc->title);
  free(doc->body);
  FreeLinks(doc->links);
  free(doc);
}

// Prints the full contents of a Document
void PrintDocument(Document *doc) {
  if (!doc) return;
  printf("Document ID: %d\n", doc->id);
  printf("Title: %s\n", doc->title ? doc->title : "(no title)");
  printf("Body:\n%s\n", doc->body ? doc->body : "(no body)");
  printf("Links: ");
  PrintLinks(doc->links);
  printf("\n");
}