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
T bytes_##O##_to_##T(const unsigned char *src, uint64_t i) \
{ \
    const T *pos = (const T *)(src + i); \
    return order_##O##_##T(*pos); \
}

define_bytes_to(be, uint8_t)
define_bytes_to(be, uint16_t)
define_bytes_to(be, uint32_t)
define_bytes_to(be, uint64_t)
define_bytes_to(le, uint8_t)
define_bytes_to(le, uint16_t)
define_bytes_to(le, uint32_t)
define_bytes_to(le, uint64_t)

#define define_find_first(O, T) \
uint64_t find_first_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = bytes_##O##_to_##T(src, i); \
        if (x == search) \
        { \
            if (middle == 0) { \
                return middle; \
            } \
            found = middle; \
            *last = (middle - 1); \
        } \
        else { \
            if (x < search) { \
                *first = (middle + 1); \
            } \
            else \
            { \
                if (middle > 0) { \
                    *last = (middle - 1); \
                } \
                else \
                { \
                    return found; \
                } \
            } \
        } \
    } \
    return found; \
}

define_find_first(be, uint8_t)
define_find_first(be, uint16_t)
define_find_first(be, uint32_t)
define_find_first(be, uint64_t)
define_find_first(le, uint8_t)
define_find_first(le, uint16_t)
define_find_first(le, uint32_t)
define_find_first(le, uint64_t)

#define define_find_last(O, T) \
uint64_t find_last_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = bytes_##O##_to_##T(src, i); \
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
                    *last = (middle - 1); \
                } \
                else \
                { \
                    return found; \
                } \
            } \
        } \
    } \
    return found; \
}

define_find_last(be, uint8_t)
define_find_last(be, uint16_t)
define_find_last(be, uint32_t)
define_find_last(be, uint64_t)
define_find_last(le, uint8_t)
define_find_last(le, uint16_t)
define_find_last(le, uint32_t)
define_find_last(le, uint64_t)


#define define_find_first_sub(O, T) \
uint64_t find_first_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    uint8_t mbit = ((uint8_t)(sizeof(T) * 8) - 1); \
    T bitmask = ((T)1 << (mbit - bitstart)); \
    bitmask ^= (bitmask - 1); \
    uint8_t rshift = (mbit - bitend); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = ((bytes_##O##_to_##T(src, i) & bitmask) >> rshift); \
        if (x == search) \
        { \
            if (middle == 0) { \
                return middle; \
            } \
            found = middle; \
            *last = (middle - 1); \
        } \
        else { \
            if (x < search) { \
                *first = (middle + 1); \
            } \
            else \
            { \
                if (middle > 0) { \
                    *last = (middle - 1); \
                } \
                else \
                { \
                    return found; \
                } \
            } \
        } \
    } \
    return found; \
}

define_find_first_sub(be, uint8_t)
define_find_first_sub(be, uint16_t)
define_find_first_sub(be, uint32_t)
define_find_first_sub(be, uint64_t)
define_find_first_sub(le, uint8_t)
define_find_first_sub(le, uint16_t)
define_find_first_sub(le, uint32_t)
define_find_first_sub(le, uint64_t)

#define define_find_last_sub(O, T) \
uint64_t find_last_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    uint8_t mbit = ((uint8_t)(sizeof(T) * 8) - 1); \
    T bitmask = ((T)1 << (mbit - bitstart)); \
    bitmask ^= (bitmask - 1); \
    uint8_t rshift = (mbit - bitend); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = ((bytes_##O##_to_##T(src, i) & bitmask) >> rshift); \
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
                    *last = (middle - 1); \
                } \
                else \
                { \
                    return found; \
                } \
            } \
        } \
    } \
    return found; \
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
bool has_next_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *pos, uint64_t last, T search) \
{ \
    if (*pos >= last) \
    { \
        return 0; \
    } \
    (*pos)++; \
    T x = bytes_##O##_to_##T(src, get_address(blklen, blkpos, *pos)); \
    return (x == search); \
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
bool has_next_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search) \
{ \
    if (*pos >= last) \
    { \
        return 0; \
    } \
    (*pos)++; \
    uint8_t mbit = ((uint8_t)(sizeof(T) * 8) - 1); \
    T bitmask = ((T)1 << (mbit - bitstart)); \
    bitmask ^= (bitmask - 1); \
    uint8_t rshift = (mbit - bitend); \
    T x = ((bytes_##O##_to_##T(src, get_address(blklen, blkpos, *pos)) & bitmask) >> rshift); \
    return (x == search); \
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
bool has_prev_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t *pos, T search) \
{ \
    if (*pos <= first) { \
        return 0; \
    } \
    (*pos)--; \
    T x = bytes_##O##_to_##T(src, get_address(blklen, blkpos, *pos)); \
    return (x == search); \
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
bool has_prev_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search) \
{ \
    if (*pos <= first) { \
        return 0; \
    } \
    uint8_t mbit = ((uint8_t)(sizeof(T) * 8) - 1); \
    T bitmask = ((T)1 << (mbit - bitstart)); \
    bitmask ^= (bitmask - 1); \
    uint8_t rshift = (mbit - bitend); \
    (*pos)--; \
    T x = ((bytes_##O##_to_##T(src, get_address(blklen, blkpos, *pos)) & bitmask) >> rshift); \
    return (x == search); \
}

define_has_prev_sub(be, uint8_t)
define_has_prev_sub(be, uint16_t)
define_has_prev_sub(be, uint32_t)
define_has_prev_sub(be, uint64_t)
define_has_prev_sub(le, uint8_t)
define_has_prev_sub(le, uint16_t)
define_has_prev_sub(le, uint32_t)
define_has_prev_sub(le, uint64_t)

void mmap_binfile(const char *file, mmfile_t *mf)
{
    mf->src = (unsigned char*)MAP_FAILED; // NOLINT
    mf->fd = -1;
    mf->size = 0;
    struct stat statbuf;
    if (((mf->fd = open(file, O_RDONLY)) < 0) || (fstat(mf->fd, &statbuf) < 0))
    {
        return;
    }
    mf->size = (uint64_t)statbuf.st_size;
    mf->src = (unsigned char*)mmap(0, mf->size, PROT_READ, MAP_PRIVATE, mf->fd, 0);
    if (mf->size > 4)
    {
        mf->last = (uint64_t)bytes_be_to_uint32_t(mf->src, (mf->size - 4)) - 1;
    }
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
