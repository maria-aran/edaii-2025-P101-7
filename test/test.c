#include "../src/dataset.h"
#include "../src/document.h"
#include "../src/graph.h"
#include "../src/hashmap.h"
#include "../src/links.h"
#include "../src/query.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Counter for test numbers
int test_number = 0;
// lab 1:

// Document Parsing Functionality: Test parsing a document from a file
void test_document_desserialize() {
  // Create a temporary file with document data
  FILE *temp = fopen("temp_doc.txt", "w");
  fprintf(temp, "1\nCats\ncats\n[Link](2)");
  fclose(temp);

  // Load the document
  Document *doc = document_desserialize("temp_doc.txt");
  assert(doc != NULL);                     // Check document was loaded
  assert(doc->id == 1);                    // Check ID
  assert(strcmp(doc->title, "Cats") == 0); // Check title

  assert(strcmp(doc->body, "cats\n[Link](2)") ==
         0); // Check body (includes link line)

  assert(doc->links->next != NULL);  // Check link exists
  assert(doc->links->next->id == 2); // Check link ID
  FreeDocument(doc);

  // Clean up
  remove("temp_doc.txt");
  printf("test %d (document parsing) passed!\n", ++test_number);
}

// Documents Linked List: Test creating a document
void test_create_document() {
  Document *doc = CreateDocument(1, "Cats", "I love cats");
  assert(doc != NULL);                           // Check document was created
  assert(doc->id == 1);                          // Check ID
  assert(strcmp(doc->title, "Cats") == 0);       // Check title
  assert(strcmp(doc->body, "I love cats") == 0); // Check body
  assert(doc->links != NULL);                    // Check links list exists
  assert(doc->links->id == -1);                  // Check dummy link node
  assert(doc->next == NULL);                     // Check no next document
  FreeDocument(doc);
  printf("test %d (creating document) passed!\n", ++test_number);
}

// Documents Linked List: Test freeing a document
void test_free_document() {
  Document *doc = CreateDocument(1, "Cats", "I love cats");
  assert(doc != NULL); // Check it was created
  FreeDocument(doc);   // Free it (should not crash)
  printf("test %d (freeing document) passed!\n", ++test_number);
}

// Documents Linked List: Test linking documents
void test_link_documents() {
  Document *doc1 = CreateDocument(1, "Cats", "I love cats");
  Document *doc2 = CreateDocument(2, "Dogs", "I love dogs");
  doc1->next = doc2;           // Link documents
  assert(doc1->next != NULL);  // Check link exists
  assert(doc1->next->id == 2); // Check linked document ID
  assert(doc2->next == NULL);  // Check no further links
  FreeDocument(doc1);
  printf("test %d (linking documents) passed!\n", ++test_number);
}

// Links Linked List: Test creating a link
void test_create_link() {
  Document *doc = CreateDocument(1, "Cats", "I love cats");
  Link *link = malloc(sizeof(Link));
  link->id = 2;
  link->next = doc->links->next;
  doc->links->next = link;
  assert(doc->links->next != NULL);  // Check link exists
  assert(doc->links->next->id == 2); // Check link ID
  FreeDocument(doc);
  printf("test %d (creating link) passed!\n", ++test_number);
}

// Links Linked List: Test adding a link
void test_add_link() {
  Document *doc = CreateDocument(1, "Cats", "I love cats");

  // use LinksAppend to add two links
  LinksAppend(doc->links, 2);
  LinksAppend(doc->links, 3);

  // now the list should be: head -> 2 -> 3
  assert(doc->links->next != NULL);  // first link exists
  assert(doc->links->next->id == 2); // first link ID is 2

  assert(doc->links->next->next != NULL);  // second link exists
  assert(doc->links->next->next->id == 3); // second link ID is 3

  FreeDocument(doc);
  printf("test %d (adding link) passed!\n", ++test_number);
}

// Links Linked List: Test handling duplicate links
void test_duplicate_links() {
  Document *doc = CreateDocument(1, "Cats", "I love cats");
  Link *link1 = malloc(sizeof(Link));
  link1->id = 2;
  link1->next = NULL;
  doc->links->next = link1;
  Link *link2 = malloc(sizeof(Link));
  link2->id = 2; // Duplicate ID
  link2->next = doc->links->next;
  doc->links->next = link2;
  int count = 0;
  Link *current = doc->links->next;
  while (current) {
    if (current->id == 2)
      count++;
    current = current->next;
  }
  assert(count == 2); // Allow duplicates (no deduplication assumed)
  FreeDocument(doc);
  printf("test %d (duplicate links) passed!\n", ++test_number);
}

// Lab2:

