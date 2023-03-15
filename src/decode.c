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

static int bitlen(uint16_t val) {
  uint8_t count;
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
    printf("   Decompresses a file using Lempel-Ziv compression.\n");
    printf("USAGE\n");
    printf("./decode [OPTIONS]\n\n");
    printf("OPTIONS\n");
    printf("   -h		   Display program help and usage.\n");
    printf("   -v		   Display verbose program output.\n");
    printf("   -i infile	   Input file of data to decompress (default: "
           "stdin).\n");
    printf("   -o outfile	   Output file for decompressed data (default: "
           "stdout).\n");
    return 0;
  }

  FileHeader header;
  read_header(infile, &header);

  WordTable *table = wt_create();

  uint16_t curr_code = 0, next_code = START_CODE;
  uint8_t curr_sym = 0;

  while (read_pair(infile, &curr_code, &curr_sym, bitlen(next_code))) {
    table[next_code] = word_append_sym(table[curr_code], curr_sym);
    write_word(outfile, table[next_code]);
    if (++next_code == MAX_CODE) {
      wt_reset(table);
      next_code = START_CODE;
    }
  }
  flush_words(outfile);
  fchmod(outfile, header.protection);

  if (verbose) {
    int compSize = total_bits / 8;
    int uncompSize = total_syms;
    fprintf(stderr, "Compressed file size: %d bytes\n", compSize);
    fprintf(stderr, "Uncompressed file size: %d bytes\n", uncompSize);
    fprintf(stderr, "Compression ratio: %.2f%%\n",
            100 - ((double)compSize * 100 / (double)uncompSize));
  }

  wt_delete(table);
  close(infile);
  close(outfile);

  return 0;
}
