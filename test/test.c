#include "../src/dataset.h"
#include "../src/document.h"
#include "../src/links.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === Document Tests ===
void test_document_creation() {
  runningtest("Document Creation");
  Document *doc = CreateDocument(123, "Test Title", "Test Content");

  assert(doc != NULL);
  assert(doc->id == 123);
  assert(strcmp(doc->title, "Test Title") == 0);
  assert(strcmp(doc->body, "Test Content") == 0);
  assert(doc->links != NULL);

  successtest();
  FreeDocument(doc);
}

void test_document_parsing() {
  runningtest("Document Parsing from File");

  Document *doc = document_desserialize("datasets/wikipedia12/2.txt");

  assert(doc != NULL);
  assert(doc->id > 0);
  assert(doc->title != NULL);
  assert(doc->body != NULL);
  assert(doc->links != NULL);

  successtest();
  FreeDocument(doc);
}

void test_document_list_loading() {
  runningtest("Loading Multiple Documents");

  Document *head = LoadDocumentsFromFolder("datasets/wikipedia12");
  assert(head != NULL);

  int count = 0;
  Document *curr = head;
  while (curr) {
    assert(curr->title != NULL);
    count++;
    curr = curr->next;
  }

  assert(count > 0);

  successtest();
  FreeDocumentList(head);
}

void test_document_linking() {
  runningtest("Document List Linking");

  Document *doc1 = CreateDocument(1, "Doc1", "Body1");
  Document *doc2 = CreateDocument(2, "Doc2", "Body2");

  doc1->next = doc2;

  assert(doc1->next == doc2);

  successtest();
  FreeDocument(doc1);
  FreeDocument(doc2);
}

void test_document_fields() {
  runningtest("Document Field Integrity");

  Document *doc = CreateDocument(42, "Sample Title", "Sample Body");

  assertEqualsInt(doc->id, 42);
  assertEquals(doc->title, "Sample Title");
  assertEquals(doc->body, "Sample Body");

  successtest();
  FreeDocument(doc);
}

// === Links Tests ===
void test_links_creation() {
  runningtest("Links Initialization");

  Link *links = LinksInit();
  assert(links != NULL);
  assertEqualsInt(links->id, -1);
  assert(links->next == NULL);

  successtest();
  FreeLinks(links);
}

void test_links_addition() {
  runningtest("Link Addition");

  Link *head = LinksInit();
  Link *link1 = malloc(sizeof(Link));
  link1->id = 101;
  link1->next = NULL;
  head->next = link1;

  assertEqualsInt(head->next->id, 101);
  assert(head->next->next == NULL);

  successtest();
  FreeLinks(head);
}

void test_links_chain() {
  runningtest("Multiple Links Chaining");

  Link *head = LinksInit();
  Link *l1 = malloc(sizeof(Link));
  l1->id = 1;
  Link *l2 = malloc(sizeof(Link));
  l2->id = 2;
  l1->next = l2;
  l2->next = NULL;
  head->next = l1;

  assertEqualsInt(head->next->id, 1);
  assertEqualsInt(head->next->next->id, 2);

  successtest();
  FreeLinks(head);
}

// === Entry Point ===
int main() {
  running("Document creation");
  test_document_creation();

  running("Document parsing");
  test_document_parsing();

  running("Document list loading");
  test_document_list_loading();

  running("Document linking");
  test_document_linking();

  running("Document field validation");
  test_document_fields();

  running("Links creation");
  test_links_creation();

  running("Links addition");
  test_links_addition();

  running("Links chaining");
  test_links_chain();

  allsuccess();
}