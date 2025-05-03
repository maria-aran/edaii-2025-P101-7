#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "query.h"

int main() {
    Document* docs = LoadDocumentsFromFile("dataset.txt");
    QueryQueue* recentQueries = InitQueryQueue(3);

    char input[256];
    while (1) {
        printf("Search> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        if (input[0] == '\n') continue;

        Query* q = CreateQuery(input);              // Crear la Query desde string
        EnqueueQuery(recentQueries, q);             // Guardar en la cola de recientes
        LinearSearchDocuments(docs, q);             // Buscar en los documentos
        PrintQueryQueue(recentQueries);             // Mostrar últimas 3 queries
        FreeQuery(q);                               // Liberar query
    }

    FreeQueryQueue(recentQueries);
    FreeDocument(docs);
    return 0;
}
