# My own implementation of Huffman codes in C

### Description
Huffman encoding provides the most efficient way to compress data, from a theoretical point of view. It is a lossless compression algorithm, which means that the original data can be perfectly reconstructed from the compressed data.

### How to use
To properly test the algorithm, you can use the following commands:

```bash
$ ./HuffmanCodes encode text.txt huffman.huff
$ ./HuffmanCodes decode huffman.huff 
$ ls | grep "decoded.txt"
decoded.txt
```
Keep in mind, that instead of creating binary file, im creating "decoded.txt" file, which is not efficient in terms of compressing data, but it shows that the algorithm works properly.

### Resources
To understand the Huffman coding algorithm, I used the following resources:

https://www.youtube.com/watch?v=B3y0RsVCyrw

https://www.youtube.com/watch?v=lAyhKaclsPM

https://www.geeksforgeeks.org/priority-queue-set-1-introduction/

https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/