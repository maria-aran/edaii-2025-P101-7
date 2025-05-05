#include "../src/dataset.h"
#include "../src/document.h"
#include "../src/links.h"
#include "../src/query.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Counter for test numbers
int test_number = 0;

//1.  Lab 1: Test creating a document
void test_create_document() {
    // Create a document with ID 1, title "Cats", body "I love cats"
    Document *doc = CreateDocument(1, "Cats", "I love cats");
    assert(doc != NULL); // Check document was created
    assert(doc->id == 1); // Check ID is 1
    assert(strcmp(doc->title, "Cats") == 0); // Check title is "Cats"
    assert(strcmp(doc->body, "I love cats") == 0); // Check body is correct
    assert(doc->links != NULL); // Check links list exists
    assert(doc->links->id == -1); // Check dummy link node
    assert(doc->next == NULL); // Check no next document
    FreeDocument(doc); // Free the document
    printf("test %d (creating documents) passed!\n", ++test_number);
}

//2.  Lab 1: Test freeing a document
void test_free_document() {
    // Create a document
    Document *doc = CreateDocument(1, "Cats", "I love cats");
    assert(doc != NULL); // Check it was created
    FreeDocument(doc); // Free it (should not crash)
    printf("test %d (freeing documents) passed!\n", ++test_number);
}

//3.  Lab 1: Test printing a document
void test_print_document() {
    // Create a document
    Document *doc = CreateDocument(1, "Cats", "I love cats.");
    PrintDocument(doc); // Call print (should not crash)
    FreeDocument(doc);

    // Test with a long body
    doc = CreateDocument(2, "Long", "This is a very long body that should be truncated because it exceeds 150 characters. Let’s add more text to ensure it’s long enough.");
    PrintDocument(doc); // Call print (should not crash)
    FreeDocument(doc);
    printf("test %d (printing documents) passed!\n", ++test_number);
}

//4. Lab 1: Test loading a document from a file
void test_document_desserialize() {
    // Create a temporary file with document data
    FILE *temp = fopen("temp_doc.txt", "w");
    if (!temp) {
        printf("Warning: Skipping test_document_desserialize (file creation failed)\n");
        return;
    }
    fprintf(temp, "1\nCats\nI love cats.\n[Link](2)"); // ID, title, body, link
    fclose(temp);

    // Load the document
    Document *doc = document_desserialize("temp_doc.txt");
    assert(doc != NULL); // Check document was loaded
    assert(doc->id == 1); // Check ID
    assert(strcmp(doc->title, "Cats") == 0); // Check title
    assert(strstr(doc->body, "I love cats.") != NULL); // Check body
    assert(doc->links->next != NULL); // Check link exists
    assert(doc->links->next->id == 2); // Check link ID
    FreeDocument(doc);

    // Clean up
    remove("temp_doc.txt");
    printf("test %d (loading document from file) passed!\n", ++test_number);
}

//5. Lab 1: Test loading documents from a folder
void test_load_documents_from_folder() {
    // Create a temporary folder and file
    system("mkdir -p test_temp");
    FILE *file = fopen("test_temp/doc1.txt", "w");
    if (!file) {
        printf("Warning: Skipping test_load_documents_from_folder (file creation failed)\n");
        return;
    }
    fprintf(file, "1\nCats\nI love cats.");
    fclose(file);

    // Load documents from the folder
    Document *docs = LoadDocumentsFromFolder("test_temp");
    assert(docs != NULL); // Check documents were loaded
    assert(docs->id == 1); // Check ID
    assert(strcmp(docs->title, "Cats") == 0); // Check title
    assert(strcmp(docs->body, "I love cats.") == 0); // Check body
    FreeDocumentList(docs);

    // Clean up
    system("rm -rf test_temp");
    printf("test %d (loading documents from folder) passed!\n", ++test_number);
}

//6. Lab 1: Test printing document list
void test_print_document_list() {
    // Create two documents
    Document *doc1 = CreateDocument(1, "Cats", "I love cats");
    Document *doc2 = CreateDocument(2, "Dogs", "I love dogs");
    doc1->next = doc2; // Link them
    PrintDocumentList(doc1); // Call print (should not crash)
    PrintDocumentList(NULL); // Test empty list (should not crash)
    FreeDocumentList(doc1);
    printf("test %d (printing document list) passed!\n", ++test_number);
}

//7.  Lab 1: Test freeing document list
void test_free_document_list() {
    // Create two documents
    Document *doc1 = CreateDocument(1, "Cats", "I love cats");
    Document *doc2 = CreateDocument(2, "Dogs", "I love dogs");
    doc1->next = doc2;
    FreeDocumentList(doc1); // Free list (should not crash)
    printf("test %d (freeing document list) passed!\n", ++test_number);
}

