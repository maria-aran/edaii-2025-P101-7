#ifndef GRAPH_H
#define GRAPH_H

typedef struct Node{
    int docID;
    struct Node* next;
}Node;

typedef struct{
    int numdocs; //num of documents
    Node** adjlist; // array of linked list
}Graph;

//function declarations 
Graph *creategraph(int numdocs );
int getindegree(Graph *graph, int docID);
#endif