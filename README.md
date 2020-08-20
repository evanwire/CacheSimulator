## Project Description

In this project, I simulated a cache to evaluate various cache configurations. It will take in a memory trace file, and according to the list of memory addresses and the specified operations, will compute whether the operation results in a hit or a miss and perform the necessary actions in either case. 

### To Run

In the project directory, run:

>make
>./first \<cache size\> \<block size\> \<cache policy\> \<associativity\> \<prefetch size\> \<trace file\>

### Arguments
- <cache size> is the total size of the cache in bytes. This number should be a power of 2.
- <block size> is a power of 2 integer that specifies the size of the cache block in bytes.
- <cache policy> is valid cache policy is fifo and lru.
- <associativity> is one of:
    - direct - simulate a direct mapped cache.
    - assoc - simulate a fully associative cache.
    - assoc:n - simulate an n way associative cache. n will be a power of 2.
- <prefetch size> is the number of ajacent blocks that should be prefetched by the prefetcher in case of a miss
- <trace file> is the name of the input trace file.
  
Each line of the trace fileconsists of two columns, which are space separated. The second column
reports 48-bit memory address that has been accessed by the program while the first column
indicates whether the memory access is a read (R) or a write (W). A sample trace file can be seen below:

R 0x9cb3d40<br>
W 0x9cb3d40<br>
R 0x9cb3d44<br>
W 0x9cb3d44<br>
R 0xbf8ef498<br>
#eof<br>

### Output

The program will track 4 counters which are reads, writes, hits, and misses. It will track these for both prefetching and no prefetching. Here is what the output will look like:

no-prefetch<br>
Memory reads: 3499<br>
Memory writes: 2861<br>
Cache hits: 6501<br>
Cache misses: 3499<br>
with-prefetch<br>
Memory reads: 3521<br>
Memory writes: 2861<br>
Cache hits: 8124<br>
Cache misses: 1876<br>



