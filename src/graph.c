#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

//we will use approach 2, using an array of N linked lists, fill it with NULL, 
//if the document with ID i has a link to the document with ID=j, we add i to the
//linked list stored in array[j]

//create the graph

Graph *creategraph(int numdocs ){
    Graph *graph = (Graph*)malloc(sizeof(Graph));
    graph->numdocs=numdocs;
    graph->adjlist=(Node**)malloc(numdocs*sizeof(Node*));

    for(int i=0; i<numdocs; i++){
        graph->adjlist[i]=NULL; //we fill the graph with null identifiers
    }
    return graph;
}

int getindegree(Graph *graph, int docID ){
    int num=0;
    Node *temp = graph->adjlist[docID];
    while(temp!=NULL){
        num++;
        temp=temp->next;
    }
    return num;
}

void printrelevancescore(Graph *graph){
    for(int i=0; i<graph->numdocs; i++){
        int indegree=getdegree(graph,i);
        printf("The relevance score of the document %d is %d\n",i, indegree);
    }
}