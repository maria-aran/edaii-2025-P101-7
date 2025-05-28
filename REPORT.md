# Report: Building a search engine like Google

> [!NOTE]  
> Complete the report in this file before delivering the project.
> The lab guide has a list of the questions you must answer.


### Runtime Complexity Analysis Report Table
##### Now, we will present the runtime complexity analysis for various operations in our and search system.
| Description  | Big O | Justification |
| ------------- |:-------------:|:-------------:|
| Parsing a document into the struct (document_desserialize)    | right foo     | right foo     |
| Parsing a query into the struct      | right bar     | right bar     |
| Counting the neighbours in the graph      | right baz     | right baz     |
| Counting the neighbours of a document in the graph     | right baz     | right baz     |
| Finding the documents that contain a keyword in the reverse-index      | right baz     | right baz     |
| Finding the documents that match all keywords in the query | right baz     | right baz     |
| sorting the documents according to the relevance score | right baz     | right baz     |

### Search time with/without the reverse index for the different-sized datasets
x

### Initialization time for different hashmap slot count settings 
x

### Improvement of the reverse index to improve search and initialization speed.Do we think it would it require same, less or more memory overall?. What would the runtime be? Would it take same, less or more time to run?