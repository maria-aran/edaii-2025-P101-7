# Report: Building a search engine like Google

## C4 component diagram showing the data structures, files read/write and their relation.
### Volatile components in red, persistent component in blue

![alt text](image.png)














### Runtime Complexity Analysis Report Table
#### Now, we will present the runtime complexity analysis for various operations in our and search system.
| Description  | Big O | Justification |
| ------------- |:-------------:|:-------------:|
| Parsing a document into the struct (document_desserialize)    | O(L), where L is the length of the body     | In this function, there are no nested loops or recursive calls, in the loop while ((ch = fgetc(f)) != EOF), we are reading one character at a time from the file and performing a constant amount of work for each character. For each character in the ID, title and body, the code does a fixed number of operations (checking for links, appending, copying.) Since both the ID and the title are both small, these sections are minimal since they consist of only just a few lines. Since the body is the largest part of the document, parsing the body involves reading each character in the body **once**, and since the body is much larger than the ID or title, this step dominates the time complexity |
| Parsing a query into the struct (CreateQuery)     | right bar     | right bar     |
| Counting the neighbours in the graph  (graph_get_indegree)   | right baz     | right baz     |
| Counting the neighbours of a document in the graph (graph_get_indegree, same function, but applied specifically to a document by ID)    | O(n), where n is the number of nodes in the linked list     | The while loop runs once per node in the list, thus, n iterations. There is also no nested loops or recursion, so that is the final complexity |
| Finding the documents that contain a keyword in the reverse-index      | right baz     | right baz     |
| Finding the documents that match all keywords in the query (LinearSearchDocuments) | right baz     | right baz     |
| Sorting the documents according to the relevance score (sort_documents_by_relevance) | O(n^2)    | For each document in the list, it finds the correct place in the sorted portion where the document should go (based on the revelance score), and then, inserts the document.Each insertion takes up to O(n) time, and we do this for O(n) documents, thus, the total time complexity is O(n) (insertions) times O(n) (scans) = O    |

### Search time with/without the reverse index for the different-sized datasets
x

### Initialization time for different hashmap slot count settings 
x

### Improvement of the reverse index to improve search and initialization speed.Do we think it would it require same, less or more memory overall?. What would the runtime be? Would it take same, less or more time to run?