#include "document.h"
#include "query.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Test document parsing
void test_document_desserialize() {
    // Assume a test file exists; replace with actual path
    Document *doc = document_desserialize("datasets/wikipedia270/sample.txt");
    assert(doc != NULL);
    assert(doc->id >= 0);
    assert(doc->title != NULL);
    assert(doc->body != NULL);
    FreeDocument(doc);
}

// Test query creation
void test_create_query() {
    Query *q = CreateQuery("cat -dog");
    assert(q != NULL);
    assert(q->head != NULL);
    
    QueryNode *node = q->head;
    assert(node != NULL);
    assert(strcmp(node->keyword, "dog") == 0);
    assert(node->exclude == 1);
    
    node = node->next;
    assert(node != NULL);
    assert(strcmp(node->keyword, "cat") == 0);
    assert(node->exclude == 0);
    
    assert(node->next == NULL);
    FreeQuery(q);
}

// Test query history
void test_query_history() {
    QueryHistory history;
    InitQueryHistory(&history);
    
    Query *q1 = CreateQuery("cat");
    Query *q2 = CreateQuery("dog");
    Query *q3 = CreateQuery("bird");
    
    AddQueryToHistory(&history, q1);
    AddQueryToHistory(&history, q2);
    AddQueryToHistory(&history, q3);
    
    assert(history.count == 3);
    assert(history.slots[history.front] == q1);
    
    Query *q4 = CreateQuery("fish");
    AddQueryToHistory(&history, q4);
    assert(history.count == 3);
    assert(history.slots[history.front] == q2); // q1 was removed
    
    FreeQueryHistory(&history);
}

// Test linear search
void test_linear_search() {
    Document *doc1 = CreateDocument(1, "Cats", "I love cats");
    Document *doc2 = CreateDocument(2, "Dogs", "I love dogs");
    doc1->next = doc2;
    
    Query *q = CreateQuery("cats -dogs");
    Document *results = LinearSearchDocuments(doc1, q);
    
    assert(results != NULL);
    assert(results->id == 1);
    assert(results->next == NULL); // Should not include doc2
    
    FreeDocumentList(results);
    FreeQuery(q);
    FreeDocumentList(doc1);
}

int main() {
    printf("Running tests...\n");
    test_document_desserialize();
    test_create_query();
    test_query_history();
    test_linear_search();
    printf("All tests passed!\n");
    return 0;
}