//8. Lab 2: Test creating a query
void test_create_query() {
    // Create query "cat -dog"
    Query *q = CreateQuery("cat -dog");
    assert(q != NULL); // Check query exists
    assert(q->head != NULL); // Check keywords exist

    // Check keywords (order may vary)
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

    // Test empty query
    q = CreateQuery("");
    assert(q != NULL); // Check query exists
    assert(q->head == NULL); // Check no keywords
    FreeQuery(q);
    printf("test %d (creating query) passed!\n", ++test_number);
}

//9. Lab 2: Test freeing a query
void test_free_query() {
    // Create a query
    Query *q = CreateQuery("cat -dog");
    assert(q != NULL); // Check it was created
    FreeQuery(q); // Free it (should not crash)
    printf("test %d (freeing query) passed!\n", ++test_number);
}

//10. Lab 2: Test whole-word matching
void test_string_contains() {
    // Test string_contains (must be non-static in query.c)
    extern int string_contains(const char *, const char *);
    assert(string_contains("I love cat", "cat") == 1); // Should match
    assert(string_contains("Category", "cat") == 0); // Should not match
    assert(string_contains("cat at start", "cat") == 1); // Should match
    assert(string_contains("", "cat") == 0); // Empty text
    printf("test %d (whole-word matching) passed!\n", ++test_number);
}

void test_linear_search() {
    // Create test documents with lowercase titles
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
    // Check for expected documents (ID 1, 2, 3, 4) in any order
    int found_1 = 0, found_2 = 0, found_3 = 0, found_4 = 0;
    Document *current = results;
    while (current) {
        if (current->id == 1) found_1 = 1;
        if (current->id == 2) found_2 = 1;
        if (current->id == 3) found_3 = 1;
        if (current->id == 4) found_4 = 1;
        current = current->next;
    }
    assert(found_1); // Should include "cats"
    assert(found_2); // Should include "dogs"
    assert(found_3); // Should include "pets"
    assert(found_4); // Should include "more cats"
    // Debug print to inspect results
    printf("Query 'cat' results: ");
    current = results;
    while (current) {
        printf("%d ", current->id);
        current = current->next;
    }
    printf("\n");
    FreeDocumentList(results);
    FreeQuery(q);

    // Test query "cat dog" (AND logic, both words required)
    q = CreateQuery("cat dog");
    results = LinearSearchDocuments(doc1, q);
    assert(results != NULL); // Should have results
    assert(results->id == 3); // Should include "pets"
    assert(results->next == NULL); // Only one match
    // Debug print to inspect results
    printf("Query 'cat dog' results: ");
    current = results;
    while (current) {
        printf("%d ", current->id);
        current = current->next;
    }
    printf("\n");
    FreeDocumentList(results);
    FreeQuery(q);

    // Test empty query
    q = CreateQuery("");
    results = LinearSearchDocuments(doc1, q);
    assert(results == NULL); // No results
    FreeQuery(q);

    FreeDocumentList(doc1);
    printf("test %d (linear search) passed!\n", ++test_number);
}

//12. Lab 2: Test printing search results
void test_print_search_results() {
    // Create two documents
    Document *doc1 = CreateDocument(1, "Cats", "I love cats. This is a long body to test truncation.");
    Document *doc2 = CreateDocument(2, "Pets", "Cats and dogs.");
    doc1->next = doc2;
    PrintSearchResults(doc1); // Call print (should not crash)
    PrintSearchResults(NULL); // Test empty results (should not crash)
    FreeDocumentList(doc1);
    printf("test %d (printing search results) passed!\n", ++test_number);
}

//13. Lab 2: Test query history
void test_query_history() {
    // Initialize history
    QueryHistory history;
    InitQueryHistory(&history);
    assert(history.count == 0); // Check history is empty

    // Add two queries
    Query *q1 = CreateQuery("cat");
    Query *q2 = CreateQuery("dog");
    AddQueryToHistory(&history, q1);
    AddQueryToHistory(&history, q2);
    assert(history.count == 2); // Check 2 queries
    assert(history.slots[0] == q1); // Check first is "cat"
    assert(history.slots[1] == q2); // Check second is "dog"

    // Call print (should not crash)
    PrintQueryHistory(&history);

    // Free history
    FreeQueryHistory(&history);
    assert(history.count == 0); // Check history is empty
    printf("test %d (query history) passed!\n", ++test_number);
}

int main() {
    printf("Running tests...\n");
    test_create_document();
    test_free_document();
    test_print_document();
    test_document_desserialize();
    test_load_documents_from_folder();
    test_print_document_list();
    test_free_document_list();
    test_create_query();
    test_free_query();
    test_string_contains();
    test_linear_search();
    test_print_search_results();
    test_query_history();
    printf("All tests passed!\n");
    return 0;
}