// Linked List (QueryNode): Test creating a query
void test_create_query() {
  Query *q = CreateQuery("cat -dog");
  assert(q != NULL);       // Check query exists
  assert(q->head != NULL); // Check keywords exist
  int found_cat = 0, found_dog = 0;
  QueryNode *node = q->head;
  while (node) {
    if (strcmp(node->keyword, "cat") == 0) {
      assert(node->exclude == 0); // "cat" is include
      found_cat = 1;
    } else if (strcmp(node->keyword, "dog") == 0) {
      assert(node->exclude == 1); // "-dog" is exclude
      found_dog = 1;
    }
    node = node->next;
  }
  assert(found_cat); // Check "cat" was found
  assert(found_dog); // Check "dog" was found
  FreeQuery(q);
  printf("test %d (creating query node) passed!\n", ++test_number);
}

// Linked List (QueryNode): Test freeing a query
void test_free_query() {
  Query *q = CreateQuery("cat -dog");
  assert(q != NULL); // Check it was created
  FreeQuery(q);      // Free it (should not crash)
  printf("test %d (freeing query node) passed!\n", ++test_number);
}

// Linked List (QueryNode): Test appending a query node
void test_append_query_node() {
  Query *q = CreateQuery("cat");
  Query *temp = CreateQuery("dog");
  QueryNode *node = temp->head;
  temp->head = NULL; // Prevent freeing
  node->next = q->head;
  q->head = node;
  assert(q->head != NULL);                            // Check new node exists
  assert(strcmp(q->head->keyword, "dog") == 0);       // Check new keyword
  assert(q->head->exclude == 0);                      // Check include flag
  assert(q->head->next != NULL);                      // Check original node
  assert(strcmp(q->head->next->keyword, "cat") == 0); // Check original keyword
  FreeQuery(q);
  FreeQuery(temp);
  printf("test %d (appending query node) passed!\n", ++test_number);
}

// Linear Search Functionality: Test linear search
void test_linear_search() {
  Document *doc1 = CreateDocument(1, "cats", "I love cat.");
  Document *doc2 = CreateDocument(2, "dogs", "I love dogs, not cat.");
  Document *doc3 = CreateDocument(3, "pets", "Cats and dogs.");
  Document *doc4 = CreateDocument(4, "more cats", "Cats are great.");
  doc1->next = doc2;
  doc2->next = doc3;
  doc3->next = doc4;

  // Test query "cat"
  Query *q = CreateQuery("cat");
  Document *results = LinearSearchDocuments(doc1, q);
  assert(results != NULL); // Should have results
  int found_1 = 0, found_2 = 0, found_3 = 0, found_4 = 0;
  Document *current = results;
  while (current) {
    if (current->id == 1)
      found_1 = 1;
    if (current->id == 2)
      found_2 = 1;
    if (current->id == 3)
      found_3 = 1;
    if (current->id == 4)
      found_4 = 1;
    current = current->next;
  }
  assert(found_1); // Should include "cats"
  assert(found_2); // Should include "dogs"
  assert(found_3); // Should include "pets"
  assert(found_4); // Should include "more cats"
  FreeDocument(results);
  FreeQuery(q);

  // Test query "cat dog"
  q = CreateQuery("cat dog");
  results = LinearSearchDocuments(doc1, q);
  assert(results != NULL); // Should have results
  int cat_dog_found_3 = 0, cat_dog_count = 0;
  current = results;
  while (current) {
    if (current->id == 3)
      cat_dog_found_3 = 1;
    cat_dog_count++;
    current = current->next;
  }
  assert(cat_dog_found_3); // Should include "pets"
  FreeDocument(results);
  FreeQuery(q);

  // Test query "cat -dog"
  q = CreateQuery("cat -dog");
  results = LinearSearchDocuments(doc1, q);
  assert(results != NULL); // Should have results
  int cat_not_dog_found_1 = 0, cat_not_dog_found_4 = 0, cat_not_dog_count = 0;
  current = results;
  while (current) {
    if (current->id == 1)
      cat_not_dog_found_1 = 1;
    if (current->id == 4)
      cat_not_dog_found_4 = 1;
    cat_not_dog_count++;
    current = current->next;
  }
  assert(cat_not_dog_found_1);    // Should include "cats"
  assert(cat_not_dog_found_4);    // Should include "more cats"
  assert(cat_not_dog_count == 2); // Exactly two matches
  FreeDocument(results);
  FreeQuery(q);

  FreeDocument(doc1);
  printf("test %d (linear search) passed!\n", ++test_number);
}

// lab 3:

