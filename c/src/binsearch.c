// Copyright (c) 2017-2018 Nicola Asuni - Tecnick.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "binsearch.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define define_bytes_to(O, T) \
T bytes_##O##_to_##T(const uint8_t *src, uint64_t i) \
{ \
    return order_##O##_##T(*((const T *)(src + i))); \
}

define_bytes_to(be, uint8_t)
define_bytes_to(be, uint16_t)
define_bytes_to(be, uint32_t)
define_bytes_to(be, uint64_t)
define_bytes_to(le, uint8_t)
define_bytes_to(le, uint16_t)
define_bytes_to(le, uint32_t)
define_bytes_to(le, uint64_t)

#define define_get_src_offset(T) \
const T *get_src_offset_##T(const uint8_t *src, uint64_t offset) \
{ \
    return get_src_offset(T, src, offset); \
}

define_get_src_offset(uint8_t)
define_get_src_offset(uint16_t)
define_get_src_offset(uint32_t)
define_get_src_offset(uint64_t)

#define GET_MIDDLE_BLOCK(O, T) order_##O##_##T(*(get_src_offset(T, src, get_address(blklen, blkpos, middle))))

#define FIND_START_LOOP_BLOCK(T) \
    uint64_t middle, found = *last; \
    T x; \
    while (*first < *last) \
    { \
        middle = get_middle_point(*first, *last); \

#define FIND_END_LOOP_BLOCK \
    } \
    return found;

#define SUB_ITEM_VARS(T) \
    T bitmask = ((T)1 << (bitend - bitstart)); \
    bitmask ^= (bitmask - 1); \
    const uint8_t rshift = (((uint8_t)(sizeof(T) * 8) - 1) - bitend);

#define GET_ITEM_TASK(O, T) \
        x = GET_MIDDLE_BLOCK(O, T);

#define COL_GET_ITEM_TASK \
        x = *(src + middle);

#define GET_SUB_ITEM_TASK(O, T) \
        x = ((GET_MIDDLE_BLOCK(O, T) >> rshift) & bitmask);

#define COL_GET_SUB_ITEM_TASK \
        x = ((*(src + middle) >> rshift) & bitmask);

#define FIND_FIRST_INNER_CHECK \
        if (x == search) \
        { \
            if (middle == 0) { \
                return middle; \
            } \
            found = middle; \
            *last = middle; \
        } \
        else { \
            if (x < search) { \
                *first = (middle + 1); \
            } \
            else \
            { \
                if (middle > 0) { \
                    *last = middle; \
                } \
                else \
                { \
                    return found; \
                } \
            } \
        }

#define FIND_LAST_INNER_CHECK \
        if (x == search) \
        { \
            found = middle; \
            *first = (middle + 1); \
        } \
        else \
        { \
            if (x < search) { \
                *first = (middle + 1); \
            } \
            else \
            { \
                if (middle > 0) { \
                    *last = middle; \
                } \
                else \
                { \
                    return found; \
                } \
            } \
        }

#define HAS_NEXT_START_BLOCK \
    if (*pos >= (last - 1)) \
    { \
        return 0; \
    } \
    (*pos)++;

#define HAS_PREV_START_BLOCK \
    if (*pos <= first) { \
        return 0; \
    } \
    (*pos)--;

#define GET_POS_BLOCK(O, T) order_##O##_##T(*(get_src_offset(T, src, get_address(blklen, blkpos, *pos))))

#define HAS_END_BLOCK(O, T) \
    return (GET_POS_BLOCK(O, T) == search);

#define COL_HAS_END_BLOCK(T) \
    return (*(src + *pos) == search);

#define HAS_SUB_END_BLOCK(O, T) \
    return (((GET_POS_BLOCK(O, T) >> rshift) & bitmask) == search);

#define COL_HAS_SUB_END_BLOCK(T) \
    return (((*(src + *pos) >> rshift) & bitmask) == search);

#define define_find_first(O, T) \
uint64_t find_first_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
GET_ITEM_TASK(O, T) \
FIND_FIRST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_find_first(be, uint8_t)
define_find_first(be, uint16_t)
define_find_first(be, uint32_t)
define_find_first(be, uint64_t)
define_find_first(le, uint8_t)
define_find_first(le, uint16_t)
define_find_first(le, uint32_t)
define_find_first(le, uint64_t)

#define define_find_first_sub(O, T) \
uint64_t find_first_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
GET_SUB_ITEM_TASK(O, T) \
FIND_FIRST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_find_first_sub(be, uint8_t)
define_find_first_sub(be, uint16_t)
define_find_first_sub(be, uint32_t)
define_find_first_sub(be, uint64_t)
define_find_first_sub(le, uint8_t)
define_find_first_sub(le, uint16_t)
define_find_first_sub(le, uint32_t)
define_find_first_sub(le, uint64_t)

