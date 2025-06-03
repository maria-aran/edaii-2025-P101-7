# Report: Building a search engine like Google

## C4 component diagram showing the data structures, files read/write and their relation.
### Volatile components in red, persistent component in blue

![alt text](image.png)














### Runtime Complexity Analysis Report Table
#### Now, we will present the runtime complexity analysis for various operations in our and search system.
| Description  | Big O | Justification |
| ------------- |:-------------:|:-------------:|
| Parsing a document into the struct (document_desserialize)    | right foo     | right foo     |
| Parsing a query into the struct (CreateQuery)     | right bar     | right bar     |
| Counting the neighbours in the graph  (graph_get_indegree)   | right baz     | right baz     |
| Counting the neighbours of a document in the graph (graph_get_indegree, same function, but applied specifically to a document by ID)    | right baz     | right baz     |
| Finding the documents that contain a keyword in the reverse-index      | right baz     | right baz     |
| Finding the documents that match all keywords in the query (LinearSearchDocuments) | right baz     | right baz     |
| Sorting the documents according to the relevance score (sort_documents_by_relevance) | right baz     | right baz     |

### Search time with/without the reverse index for the different-sized datasets
x

### Initialization time for different hashmap slot count settings 
x

### Improvement of the reverse index to improve search and initialization speed.Do we think it would it require same, less or more memory overall?. What would the runtime be? Would it take same, less or more time to run?