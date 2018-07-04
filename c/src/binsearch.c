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
        mf->last = (uint64_t)bytes_to_uint32_t(mf->src, (mf->size - 4)) - 1;
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

inline uint64_t get_address(uint64_t blklen, uint64_t blkpos, uint64_t item)
{
    return ((blklen * item) + blkpos);
}

inline uint8_t bytes_to_uint8_t(const unsigned char *src, uint64_t i)
{
    return ((uint8_t)src[i]);
}

inline uint16_t bytes_to_uint16_t(const unsigned char *src, uint64_t i)
{
    return (((uint16_t)src[i] << 8) | (uint16_t)src[i+1]);
}

inline uint32_t bytes_to_uint32_t(const unsigned char *src, uint64_t i)
{
    return (((uint32_t)src[i] << 24)
            | ((uint32_t)src[i+1] << 16)
            | ((uint32_t)src[i+2] << 8)
            | (uint32_t)src[i+3]);
}

inline uint64_t bytes_to_uint64_t(const unsigned char *src, uint64_t i)
{
    return (((uint64_t)src[i] << 56)
            | ((uint64_t)src[i+1] << 48)
            | ((uint64_t)src[i+2] << 40)
            | ((uint64_t)src[i+3] << 32)
            | ((uint64_t)src[i+4] << 24)
            | ((uint64_t)src[i+5] << 16)
            | ((uint64_t)src[i+6] << 8)
            | (uint64_t)src[i+7]);
}

#define define_find_first(T) \
inline uint64_t find_first_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = bytes_to_##T(src, i); \
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

define_find_first(uint8_t)
define_find_first(uint16_t)
define_find_first(uint32_t)
define_find_first(uint64_t)

#define define_find_last(T) \
inline uint64_t find_last_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = bytes_to_##T(src, i); \
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

define_find_last(uint8_t)
define_find_last(uint16_t)
define_find_last(uint32_t)
define_find_last(uint64_t)


#define define_find_first_sub(T) \
inline uint64_t find_first_sub_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    uint8_t rshift = ((sizeof(T) * 8) - 1 - bitend + bitstart); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = ((bytes_to_##T(src, i) << bitstart) >> rshift); \
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

define_find_first_sub(uint8_t)
define_find_first_sub(uint16_t)
define_find_first_sub(uint32_t)
define_find_first_sub(uint64_t)

#define define_find_last_sub(T) \
inline uint64_t find_last_sub_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    uint8_t rshift = ((sizeof(T) * 8) - 1 - bitend + bitstart); \
    T x; \
    while (*first <= *last) \
    { \
        middle = (*first + ((*last - *first) >> 1)); \
        i = get_address(blklen, blkpos, middle); \
        x = ((bytes_to_##T(src, i) << bitstart) >> rshift); \
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

define_find_last_sub(uint8_t)
define_find_last_sub(uint16_t)
define_find_last_sub(uint32_t)
define_find_last_sub(uint64_t)

#define define_has_next(T) \
inline bool has_next_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *pos, uint64_t last, T search) \
{ \
    if (*pos >= last) \
    { \
        return 0; \
    } \
    (*pos)++; \
    T x = bytes_to_##T(src, get_address(blklen, blkpos, *pos)); \
    return (x == search); \
}

define_has_next(uint8_t)
define_has_next(uint16_t)
define_has_next(uint32_t)
define_has_next(uint64_t)

#define define_has_next_sub(T) \
inline bool has_next_sub_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search) \
{ \
    if (*pos >= last) \
    { \
        return 0; \
    } \
    (*pos)++; \
    uint8_t rshift = ((sizeof(T) * 8) - 1 - bitend + bitstart); \
    T x = ((bytes_to_##T(src, get_address(blklen, blkpos, *pos)) << bitstart) >> rshift); \
    return (x == search); \
}

define_has_next_sub(uint8_t)
define_has_next_sub(uint16_t)
define_has_next_sub(uint32_t)
define_has_next_sub(uint64_t)

#define define_has_prev(T) \
inline bool has_prev_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t *pos, T search) \
{ \
    if (*pos <= first) { \
        return 0; \
    } \
    (*pos)--; \
    T x = bytes_to_##T(src, get_address(blklen, blkpos, *pos)); \
    return (x == search); \
}

define_has_prev(uint8_t)
define_has_prev(uint16_t)
define_has_prev(uint32_t)
define_has_prev(uint64_t)

#define define_has_prev_sub(T) \
inline bool has_prev_sub_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search) \
{ \
    if (*pos <= first) { \
        return 0; \
    } \
    uint8_t rshift = ((sizeof(T) * 8) - 1 - bitend + bitstart); \
    (*pos)--; \
    T x = ((bytes_to_##T(src, get_address(blklen, blkpos, *pos)) << bitstart) >> rshift); \
    return (x == search); \
}

define_has_prev_sub(uint8_t)
define_has_prev_sub(uint16_t)
define_has_prev_sub(uint32_t)
define_has_prev_sub(uint64_t)