#define define_find_last(O, T) \
uint64_t find_last_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
GET_ITEM_TASK(O, T) \
FIND_LAST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_find_last(be, uint8_t)
define_find_last(be, uint16_t)
define_find_last(be, uint32_t)
define_find_last(be, uint64_t)
define_find_last(le, uint8_t)
define_find_last(le, uint16_t)
define_find_last(le, uint32_t)
define_find_last(le, uint64_t)

#define define_find_last_sub(O, T) \
uint64_t find_last_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
GET_SUB_ITEM_TASK(O, T) \
FIND_LAST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_find_last_sub(be, uint8_t)
define_find_last_sub(be, uint16_t)
define_find_last_sub(be, uint32_t)
define_find_last_sub(be, uint64_t)
define_find_last_sub(le, uint8_t)
define_find_last_sub(le, uint16_t)
define_find_last_sub(le, uint32_t)
define_find_last_sub(le, uint64_t)

#define define_has_next(O, T) \
bool has_next_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t *pos, uint64_t last, T search) \
{ \
HAS_NEXT_START_BLOCK \
HAS_END_BLOCK(O, T) \
}

define_has_next(be, uint8_t)
define_has_next(be, uint16_t)
define_has_next(be, uint32_t)
define_has_next(be, uint64_t)
define_has_next(le, uint8_t)
define_has_next(le, uint16_t)
define_has_next(le, uint32_t)
define_has_next(le, uint64_t)

#define define_has_next_sub(O, T) \
bool has_next_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search) \
{ \
HAS_NEXT_START_BLOCK \
SUB_ITEM_VARS(T) \
HAS_SUB_END_BLOCK(O, T) \
}

define_has_next_sub(be, uint8_t)
define_has_next_sub(be, uint16_t)
define_has_next_sub(be, uint32_t)
define_has_next_sub(be, uint64_t)
define_has_next_sub(le, uint8_t)
define_has_next_sub(le, uint16_t)
define_has_next_sub(le, uint32_t)
define_has_next_sub(le, uint64_t)

#define define_has_prev(O, T) \
bool has_prev_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t *pos, T search) \
{ \
HAS_PREV_START_BLOCK \
HAS_END_BLOCK(O, T) \
}

define_has_prev(be, uint8_t)
define_has_prev(be, uint16_t)
define_has_prev(be, uint32_t)
define_has_prev(be, uint64_t)
define_has_prev(le, uint8_t)
define_has_prev(le, uint16_t)
define_has_prev(le, uint32_t)
define_has_prev(le, uint64_t)

#define define_has_prev_sub(O, T) \
bool has_prev_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search) \
{ \
HAS_PREV_START_BLOCK \
SUB_ITEM_VARS(T) \
HAS_SUB_END_BLOCK(O, T) \
}

define_has_prev_sub(be, uint8_t)
define_has_prev_sub(be, uint16_t)
define_has_prev_sub(be, uint32_t)
define_has_prev_sub(be, uint64_t)
define_has_prev_sub(le, uint8_t)
define_has_prev_sub(le, uint16_t)
define_has_prev_sub(le, uint32_t)
define_has_prev_sub(le, uint64_t)

// --- COLUMN MODE ---

#define define_col_find_first(T) \
uint64_t col_find_first_##T(const T *src, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
COL_GET_ITEM_TASK \
FIND_FIRST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_col_find_first(uint8_t)
define_col_find_first(uint16_t)
define_col_find_first(uint32_t)
define_col_find_first(uint64_t)

#define define_col_find_first_sub(T) \
uint64_t col_find_first_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
COL_GET_SUB_ITEM_TASK \
FIND_FIRST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_col_find_first_sub(uint8_t)
define_col_find_first_sub(uint16_t)
define_col_find_first_sub(uint32_t)
define_col_find_first_sub(uint64_t)

#define define_col_find_last(T) \
uint64_t col_find_last_##T(const T *src, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
COL_GET_ITEM_TASK \
FIND_LAST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_col_find_last(uint8_t)
define_col_find_last(uint16_t)
define_col_find_last(uint32_t)
define_col_find_last(uint64_t)

#define define_col_find_last_sub(T) \
uint64_t col_find_last_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
COL_GET_SUB_ITEM_TASK \
FIND_LAST_INNER_CHECK \
FIND_END_LOOP_BLOCK \
}

define_col_find_last_sub(uint8_t)
define_col_find_last_sub(uint16_t)
define_col_find_last_sub(uint32_t)
define_col_find_last_sub(uint64_t)

#define define_col_has_next(T) \
bool col_has_next_##T(const T *src, uint64_t *pos, uint64_t last, T search) \
{ \
HAS_NEXT_START_BLOCK \
COL_HAS_END_BLOCK(T) \
}

define_col_has_next(uint8_t)
define_col_has_next(uint16_t)
define_col_has_next(uint32_t)
define_col_has_next(uint64_t)

#define define_col_has_next_sub(T) \
bool col_has_next_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search) \
{ \
HAS_NEXT_START_BLOCK \
SUB_ITEM_VARS(T) \
COL_HAS_SUB_END_BLOCK(T) \
}

