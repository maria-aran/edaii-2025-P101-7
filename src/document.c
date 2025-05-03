#include "document.h"
#include "links.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Deserialize a document from a file path and return a Document struct
Document *document_desserialize(const char *path) {
  FILE *f = fopen(path, "r");
  if (!f) {
      printf("Cannot open file %s\n", path);
      return NULL;
  }

  Document *document = malloc(sizeof(Document));
  if (!document) {
      fclose(f);
      return NULL;
  }

  char buffer[1024];
  int bufferIdx = 0;
  int ch;

  // Read the ID (first line)
  bufferIdx = 0;
  while ((ch = fgetc(f)) != '\n' && ch != EOF) {
      if (ch != '\r') {
          buffer[bufferIdx++] = ch;
      }
  }
  buffer[bufferIdx] = '\0';
  document->id = atoi(buffer);

  // Skip any remaining \r\n characters
  if (ch == '\n') {
      ch = fgetc(f);
      if (ch == '\r') ch = fgetc(f);
  }

  // Read the title (second line)
  bufferIdx = 0;
  while ((ch = fgetc(f)) != '\n' && ch != EOF) {
      if (ch != '\r') {
          buffer[bufferIdx++] = ch;
      }
  }
  buffer[bufferIdx] = '\0';
  if (bufferIdx == 0 || buffer[0] == '\0') {
      document->title = strdup("Untitled");
      if (!document->title) {
          free(document);
          fclose(f);
          return NULL;
      }
      printf("Title was empty for file %s, set to 'Untitled'\n", path);
  } else {
      document->title = strdup(buffer);
      if (!document->title) {
          free(document);
          fclose(f);
          return NULL;
      }
  }

  // Read the body (remaining lines)
  char *body = NULL;
  size_t body_size = 0;
  size_t body_capacity = 1024;
  body = malloc(body_capacity);
  if (!body) {
      free(document->title);
      free(document);
      fclose(f);
      return NULL;
  }
  bufferIdx = 0;

  while ((ch = fgetc(f)) != EOF) {
      if (ch != '\r') {
          buffer[bufferIdx++] = ch;
      }
      if (bufferIdx >= 1023 || ch == EOF) {
          buffer[bufferIdx] = '\0';
          size_t chunk_len = strlen(buffer);
          if (body_size + chunk_len + 1 > body_capacity) {
              body_capacity *= 2;
              char *temp = realloc(body, body_capacity);
              if (!temp) {
                  free(body);
                  free(document->title);
                  free(document);
                  fclose(f);
                  return NULL;
              }
              body = temp;
          }
          strcpy(body + body_size, buffer);
          body_size += chunk_len;
          bufferIdx = 0;
      }
  }
  body[body_size] = '\0';
  document->body = body;

  document->links = LinksInit();
  if (!document->links) {
      free(document->title);
      free(document->body);
      free(document);
      fclose(f);
      return NULL;
  }
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
  if (!doc) {
      printf("Document is NULL\n");
      return;
  }
  printf("Document ID: %d\n", doc->id);
  printf("Title: %s\n", doc->title ? doc->title : "(no title)");
  printf("Body:\n%s\n", doc->body ? doc->body : "(no body)");
  printf("Links: ");
  PrintLinks(doc->links);
}