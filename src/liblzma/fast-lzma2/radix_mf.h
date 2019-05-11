/*
* Copyright (c) 2018, Conor McCarthy
* All rights reserved.
*
* This source code is licensed under both the BSD-style license (found in the
* LICENSE file in the root directory of this source tree) and the GPLv2 (found
* in the COPYING file in the root directory of this source tree).
* You may select, at your option, one of the above-listed licenses.
*/

#ifndef RADIX_MF_H
#define RADIX_MF_H


#include "data_block.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct FL2_matchTable_s FL2_matchTable;
typedef struct RMF_builder_s RMF_builder;

#define OVERLAP_FROM_DICT_SIZE(d, o) (((d) >> 4) * (o))

#define RMF_MIN_BYTES_PER_THREAD 1024

#define RMF_DEFAULT_BUF_RESIZE 2

typedef struct
{
    size_t dictionary_size;
    unsigned match_buffer_resize;
    unsigned overlap_fraction;
    unsigned divide_and_conquer;
    unsigned depth;
#ifdef RMF_REFERENCE
    unsigned use_ref_mf;
#endif
} RMF_parameters;

typedef struct
{
	uint32_t length;
	uint32_t dist;
} RMF_match;

FL2_matchTable* RMF_createMatchTable(const RMF_parameters* const params, size_t const dict_reduce);
void RMF_freeMatchTable(FL2_matchTable* const tbl);
uint8_t RMF_compatibleParameters(const FL2_matchTable* const tbl, const RMF_parameters* const params, size_t const dict_reduce);
lzma_ret RMF_applyParameters(FL2_matchTable* const tbl, const RMF_parameters* const params, size_t const dict_reduce);
RMF_builder* RMF_createBuilder(FL2_matchTable* const tbl, RMF_builder *existing);
void RMF_initProgress(FL2_matchTable * const tbl);
void RMF_initTable(FL2_matchTable* const tbl, const void* const data, size_t const end);
int RMF_buildTable(FL2_matchTable* const tbl,
	RMF_builder* const builder,
    int const thread,
    lzma_data_block const block);
void RMF_cancelBuild(FL2_matchTable* const tbl);
void RMF_resetIncompleteBuild(FL2_matchTable* const tbl);
int RMF_integrityCheck(const FL2_matchTable* const tbl, const uint8_t* const data, size_t const pos, size_t const end, unsigned const max_depth);
void RMF_limitLengths(FL2_matchTable* const tbl, size_t const pos);
uint8_t* RMF_getTableAsOutputBuffer(FL2_matchTable* const tbl, size_t const pos);
size_t RMF_memoryUsage(size_t const dict_size, unsigned const buffer_resize, unsigned const thread_count);

#if defined (__cplusplus)
}
#endif

#endif /* RADIX_MF_H */