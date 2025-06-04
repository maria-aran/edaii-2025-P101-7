# Report: Building a search engine like Google

## C4 component diagram showing the data structures, files read/write and their relation.
### Volatile components in blue, persistent component in pink

![alt text](image.png)


### Runtime Complexity Analysis Report Table
#### Now, we will present the runtime complexity analysis for various operations in our and search system.
| Description  | Big O | Justification |
|:------------- |:-------------:|:-------------|
| Parsing a document into the struct (document_desserialize) | O(L), where L is the length of the body     | In this function, there are no nested loops or recursive calls, in the loop `while ((ch = fgetc(f)) != EOF)`, we are reading one character at a time from the file and performing a constant amount of work for each character. For each character in the ID, title and body, the code does a fixed number of operations (checking for links, appending, copying.) Since both the ID and the title are both small, these sections are minimal since they consist of only just a few lines. Since the body is the largest part of the document, parsing the body involves reading each character in the body **once**, and since the body is much larger than the ID or title, this step dominates the time complexity |
| Parsing a query into the struct `CreateQuery(const char *str)`  | O(n) | The string is scanned once (`while(*p)`) character by character. For each word, memory is allocated and copied based on token length. All allocations and operations are linear in total size  |
| Counting the neighbours in the graph  (graph_get_indegree)   | O(d), where d is the indegree of the node   | The function simply walks the linked list of incoming edges once, performing a constant‐time increment per edge, so if there are d incoming edges, it does d steps and runs in O(d)|
|Counting the neighbours of a document in the graph (graph_get_indegree, same function, but applied specifically to a document by ID)    | O(d), where d = indegree of that document     | For a given document ID, the code again traverses its incoming‐edge list exactly once, so the time is proportional to the number of neighbours (d) with no extra nested loops. |
| Finding the documents that contain a keyword in the reverse-index `DocLinked *get_list(HashMap *m, const char *key)`     | O(k + t), where k = length of the keyword and t = size of the returned posting list | irst, the code normalizes and hashes the k characters (O(k)) to compute the bucket index, then on average visits a constant‐length chain (O(1)) to find the entry, and finally returns (or iterates) the linked list of t documents, so the total work is O(k) plus the cost to walk t nodes. |
| Finding the documents that match all keywords in the query `LinearSearchDocuments(Document *docs, Query *query) ` | O(q·n² + h²) → O(n²) if q is constant, where q = number of include‐terms, n = total documents, h = final hits| For each of the q terms, the code retrieves a posting list (worst‐case size n) in O(n) and intersects two linked lists by nested loops (O(n²)), and at the end it sorts h hits by insertion‐sort in O(h²), so overall it becomes O(q·n² + h²), which simplifies to O(n²) when q is fixed. |
| Sorting the documents according to the relevance score (sort_documents_by_relevance) | O(h²), where h = number of hits to sort   | FUsing insertion‐sort on an array of length h, each new element may shift past up to h elements in the worst case, summing to O(h²) overall. |


### Search time with/without the reverse index for the different-sized datasets
### Search time with the reverse index for the different-sized datasets
![alt text](image-3.png)

### Search time without the reverse index for the different-sized datasets
![alt text](image-4.png)

With my reverse‐index in place, query time remains essentially constant (~0.003 ms) as the dataset grows, because each lookup is O(1) and the only remaining cost is merging and sorting the small hit list (O(K + R)). In contrast, when I scan every document’s words directly, search time increases linearly (≈ 150 ms → 72 000 ms) with N, reflecting the O(N·L) work of examining every word in every document.

### Initialization time for different hashmap slot count settings 
To compute the time a simple technique was used. First we compute the load factor
a = distinct tokens / S (buckets) . And, each lookup walks a node on average, and there are two lookups, one for the body and one for the title. After some research I found out that a pointer-hop takes about 15 ns. And finally we have to add 3us for merging lists and doing insertion-sort on a few dozen hits. Now we will need to repeat the process for all datasets (12, 270, 540 and 5400), and different buckets sizes, in my case, I choose 1024, 4096, 16384 and 6536. It is also important to explain that an estimation of 225 tokens per document (200 in body + 25 in title) was done in order to produce the calculations. So:
dataset12 → 3 000 tokens, dataset270 → 60 000 tokens, dataset540 → 120 000 tokens and dataset5400 → 600 000 tokens
![alt text](image-1.png)
![alt text](image-2.png)
Finally, to conclude, as we can see clearly on the graph, a smaller slot count makes a higher load factor, which causes slower searches. For instance, at S=1024, the bucket array is too small in comparison to the number of distinct tokens, so each lookup must traverse long collision chains. We can also see that doubling the bucket size halves the lookup time. From S=1024 to S=4096, dataset270’s total time drops from 4.758 ms → 3.440 ms, and dataset5400’s from 20.578 ms → 7.395 ms. Larger datasets require larger S to reach the “flat” region. For instance, dataset12 flattens by S=4096, because 4096≫3000 tokens. While dataset 5400 still drops significantly from S=4096→ S=16384 → S=65536S (7.395 → 4.099 → 3.274 ms) because it has 600 000 tokens, so needs S≫600 000 to reach minimal hash cost. Also, once hash cost is negligible, total time approaches a constant, in our case, 3 ms (time for merging lists and doing insertion-sort on a few dozen hits). Finally, we can take as a learned lesson that to minimize search time, pick a bucket count roughly equal to (or a bit larger than) the number of distinct tokens .

### Improvement of the reverse index to improve search and initialization speed. Do we think it would it require same, less or more memory overall?. What would the runtime be? Would it take same, less or more time to run?
An improvement could be replacing the current chained‐list hash table with an open-addressing array, this way there wouldn’t be next pointers. Furthermore, store each term’s posting list in a dynamic int[] array instead of a linked DocLinked list.

This would improve the speed. Starting with initialization, we can see that it used to do the following for each token: it made a malloc for a HashNode and a DocLinked node and inserted it by following pointers. Now, it computes the hash, probes a fixed‐size bucket array, then appends the docID to a contiguous int[]. No malloc for every token. The complexity is still big-O, but the improvement is seen when constants drop sharply because it avoids thousands of small allocations and pointer chasing. 

Now, let’s look at the search time complexity. Then, it used to walk a linked list for each keyword, then merge two linked lists, O(m×n), and sort hits O(k^2). If this change was made, each lookup would probe a few array slots. Merging two posting arrays takes O(m+n) by a simple two-pointer scan, and sorting hits is O(klog⁡k), where k is the number of hits. The memory use would also change. Before the change, each HashNode had a next pointer, and each posting used a DocLinked node. If the improvement is applied, you drop all linked-list overhead (no next pointers, no DocLinked nodes). Even though buckets are a bit bigger, posting arrays use only 4 B per entry instead of 12 B. Overall, it uses less memory.