define_col_has_next_sub(uint8_t)
define_col_has_next_sub(uint16_t)
define_col_has_next_sub(uint32_t)
define_col_has_next_sub(uint64_t)

#define define_col_has_prev(T) \
bool col_has_prev_##T(const T *src, uint64_t first, uint64_t *pos, T search) \
{ \
HAS_PREV_START_BLOCK \
COL_HAS_END_BLOCK(T) \
}

define_col_has_prev(uint8_t)
define_col_has_prev(uint16_t)
define_col_has_prev(uint32_t)
define_col_has_prev(uint64_t)

#define define_col_has_prev_sub(T) \
bool col_has_prev_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search) \
{ \
HAS_PREV_START_BLOCK \
SUB_ITEM_VARS(T) \
COL_HAS_SUB_END_BLOCK(T) \
}

define_col_has_prev_sub(uint8_t)
define_col_has_prev_sub(uint16_t)
define_col_has_prev_sub(uint32_t)
define_col_has_prev_sub(uint64_t)

// --- FILE ---

static void parse_col_offset(mmfile_t *mf)
{
    uint8_t i;
    uint64_t b = 0;
    mf->index[0] = mf->doffset;
    for (i = 0; i < mf->ncols; i++)
    {
        b += mf->ctbytes[i];
    }
    if (b == 0)
    {
        return;
    }
    mf->nrows = (mf->dlength / b);
    for (i = 1; i < mf->ncols; i++)
    {
        b = (mf->nrows * mf->ctbytes[(i - 1)]);
        mf->index[i] = mf->index[(i - 1)] + b + ((8 - (b & 7)) & 7); // account for 8-byte padding
    }
}

static void parse_info_binsrc(mmfile_t *mf)
{
    const uint8_t *tp = (const uint8_t *)(mf->src + 8);
    mf->ncols = *tp++;
    mf->doffset = (uint64_t)9 + mf->ncols + ((8 - ((mf->ncols + 1) & 7)) & 7); // account for 8-byte padding
    const uint64_t *op = (const uint64_t *)(mf->src + mf->doffset);
    mf->nrows = *op++;
    uint8_t i;
    for (i = 0; i < mf->ncols; i++)
    {
        mf->ctbytes[i] = *tp++;
        mf->index[i] = *op++;
    }
    mf->doffset += ((mf->ncols + 1) * 8); // skip column offsets section
    mf->dlength -= mf->doffset;
}

static void parse_info_arrow(mmfile_t *mf)
{
    mf->doffset = (uint64_t)(*((const uint32_t *)(mf->src + 9))) + 13; // skip metadata
    mf->doffset += (uint64_t)(*((const uint32_t *)(mf->src + mf->doffset)) + 4); // skip dictionary
    mf->dlength -= mf->doffset;
    uint64_t type = (*((const uint64_t *)(mf->src + mf->size - 8)));
    if ((type & 0xffffffffffff0000) == 0x31574f5252410000) // magic number "ARROW1" in LE
    {
        mf->dlength -= (uint64_t)(*((const uint32_t *)(mf->src + mf->size - 10))) + 10; // remove footer
    }
}

static void parse_info_feather(mmfile_t *mf)
{
    mf->doffset = 8;
    mf->dlength -= mf->doffset;
    uint32_t type = (*((const uint32_t *)(mf->src + mf->size - 4)));
    if (type == 0x31414546) // magic number "FEA1" in LE
    {
        mf->dlength -= (uint64_t)(*((const uint32_t *)(mf->src + mf->size - 8))) + 8; // remove metadata
    }
}

void mmap_binfile(const char *file, mmfile_t *mf)
{
    mf->src = (uint8_t*)MAP_FAILED; // NOLINT
    mf->fd = -1;
    mf->size = 0;
    struct stat statbuf;
    if (((mf->fd = open(file, O_RDONLY)) < 0) || (fstat(mf->fd, &statbuf) < 0))
    {
        return;
    }
    mf->size = (uint64_t)statbuf.st_size;
    mf->src = (uint8_t*)mmap(0, mf->size, PROT_READ, MAP_PRIVATE, mf->fd, 0);
    mf->doffset = 0;
    mf->dlength = mf->size;
    if (mf->size < 28)
    {
        return;
    }
    uint64_t type = (*((const uint64_t *)(mf->src)));
    switch (type)
    {
    // Custom binsearch format
    case 0x00314352534e4942: // magic number "BINSRC1" in LE
        parse_info_binsrc(mf);
        return;
    // Basic support for Apache Arrow File format with a single RecordBatch.
    case 0x000031574f525241: // magic number "ARROW1" in LE
        parse_info_arrow(mf);
        break;
    // Basic support for Feather File format.
    case 0x0000000031414546: // magic number "FEA1" in LE
        parse_info_feather(mf);
        break;
    }
    parse_col_offset(mf);
}

int munmap_binfile(mmfile_t mf)
{
    int err = munmap(mf.src, mf.size);
    if (err != 0)
    {
        return err;
    }
    return close(mf.fd);
}
