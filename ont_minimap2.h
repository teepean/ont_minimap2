/*
* ont_minimap2.h
*
*  Created on: August 10, 2017
*  Proprietary and confidential information of Oxford Nanopore Technologies, Limited
*  All rights reserved; (c)2017: Oxford Nanopore Technologies, Limited
*/

#ifndef ONT_MINIMAP2_H_
#define ONT_MINIMAP2_H_

#include "minimap.h"
#include "bseq.h"

#if defined(_MSC_VER)
//  Microsoft
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /// loads index and returns a pointer to it. Call ontmm_unload_index() to free  the return value.
  EXPORT mm_idx_t *ontmm_load_index(const char *index_filename);

  /** call minimap2 alignment and returns SAM string (without header!).
  *  Call free() to free the return value.
  */
  EXPORT char *ontmm_align(mm_bseq1_t query, const mm_idx_t *index);

  EXPORT void ontmm_unload_index(mm_idx_t *index);

  EXPORT char *ontmm_test() { return strdup("Hello World."); }

#ifdef __cplusplus
}
#endif

#endif /* ONT_MINIMAP2_H_ */
