#include "word.h"
#include <stdlib.h>

Word *word_create(uint8_t *syms, uint32_t len) {
  Word *w = malloc(sizeof(Word));
  uint8_t *newSyms = calloc(len, sizeof(uint8_t));
  for (uint32_t i = 0; i < len; i++) {
    newSyms[i] = syms[i];
  }
  w->syms = newSyms;
  w->len = len;
  return w;
}

Word *word_append_sym(Word *w, uint8_t sym) {
  Word *newWord = malloc(sizeof(Word));
  uint8_t *newSyms = calloc(w->len + 1, sizeof(uint8_t));
  for (uint32_t i = 0; i < w->len; i++) {
    newSyms[i] = w->syms[i];
  }
  newSyms[w->len] = sym;

  newWord->syms = newSyms;
  newWord->len = w->len + 1;

  return newWord;
}

void word_delete(Word *w) {
  free(w->syms);
  free(w);
}

WordTable *wt_create() {
  WordTable *wt = calloc(UINT16_MAX, sizeof(Word));
  wt[1] = word_create(NULL, 0);
  return wt;
}

void wt_reset(WordTable *wt) {
  for (int i = 2; i < UINT16_MAX; i++) {
    if (wt[i] != NULL) {
      word_delete(wt[i]);
      wt[i] = NULL;
    }
  }
}

void wt_delete(WordTable *wt) {
  for (int i = 0; i < UINT16_MAX; i++) {
    if (wt[i] != NULL) {
      word_delete(wt[i]);
    }
  }
  free(wt);
}
