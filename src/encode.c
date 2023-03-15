#include "io.h"
#include "trie.h"
#include "word.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"
#define MAX_CODE 0xFFFF
#define START_CODE 2
#define STOP_CODE 0

// Return the number of bits needed to represent val.
static int bitlen(uint16_t val) {
  int count;
  for (count = 0; val != 0; count++) {
    val >>= 1;
  }
  return count;
}

int main(int argc, char **argv) {
  int infile = 0;
  int outfile = 1;
  int verbose = 0, help = 0;
  int opt = 0;
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'i':
      infile = open(optarg, O_RDONLY);
      if (!infile) {
        printf("Error occurred opening: %s", optarg);
        return 1;
      }
      break;
    case 'o':
      outfile = open(optarg, O_CREAT | O_WRONLY | O_TRUNC);
      if (!outfile) {
        printf("Error occurred opening: %s", optarg);
        return 1;
      }
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      help = 1;
      break;
    }
  }
  if (help) {
    printf("SYNOPSIS\n");
    printf("   Compresses a file using Lempel-Ziv compression.\n");
    printf("USAGE\n");
    printf("./encode [OPTIONS]\n\n");
    printf("OPTIONS\n");
    printf("   -h		   Display program help and usage.\n");
    printf("   -v		   Display verbose program output.\n");
    printf(
        "   -i infile	   Input file of data to compress (default: stdin).\n");
    printf("   -o outfile	   Output file for compressed data (default: "
           "stdout).\n");
    return 0;
  }
  FileHeader header;
  memset(&header, 0,
         sizeof(header)); // Ensure all bits at &header are set to 0.
  struct stat sb;
  fstat(infile, &sb);

  header.magic = MAGIC;
  header.protection = sb.st_mode;
  write_header(outfile, &header);

  TrieNode *root = trie_create();
  TrieNode *curr_node = root;
  TrieNode *prev_node = NULL;

  uint8_t curr_sym = 0;
  uint8_t prev_sym = 0;
  uint16_t next_code = START_CODE;

  while (read_sym(infile, &curr_sym)) {
    TrieNode *next_node = trie_step(curr_node, curr_sym);
    if (next_node) {
      prev_node = curr_node;
      curr_node = next_node;
    } else {
      write_pair(outfile, curr_node->code, curr_sym, bitlen(next_code));
      curr_node->children[curr_sym] = trie_node_create(next_code);
      curr_node = root;
      next_code++;
    }
    if (next_code == MAX_CODE) {
      trie_reset(root);
      curr_node = root;
      next_code = START_CODE;
    }
    prev_sym = curr_sym;
  }
  // If the current node is not root, we are still writing a pair.
  if (curr_node != root) {
    write_pair(outfile, prev_node->code, prev_sym, bitlen(next_code));
    next_code = (next_code + 1) % MAX_CODE;
  }
  write_pair(outfile, STOP_CODE, 0, bitlen(next_code));
  flush_pairs(outfile);

  if (verbose) {
    int compSize = total_bits / 8;
    int uncompSize = total_syms;
    fprintf(stderr, "Compressed file size: %d bytes\n", compSize);
    fprintf(stderr, "Uncompressed file size: %d bytes\n", uncompSize);
    fprintf(stderr, "Compression ratio: %.2f%%\n",
            100 - ((double)compSize * 100 / (double)uncompSize));
  }

  trie_delete(root);
  close(infile);
  close(outfile);

  return 0;
}
