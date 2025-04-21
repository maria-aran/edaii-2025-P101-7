#include "sample_lib_test.c"
#include "utils.h"
#include <stdio.h>
#include <assert.h>
#include "../src/document.h"
#include <string.h>

int main() {
  {
    // Call all test modules you want to run here
    sample_lib_test();
  }
  allsuccess();
}


void test_document_creation() {
    Document* doc = CreateDocument(123, "Test Title", "Test Content");

    assert(doc != NULL);
    assert(doc->id == 123);
    assert(strcmp(doc->title, "Test Title") == 0);
    assert(strcmp(doc->body, "Test Content") == 0);

    FreeDocument(doc);
}
