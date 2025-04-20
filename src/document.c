#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "document.h"



Document *document_desserialize(char *path) {
    FILE *f = fopen(path, "r"); //Open the file 'path'
    assert(f != NULL);  //To make sure the file is openned correctly
    
    Document *document = (Document *)malloc(sizeof(Document)); //Create a dynamic array of Document structures
    
    char buffer[262144];
    int bufferSize = 262144;
    int bufferIdx = 0;
    char ch;

    // parse id
    while ((ch = fgetc(f)) != '\n') {   //Reads the first line
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;  //Converts the characters into an integer and stores it in document->id.


    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    document->id = atoi(buffer);

    // parse title
    bufferIdx=0;
    while ((ch=fgetc(f))!='\n'){
        buffer[bufferIdx++]=ch;
    }
    buffer[bufferIdx++]='\0';
    document->title=malloc(bufferIdx);
    strcpy(document->title,buffer)

    // parse body
    char linkBuffer[64];
    int linkBufferSize = 64;
    int linkBufferIdx = 0;
    bool parsingLink = false;
    Link *links = LinksInit();
    
    bufferIdx = 0;
    while ((ch = fgetc(f)) != EOF) {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
        if (parsingLink) {
            if (ch == ')') { // end of link
                parsingLink = false;
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = '\0';
                int linkId = atoi(linkBuffer);

                // add to links
                Link *newLink = (Link *)malloc(sizeof(Link));
                newLink->id = linkId;
                newLink->next = links->next;
                links->next = newLink;

                linkBufferIdx = 0;

            } else if (ch != '(') { // skip first parenthesis of the link
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = ch;
            } 
        } else if (ch == ']') { // found beginning of link id, e.g.: [my link text](123)
          parsingLink = true;
        }
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    
    char *body = (char *)malloc(sizeof(char) * bufferIdx);
    strcpy(body, buffer);

    // assign body and links to document
    document->body = body;
    document->links = links;
    
    fclose(f);
    return document;
}