// Test hashmap search functionallity
void test_hashmap_search() {
  // 1) Create a small map
  HashMap *map = create_hashmap(8);
  assert(map != NULL);

  // 2) Insert (word, docID) pairs
  put_hashmap(map, "cat", 1);
  put_hashmap(map, "cat", 2);
  put_hashmap(map, "dog", 3);

  // 3) Retrieve the linked list for "cat"
  DocLinked *lst = get_list(map, "cat");
  assert(lst != NULL);

  // 4) Verify that exactly IDs 1 and 2 appear (order doesn’t matter)
  int saw1 = 0, saw2 = 0, count = 0;
  for (DocLinked *d = lst; d; d = d->next) {
    if (d->doc_id == 1)
      saw1 = 1;
    if (d->doc_id == 2)
      saw2 = 1;
    count++;
  }
  assert(count == 2);
  assert(saw1);
  assert(saw2);

  // 5) Clean up
  free_hashmap(map);
  printf("test %d (hash map search functionality) passed!\n", ++test_number);
}

// Hash map test 1
void test_hashmap_single_insert() {
  // create a small map of size 10
  HashMap *m = create_hashmap(10);

  // insert a document of ID 42 under the name apple
  put_hashmap(m, "apple", 42);

  // lookup returns a list containing exactly 42
  DocLinked *lst = get_list(m, "apple");
  assert(lst != NULL); // prove that it is not null (a key has been inserted)
  assert(lst->doc_id == 42); // the first and only doc is the one with ID 42
  assert(lst->next == NULL); // there is no next node (just one inserted)

  free_hashmap(m);
  printf("test %d (hash map) passed!\n", ++test_number);
}

// hashmap test 2: proves that eventhough
void test_hashmap_duplicate_insert() {

  HashMap *m = create_hashmap(10);

  // insert same doc twice
  put_hashmap(m, "banana", 7);
  put_hashmap(m, "banana", 7);

  // prove that it is still only one entry
  DocLinked *lst = get_list(m, "banana");
  assert(lst != NULL);
  assert(lst->doc_id == 7);
  assert(lst->next == NULL);

  free_hashmap(m);
  printf("test %d (hash map 2) passed!\n", ++test_number);
}

// hashmap test 3: verifies that a single key can correctly map to multiple
// document
void test_hashmap_multiple_keys() {

  HashMap *m = create_hashmap(10);

  // insert two different docs under same word
  put_hashmap(m, "cherry", 1);
  put_hashmap(m, "cherry", 2);

  // lookup: should see both 1 and 2 (order doesn't matter)
  int seen1 = 0, seen2 = 0, count = 0;
  for (DocLinked *p = get_list(m, "cherry"); p; p = p->next) {
    if (p->doc_id == 1)
      seen1 = 1;
    if (p->doc_id == 2)
      seen2 = 1;
    count++;
  }
  assert(seen1 && seen2);
  assert(count == 2);

  free_hashmap(m);

  printf("test %d (hash map 3) passed!\n", ++test_number);
}

// LAB 4:

// Directed Graph: Test indegree counting
void test_graph_indegree() {
  // arrange: create a graph with 5 nodes (0…4)
  DocumentGraph *g = graph_create(5);
  assert(g != NULL);

  // act: add three incoming edges into node 1
  graph_add_edge(g, 0, 1);
  graph_add_edge(g, 2, 1);
  graph_add_edge(g, 3, 1);

  // assert: node 1 has indegree 3, others 0
  assert(graph_get_indegree(g, 1) == 3);
  assert(graph_get_indegree(g, 0) == 0);
  assert(graph_get_indegree(g, 2) == 0);

  // clean up
  graph_free(g);
  printf("test %d (graph indegree) passed!\n", ++test_number);
}

void test_pagerank_uniform() {
    // arrange: graph of 3 nodes, no edges
    DocumentGraph *g = graph_create(2);
    assert(g != NULL);

    // act: ask for 0 iterations → should return the initial uniform rank
    double *r = compute_pagerank(g, 0, 0.85, 1e-6);
    assert(r != NULL);

    // assert: each of the 2 ranks is exactly 1/2

    assert(r[0]==0.5);
    assert(r[1] == 0.5);

    // clean up
    free(r);
    graph_free(g);
    printf("test %d (pagerank uniform) passed!\n", ++test_number);
}

int main() {
  printf("Running tests...\n");
  test_document_desserialize();    // Document parsing
  test_create_document();          // Documents linked list
  test_free_document();            // Documents linked list
  test_link_documents();           // Documents linked list
  test_create_link();              // Links linked list
  test_add_link();                 // Links linked list
  test_duplicate_links();          // Links linked list
  test_create_query();             // Linked list (QueryNode)
  test_free_query();               // Linked list (QueryNode)
  test_append_query_node();        // Linked list (QueryNode)
  test_linear_search();            // Linear search
  test_hashmap_search();           // Hashmap search
  test_hashmap_single_insert();    // hashmap test
  test_hashmap_duplicate_insert(); // hashmap test
  test_hashmap_multiple_keys();    // hashmap test
  test_graph_indegree();           // Graph test
  test_pagerank_uniform();

  printf("All tests passed!\n");
  return 0;
}