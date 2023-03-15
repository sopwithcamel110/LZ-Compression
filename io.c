#include "io.h"
#include "endian.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

static uint8_t symBuffer[BLOCK];
static uint8_t pairBuffer[BLOCK];

uint64_t total_syms = 0, total_bits = 0;
static uint64_t symIndex = 0, symEnd = 0, bitIndex = 0, bitEnd = 0;

int read_bytes(int infile, uint8_t *buf, int to_read) {
    int i = 0;
    while (i < to_read) {
        int n = read(infile, buf + i, to_read - i);
        if (n == -1) {
            printf("Error occurred during read!  %s", strerror(errno));
            return -1;
        }
        if (n == 0) {
            return i;
        }
        i += n;
    }
    return i;
}

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int i = 0;
    while (i < to_write) {
        int n = write(outfile, buf + i, to_write - i);
        if (n == -1) {
            printf("Error occurred during write!  %s", strerror(errno));
            return -1;
        }
        if (n == 0) {
            return i;
        }
        i += n;
    }
    return i;
}

void read_header(int infile, FileHeader *header) {
    total_bits += 8 * read_bytes(infile, (uint8_t *) header, sizeof(header));
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
    if (header->magic != MAGIC) {
        exit(EXIT_FAILURE);
    }
}

void write_header(int outfile, FileHeader *header) {
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
    total_bits += 8 * write_bytes(outfile, (uint8_t *) header, sizeof(header));
}

bool read_sym(int infile, uint8_t *sym) {
    if (symIndex >= symEnd || symEnd == 0) {
        int n = read_bytes(infile, symBuffer, BLOCK);
        if (n == 0) {
            return false;
        }
        total_syms += n;
        symIndex = 0;
        symEnd = n;
    }
    *sym = symBuffer[symIndex++];
    return true;
}
void writeValToBuffer(int outfile, int val, int len) {
    for (int i = 0; i < len; i++) {
        if (bitIndex >= BLOCK) {
            flush_pairs(outfile);
        }
        int space = bitIndex / 8;
        int bit = bitIndex % 8;
        if (val & (1 << i)) {
            pairBuffer[space] |= 1 << bit;
        } else {
            pairBuffer[space] &= ~(1 << bit);
        }
        bitIndex++;
    }
}
void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    writeValToBuffer(outfile, code, bitlen);
    writeValToBuffer(outfile, sym, 8);
}

void flush_pairs(int outfile) {
    write_bytes(outfile, pairBuffer, (bitIndex + 7) / 8);
    memset(pairBuffer, 0, (bitIndex + 1) / 8);
    total_bits += bitIndex;
    bitIndex = 0;
}

bool fillPairBuffer(int infile) {
    int n = read_bytes(infile, pairBuffer, BLOCK);
    if (n == 0) {
        return false;
    }
    memset(pairBuffer + n, 0, BLOCK - n);
    total_bits += n * 8;
    bitIndex = 0;
    bitEnd = (n * 8);
    return true;
}
bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    // Read code
    for (int i = 0; i < bitlen; i++) {
        if (bitIndex >= bitEnd || bitEnd == 0) {
            if (!fillPairBuffer(infile)) {
                return false;
            }
        }
        int space = bitIndex / 8;
        int bit = bitIndex % 8;
        if (pairBuffer[space] & (1 << bit)) {
            *code |= (1 << i);
        } else {
            *code &= ~(1 << i);
        }
        bitIndex++;
    }
    if (*code == 0) {
        return false;
    }
    // Read sym
    for (int i = 0; i < 8; i++) {
        if (bitIndex >= bitEnd) {
            if (!fillPairBuffer(infile)) {
                return false;
            }
        }
        int space = bitIndex / 8;
        int bit = bitIndex % 8;
        if (pairBuffer[space] & (1 << bit)) {
            *sym |= (1 << i);
        } else {
            *sym &= ~(1 << i);
        }
        bitIndex++;
    }
    return true;
}

void write_word(int outfile, Word *w) {
    for (uint32_t i = 0; i < w->len; i++) {
        if (symIndex >= BLOCK) {
            flush_words(outfile);
        }
        symBuffer[symIndex++] = w->syms[i];
    }
}

void flush_words(int outfile) {
    write_bytes(outfile, symBuffer, symIndex);
    memset(symBuffer, 0, BLOCK);
    total_syms += symIndex;
    symIndex = 0;
}
