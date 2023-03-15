# Lempel-Ziv Compression

## Description
Code to compress and decompress data using LZ Compression.

## How to Run
1. Navigate to the program directory.
2. Run "make" to create all binaries, or "make EXEC" where EXEC is the binary you would like to create.
3. Run ./encode [OPTIONS] to compress your file. Then, uncompress files with ./decode [OPTIONS]

## Command Line Options: Encode
-v : Print compression statistics to stderr.  
-i <input> : Specify input to compress (stdin by default)  
-o <output> : Specify output of compressed input (stdout by default)  

## Command Line Options: Decode
-v : Print decompression statistics to stderr.  
-i <input> : Specify input to decompress (stdin by default)  
-o <output> : Specify output of decompressed input (stdout by default)  

## Example
To showcase all binaries in a simple test
```sh
echo "Lorem ipsum hello world" > message.txt
./encode -i message.txt -o out.txt -v
./decode -i out.txt -v
```
