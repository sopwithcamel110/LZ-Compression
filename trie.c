#include "trie.h"
#include <stdlib.h>

TrieNode *trie_node_create(uint16_t code) {
    TrieNode *t = malloc(sizeof(TrieNode));
    for (int i = 0; i < ALPHABET; i++) {
        t->children[i] = NULL;
    }
    t->code = code;
    return t;
}

void trie_node_delete(TrieNode *n) {
    free(n);
}

TrieNode *trie_create() {
    return trie_node_create(1);
}

void trie_reset(TrieNode *root) {
    for (int i = 0; i < ALPHABET; i++) {
        if (root->children[i]) {
            trie_delete(root->children[i]);
            root->children[i] = NULL;
        }
    }
}

void trie_delete(TrieNode *n) {
    trie_reset(n);
    trie_node_delete(n);
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    return n->children[sym];
}
