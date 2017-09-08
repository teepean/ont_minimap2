/*
 * ont_minimap2.c
 *
 *  Created on: August 10, 2017
 *  Proprietary and confidential information of Oxford Nanopore Technologies, Limited
 *  All rights reserved; (c)2017: Oxford Nanopore Technologies, Limited
 */

#include <stdio.h>
#include <zlib.h>
#include "clone/minimap.h"
#include "clone/bseq.h"
#include "clone/kseq.h"
#include "clone/mmpriv.h"
KSEQ_INIT(gzFile, gzread)

#include "ont_minimap2.h"

static inline void str_enlarge(kstring_t *s, int l) {
  // copy from format.c:7
  if (s->l + l + 1 > s->m) {
    s->m = s->l + l + 1;
    kroundup32(s->m);
    s->s = (char*)realloc(s->s, s->m);
  }
}

static inline void str_copy(kstring_t *s, const char *st, const char *en) {
  // copy from format.c:16
  str_enlarge(s, en - st);
  memcpy(&s->s[s->l], st, en - st);
  s->l += en - st;
}

/// loads index and returns a pointer to it. Call ontmm_unload_index() to free  the return value.
mm_idx_t *ontmm_load_index(const char *index_filename) {
  // see main.c:190sq.
  FILE *fpr = fopen(index_filename, "rb");
  mm_idx_t *mi = mm_idx_load(fpr);
  fclose(fpr);
  return mi;
}

/** call minimap2 alignment and returns SAM string (without header!).
 *  Call free() to free the return value.
 */
char *ontmm_align(mm_bseq1_t query, const mm_idx_t *index) {
  // disable printing to stdout
  mm_verbose = 0;
  // see example.c:22
  mm_mapopt_t opt;
  mm_mapopt_init(&opt); // initialize mapping parameters
  mm_mapopt_update(&opt, index); // this sets the maximum minimizer occurrence
  opt.flag |= MM_F_CIGAR;
  mm_tbuf_t *tbuf = mm_tbuf_init();

  // get all hits for the query
  const mm_reg1_t *reg;
  int j, i, n_reg;
  reg = mm_map(index, query.l_seq, query.seq, &n_reg, tbuf, &opt, 0);

  // write SAM string
  kstring_t sam_string;
  sam_string.l = sam_string.m = 0, sam_string.s = 0;
  if (!n_reg) {
    char *empty_sam = "\t4\t*\t0\t0\t*\t*\t0\t0";
    str_copy(&sam_string, query.name, query.name + strlen(query.name));
    str_copy(&sam_string, empty_sam, empty_sam + strlen(empty_sam));
  }
  else
    // traverse hits and print them out
    for (j = 0; j < n_reg; ++j) {
      const mm_reg1_t *r = &reg[j];
      printf("[ont_minimap2 lib] Score: %d\n", r->score);
      if (!r->p) return strdup("ERROR:alignment has not returned cigar string.");
      kstring_t sam_line;
      sam_line.l = sam_line.m = 0, sam_line.s = 0;
      mm_write_sam(&sam_line, index, &query, r, n_reg, reg);
      printf("[ont_minimap2 lib] SAM line: %s\n", sam_line.s);
      str_copy(&sam_string, sam_line.s, sam_line.s + sam_line.l);
      char* newline = "\n";
      str_copy(&sam_string, newline, newline+1);
    }
  char* endstring = "\0";
  str_copy(&sam_string, endstring, endstring+1);
  mm_tbuf_destroy(tbuf);
  return sam_string.s;
}

void ontmm_unload_index(mm_idx_t *index) {
  mm_idx_destroy(index);
}
