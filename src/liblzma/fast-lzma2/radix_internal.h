/*
* Copyright (c) 2018, Conor McCarthy
* All rights reserved.
*
* This source code is licensed under both the BSD-style license (found in the
* LICENSE file in the root directory of this source tree) and the GPLv2 (found
* in the COPYING file in the root directory of this source tree).
* You may select, at your option, one of the above-listed licenses.
*/

#ifndef RADIX_INTERNAL_H
#define RADIX_INTERNAL_H

#include "common.h"
#include "atomic.h"
#include "radix_mf.h"
#include "lzma_encoder_private.h"

#if defined(TUKLIB_FAST_UNALIGNED_ACCESS)
#  define memcpy32(d, s) d = *(const uint32_t*)(s)
#else
#  define memcpy32(d, s) memcpy(&d, s, 4)
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#define DICTIONARY_LOG_MIN 12U
#define DICTIONARY_LOG_MAX_64 30U
#define DICTIONARY_LOG_MAX_32 27U
#define DICTIONARY_SIZE_MIN ((size_t)1 << DICTIONARY_LOG_MIN)
#define DICTIONARY_SIZE_MAX_64 ((size_t)1 << DICTIONARY_LOG_MAX_64)
#define DICTIONARY_SIZE_MAX_32 ((size_t)1 << DICTIONARY_LOG_MAX_32)
#define MAX_REPEAT 24
#define RADIX16_TABLE_SIZE ((size_t)1 << 16)
#define RADIX8_TABLE_SIZE ((size_t)1 << 8)
#define STACK_SIZE (RADIX16_TABLE_SIZE * 3)
#define MAX_BRUTE_FORCE_LIST_SIZE 5
#define BUFFER_LINK_MASK 0xFFFFFFU
#define MATCH_BUFFER_OVERLAP 6
#define BITPACK_MAX_LENGTH 63U
#define STRUCTURED_MAX_LENGTH 255U

#define RADIX_LINK_BITS 26
#define RADIX_LINK_MASK ((1U << RADIX_LINK_BITS) - 1)
#define RADIX_NULL_LINK 0xFFFFFFFFU

#define UNIT_BITS 2
#define UNIT_MASK ((1U << UNIT_BITS) - 1)

#define RADIX_CANCEL_INDEX (long)(RADIX16_TABLE_SIZE + LZMA_THREADS_MAX + 2)

typedef struct
{
    uint32_t head;
    uint32_t count;
} RMF_tableHead;

union src_data_u {
    uint8_t chars[4];
    uint32_t u32;
};

typedef struct
{
    uint32_t from;
    union src_data_u src;
    uint32_t next;
} RMF_buildMatch;

typedef struct
{
    uint32_t prev_index;
    uint32_t list_count;
} RMF_listTail;

typedef struct
{
    uint32_t links[1 << UNIT_BITS];
    uint8_t lengths[1 << UNIT_BITS];
} RMF_unit;

struct RMF_builder_s
{
    unsigned max_len;
    uint32_t* table;
    size_t match_buffer_size;
    size_t match_buffer_limit;
    RMF_listTail tails_8[RADIX8_TABLE_SIZE];
    RMF_tableHead stack[STACK_SIZE];
    RMF_listTail tails_16[RADIX16_TABLE_SIZE];
    RMF_buildMatch match_buffer[1];
};

struct FL2_matchTable_s
{
    FL2_atomic st_index;
    long end_index;
    int is_struct;
    int alloc_struct;
    size_t unreduced_dict_size;
    size_t progress;
    RMF_parameters params;
    uint32_t stack[RADIX16_TABLE_SIZE];
    RMF_tableHead list_heads[RADIX16_TABLE_SIZE];
    uint32_t table[1];
};

void RMF_bitpackInit(struct FL2_matchTable_s* const tbl, const void* data, size_t const end);
void RMF_structuredInit(struct FL2_matchTable_s* const tbl, const void* data, size_t const end);
void RMF_bitpackBuildTable(struct FL2_matchTable_s* const tbl,
	RMF_builder* const builder,
	int const thread,
	lzma_data_block const block);
void RMF_structuredBuildTable(struct FL2_matchTable_s* const tbl,
	RMF_builder* const builder,
	int const thread,
	lzma_data_block const block);
void RMF_recurseListChunk(RMF_builder* const tbl,
    const uint8_t* const data_block,
    size_t const block_start,
    uint32_t const depth,
    uint32_t const max_depth,
    uint32_t const list_count,
    size_t const stack_base);
int RMF_bitpackIntegrityCheck(const struct FL2_matchTable_s* const tbl, const uint8_t* const data, size_t pos, size_t const end, unsigned max_depth);
int RMF_structuredIntegrityCheck(const struct FL2_matchTable_s* const tbl, const uint8_t* const data, size_t pos, size_t const end, unsigned max_depth);
void RMF_bitpackLimitLengths(struct FL2_matchTable_s* const tbl, size_t const pos);
void RMF_structuredLimitLengths(struct FL2_matchTable_s* const tbl, size_t const pos);
uint8_t* RMF_bitpackAsOutputBuffer(struct FL2_matchTable_s* const tbl, size_t const pos);
uint8_t* RMF_structuredAsOutputBuffer(struct FL2_matchTable_s* const tbl, size_t const pos);

#if defined (__cplusplus)
}
#endif

#endif /* RADIX_INTERNAL_H */