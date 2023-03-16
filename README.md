# Lempel-Ziv Compression

## Description
Lempel-Ziv Compression is the algorithm designed by Abraham Lempel and Jacob Ziv in 1978 to reach high compression ratios with files of low entropy. It works by reading each symbol from a file and giving every unique word, or string of characters, it sees a unique code. This method works since common sequences in a file can now be represented by an integer, rather than a string of characters. The code table is stored as a trie for efficient retrieval, and the compressed file consists of all the code-symbol pairs read from the original, uncompressed file.

During decompression, each of these pairs are read into the Word ADT and stored as an array of Words, which is outputted into whatever output was specified.

Since codes can range from 0 to 2^16, we can save space in compression by writing only the number of bits needed to represent the code, rather than writing 16 bits every time. Furthermore, this program performs all read/write operations with a buffer of BLOCK bytes (default is 4KB), since reads and writes are quite time expensive. Check out /src/io.h to see how these ideas are implemented.

## How to Run
1. Navigate to the program directory /bin folder, or create the binaries yourself using "make".
2. Run ./encode [OPTIONS] to compress your file. Then, uncompress files with ./decode [OPTIONS]

## Command Line Options: Encode
-v : Print compression statistics to stderr.  
-i <input> : Specify input to compress (stdin by default)  
-o <output> : Specify output of compressed input (stdout by default)  
-h : Print details on how to run the binary.

## Command Line Options: Decode
-v : Print decompression statistics to stderr.  
-i <input> : Specify input to decompress (stdin by default)  
-o <output> : Specify output of decompressed input (stdout by default)  
-h : Print details on how to run the binary.

## Example
To showcase all binaries in a simple test
```sh
echo "Lorem ipsum hello world" > message.txt
./encode -i message.txt -o out.txt -v
./decode -i out.txt -v
```
