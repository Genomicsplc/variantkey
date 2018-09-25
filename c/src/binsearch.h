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

/**
 * @file binsearch.h
 * @brief Functions to search values in binary files made of constant-length items.
 *
 * The functions provided here allows to search unsigned integers
 * in a binary file made of adjacent constant-length binary blocks
 * sorted in ascending order.
 *
 * For example, the first 4 bytes of each 8-bytes blocks below
 * represent a uint32 in big-endian. The integers are sorted in
 * ascending order.
 *
 *   - 2f 81 f5 77 1a cc 7b 43
 *   - 2f 81 f5 78 76 5f 63 b8
 *   - 2f 81 f5 79 ca a9 a6 52
 *
 * This binary representation can be used to encode sortable key-value data,
 * even with nested keys.
 *
 * The xxd command-line application can be used to convert a binary file to
 * hexdump and reverse. For example:
 *
 *   - xxd -p binaryfile.bin > hexfile.txt
 *   - xxd -r -p hexfile.txt > binaryfile.bin
 *
 * NOTE:
 *   - The "_be_" or "BE" functions refer to source files sorted in Big-Endian.
 *   - The "_le_" or "LE" functions refer to source files sorted in Little-Endian.
 */

#ifndef BINSEARCH_H
#define BINSEARCH_H

#include <inttypes.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Account for Endianness

//!< \cond

#ifdef WORDS_BIGENDIAN
#undef BINSEARCH_BIG_ENDIAN
#define BINSEARCH_BIG_ENDIAN 1
#endif

#if defined(BINSEARCH_LITTLE_ENDIAN) && defined(BINSEARCH_BIG_ENDIAN)
#error
#endif

#if !defined(BINSEARCH_LITTLE_ENDIAN) && !defined(BINSEARCH_BIG_ENDIAN)
#define BINSEARCH_UNKNOWN_ENDIAN 1
#endif

#if !defined(bswap_16) ||  !defined(bswap_32) || !defined(bswap_64)
#undef bswap_16
#undef bswap_32
#undef bswap_64

#if defined(HAVE_BUILTIN_BSWAP) || defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ >= 5))
#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#endif

#endif

#if defined(BINSEARCH_UNKNOWN_ENDIAN) || !defined(bswap_64)

#ifdef _MSC_VER

#undef bswap_16
#undef bswap_32
#undef bswap_64
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin
#include <libkern/OSByteOrder.h>
#undef bswap_16
#undef bswap_32
#undef bswap_64
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#undef bswap_16
#undef bswap_32
#undef bswap_64
#define bswap_16(x) BSWAP_16(x)
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#undef bswap_16
#undef bswap_32
#undef bswap_64
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#undef bswap_16
#undef bswap_32
#undef bswap_64
#define bswap_16(x) swap16(x)
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#undef bswap_16
#undef bswap_32
#undef bswap_64
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#undef bswap_16
#undef bswap_32
#undef bswap_64
#include <byteswap.h>

#endif

#ifdef WORDS_BIGENDIAN
#define BINSEARCH_BIG_ENDIAN 1
#endif

#endif

//!< \endcond

#define order_be_uint8_t(x) (x) //!< Return BE uint8_t
#define order_le_uint8_t(x) (x) //!< Return LE uint8_t

#ifdef BINSEARCH_BIG_ENDIAN
#define order_be_uint16_t(x) (x) //!< Return BE uint16_t in the correct endianness order
#define order_be_uint32_t(x) (x) //!< Return BE uint32_t in the correct endianness order
#define order_be_uint64_t(x) (x) //!< Return BE uint64_t in the correct endianness order
#define order_le_uint16_t(x) (bswap_16(x)) //!< Return LE uint16_t in the correct endianness order
#define order_le_uint32_t(x) (bswap_32(x)) //!< Return LE uint32_t in the correct endianness order
#define order_le_uint64_t(x) (bswap_64(x)) //!< Return LE uint64_t in the correct endianness order
#else
#define order_be_uint16_t(x) (bswap_16(x)) //!< Return BE uint16_t in the correct endianness order
#define order_be_uint32_t(x) (bswap_32(x)) //!< Return BE uint32_t in the correct endianness order
#define order_be_uint64_t(x) (bswap_64(x)) //!< Return BE uint64_t in the correct endianness order
#define order_le_uint16_t(x) (x) //!< Return LE uint16_t in the correct endianness order
#define order_le_uint32_t(x) (x) //!< Return LE uint32_t in the correct endianness order
#define order_le_uint64_t(x) (x) //!< Return LE uint64_t in the correct endianness order
#endif

#define MAXCOLS 256 //!< Maximum number of columns indexable

/**
 * Returns the absolute file address position of the specified item (binary block).
 *
 * @param blklen    Length of the binary block in bytes.
 * @param blkpos    Indicates the position of the value to search inside a binary block.
 * @param item      Item number to search.
 *
 * @return First byte position of the specified item number.
 */
#define get_address(blklen, blkpos, item) (((blklen) * (item)) + (blkpos))

/**
 * Returns the middle point between two values.
 *
 * @param first    First point (smaller point).
 * @param last     Last point (bigger point).
 *
 * @return Middle point.
 */
#define get_middle_point(first, last) ((first) + (((last) - (first)) >> 1))

/**
 * Returns the pointer at the specified byte offset.
 *
 * @param T        Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 * @param src      Memory mapped file address.
 * @param offset   Byte offset.
 *
 * @return Pointer.
 */
#define get_src_offset(T, src, offset) ((const T *)((src) + (offset)))

/**
 * Struct containing the memory mapped file info.
 */
typedef struct mmfile_t
{
    uint8_t *src;               //!< Pointer to the memory map.
    int fd;                     //!< File descriptor.
    uint64_t size;              //!< File size in bytes.
    uint64_t doffset;           //!< Offset to the beginning of the data block (address of the first byte of the first item in the first column).
    uint64_t dlength;           //!< Length in bytes of the data block.
    uint64_t nrows;             //!< Number of rows.
    uint8_t  ncols;             //!< Number of columns - THIS MUST BE MANUALLY SET EXCEPT FOR THE "BINSRC1" FORMAT.
    uint8_t  ctbytes[MAXCOLS];  //!< Number of bytes per column type (i.e. 1 for uint8_t, 2 for uint16_t, 4 for uint32_t, 8 for uint64_t). - THIS MUST BE MANUALLY SET EXCEPT FOR THE "BINSRC1" FORMAT.
    uint64_t index[MAXCOLS];    //!< Index of the offsets to the beginning of each column.
} mmfile_t;

/**
 * Convert bytes to the specified type.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_bytes_to(O, T) \
/** Convert bytes in "O" format to T.
@param src      Memory mapped file address.
@param i        Start position.
@return Converted number
*/ \
static inline T bytes_##O##_to_##T(const uint8_t *src, uint64_t i) \
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

/**
 * Define functions to return a pointer to the offset position.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_get_src_offset(T) \
/** Return a pointer to the offset position.
@param src      Memory mapped file address.
@param offset   Start position.
@return Pointer
*/ \
static inline const T *get_src_offset_##T(const uint8_t *src, uint64_t offset) \
{ \
    return get_src_offset(T, src, offset); \
}

define_get_src_offset(uint8_t)
define_get_src_offset(uint16_t)
define_get_src_offset(uint32_t)
define_get_src_offset(uint64_t)

#define GET_MIDDLE_BLOCK(O, T) order_##O##_##T(*(get_src_offset(T, src, get_address(blklen, blkpos, middle))))

#define FIND_START_LOOP_BLOCK(T) \
    uint64_t middle, notfound = *last; \
    T x; \
    while (*first < *last) \
    { \
        middle = get_middle_point(*first, *last); \

#define FIND_END_LOOP_BLOCK \
    if (x == search) \
    { \
        return middle; \
    } \
    if (*first > 0) \
    { \
        --(*first); \
    } \
    return notfound;

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
        if (x < search) { \
            *first = middle; \
            ++(*first); \
        } \
        else \
        { \
            *last = middle; \
        } \
    } \
    middle = *first;

#define FIND_LAST_INNER_CHECK \
        if (x > search) { \
            *last = middle; \
        } \
        else \
        { \
            *first = middle; \
            ++(*first); \
        } \
    } \
    middle = *first; \
    --middle;

#define HAS_NEXT_START_BLOCK \
    if (*pos >= (last - 1)) \
    { \
        return 0; \
    } \
    ++(*pos);

#define HAS_PREV_START_BLOCK \
    if (*pos <= first) { \
        return 0; \
    } \
    --(*pos);

#define GET_POS_BLOCK(O, T) order_##O##_##T(*(get_src_offset(T, src, get_address(blklen, blkpos, *pos))))

#define HAS_END_BLOCK(O, T) \
    return (GET_POS_BLOCK(O, T) == search);

#define COL_HAS_END_BLOCK(T) \
    return (*(src + *pos) == search);

#define HAS_SUB_END_BLOCK(O, T) \
    return (((GET_POS_BLOCK(O, T) >> rshift) & bitmask) == search);

#define COL_HAS_SUB_END_BLOCK(T) \
    return (((*(src + *pos) >> rshift) & bitmask) == search);

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_find_first(O, T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return item number if notfound or (last + 1) if not notfound.
 */ \
static inline uint64_t find_first_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
GET_ITEM_TASK(O, T) \
FIND_FIRST_INNER_CHECK \
GET_ITEM_TASK(O, T) \
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

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_find_first_sub(O, T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return item number if notfound or (last + 1) if not notfound.
 */ \
static inline uint64_t find_first_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
GET_SUB_ITEM_TASK(O, T) \
FIND_FIRST_INNER_CHECK \
GET_SUB_ITEM_TASK(O, T) \
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

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_find_last(O, T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return Item number if notfound or (last + 1) if not notfound.
*/ \
static inline uint64_t find_last_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
GET_ITEM_TASK(O, T) \
FIND_LAST_INNER_CHECK \
GET_ITEM_TASK(O, T) \
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

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_find_last_sub(O, T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return Item number if notfound or (last + 1) if not notfound.
*/ \
static inline uint64_t find_last_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
GET_SUB_ITEM_TASK(O, T) \
FIND_LAST_INNER_CHECK \
GET_SUB_ITEM_TASK(O, T) \
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

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_has_next(O, T) \
/** Check if the next occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data still matches the search value.
The values in the file must be encoded in "O" format and sorted in ascending order.
This function can be used after find_first_##T to get the next elements that still satisfy the search.
The item returned by find_first_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool has_next_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t *pos, uint64_t last, T search) \
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

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_has_next_sub(O, T) \
/** Check if the next occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data still matches the search value.
The values in the file must be encoded in "O" format and sorted in ascending order.
This function can be used after find_first_sub_##T to get the next elements that still satisfy the search.
The item returned by find_first_sub_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool has_next_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search) \
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

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_has_prev(O, T) \
/** Check if the previous occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data still matches the search value.
The values in the file must be encoded in "O" format and sorted in ascending order.
This function can be used after find_last_##T to get the previous elements that still satisfy the search.
The item returned by find_last_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     First element of the range to search (min value = 0).
@param pos       Pointer to the current item position. This will be updated to point to the previous position.
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool has_prev_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t *pos, T search) \
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

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_has_prev_sub(O, T) \
/** Check if the previous occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data still matches the search value.
The values in the file must be encoded in "O" format and sorted in ascending order.
This function can be used after find_last_sub_##T to get the previous elements that still satisfy the search.
The item returned by find_last_sub_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     First element of the range to search (min value = 0).
@param pos       Pointer to the current item position. This will be updated to point to the previous position.
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool has_prev_sub_##O##_##T(const uint8_t *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search) \
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

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_col_find_first(T) \
/** Search for the first occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return item number if notfound or (last + 1) if not notfound.
 */ \
static inline uint64_t col_find_first_##T(const T *src, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
COL_GET_ITEM_TASK \
FIND_FIRST_INNER_CHECK \
COL_GET_ITEM_TASK \
FIND_END_LOOP_BLOCK \
}

define_col_find_first(uint8_t)
define_col_find_first(uint16_t)
define_col_find_first(uint32_t)
define_col_find_first(uint64_t)

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_find_first_sub(T) \
/** Search for the first occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return item number if notfound or (last + 1) if not notfound.
 */ \
static inline uint64_t col_find_first_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
COL_GET_SUB_ITEM_TASK \
FIND_FIRST_INNER_CHECK \
COL_GET_SUB_ITEM_TASK \
FIND_END_LOOP_BLOCK \
}

define_col_find_first_sub(uint8_t)
define_col_find_first_sub(uint16_t)
define_col_find_first_sub(uint32_t)
define_col_find_first_sub(uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_find_last(T) \
/** Search for the last occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return Item number if notfound or (last + 1) if not notfound.
*/ \
static inline uint64_t col_find_last_##T(const T *src, uint64_t *first, uint64_t *last, T search) \
{ \
FIND_START_LOOP_BLOCK(T) \
COL_GET_ITEM_TASK \
FIND_LAST_INNER_CHECK \
COL_GET_ITEM_TASK \
FIND_END_LOOP_BLOCK \
}

define_col_find_last(uint8_t)
define_col_find_last(uint16_t)
define_col_find_last(uint32_t)
define_col_find_last(uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_find_last_sub(T) \
/** Search for the last occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the element from where to start the search (min value = 0).
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return Item number if notfound or (last + 1) if not notfound.
*/ \
static inline uint64_t col_find_last_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search) \
{ \
SUB_ITEM_VARS(T) \
FIND_START_LOOP_BLOCK(T) \
COL_GET_SUB_ITEM_TASK \
FIND_LAST_INNER_CHECK \
COL_GET_SUB_ITEM_TASK \
FIND_END_LOOP_BLOCK \
}

define_col_find_last_sub(uint8_t)
define_col_find_last_sub(uint16_t)
define_col_find_last_sub(uint32_t)
define_col_find_last_sub(uint64_t)

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_has_next(T) \
/** Check if the next occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of sorted binary data of the same type still matches the search value.
The values must be encoded in Little-Endian format and sorted in ascending order.
This function can be used after find_first_##T to get the next elements that still satisfy the search.
The item returned by col_find_first_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool col_has_next_##T(const T *src, uint64_t *pos, uint64_t last, T search) \
{ \
HAS_NEXT_START_BLOCK \
COL_HAS_END_BLOCK(T) \
}

define_col_has_next(uint8_t)
define_col_has_next(uint16_t)
define_col_has_next(uint32_t)
define_col_has_next(uint64_t)

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_has_next_sub(T) \
/** Check if the next occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of sorted binary data of the same type still matches the search value.
The values must be encoded in Little-Endian format and sorted in ascending order.
This function can be used after find_first_sub_##T to get the next elements that still satisfy the search.
The item returned by col_find_first_sub_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Pointer to the element (up to but not including) where to end the search (max value = nrows).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool col_has_next_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search) \
{ \
HAS_NEXT_START_BLOCK \
SUB_ITEM_VARS(T) \
COL_HAS_SUB_END_BLOCK(T) \
}

define_col_has_next_sub(uint8_t)
define_col_has_next_sub(uint16_t)
define_col_has_next_sub(uint32_t)
define_col_has_next_sub(uint64_t)

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_has_prev(T) \
/** Check if the previous occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of sorted binary data of the same type still matches the search value.
The values must be encoded in Little-Endian format and sorted in ascending order.
This function can be used after find_last_##T to get the previous elements that still satisfy the search.
The item returned by col_find_last_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param first     First element of the range to search (min value = 0).
@param pos       Pointer to the current item position. This will be updated to point to the previous position.
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool col_has_prev_##T(const T *src, uint64_t first, uint64_t *pos, T search) \
{ \
HAS_PREV_START_BLOCK \
COL_HAS_END_BLOCK(T) \
}

define_col_has_prev(uint8_t)
define_col_has_prev(uint16_t)
define_col_has_prev(uint32_t)
define_col_has_prev(uint64_t)

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_col_has_prev_sub(T) \
/** Check if the previous occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of sorted binary data of the same type still matches the search value.
The values must be encoded in Little-Endian format and sorted in ascending order.
This function can be used after find_last_sub_##T to get the previous elements that still satisfy the search.
The item returned by col_find_last_sub_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     First element of the range to search (min value = 0).
@param pos       Pointer to the current item position. This will be updated to point to the previous position.
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
static inline bool col_has_prev_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search) \
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

static inline void parse_col_offset(mmfile_t *mf)
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

static inline void parse_info_binsrc(mmfile_t *mf)
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

static inline void parse_info_arrow(mmfile_t *mf)
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

static inline void parse_info_feather(mmfile_t *mf)
{
    mf->doffset = 8;
    mf->dlength -= mf->doffset;
    uint32_t type = (*((const uint32_t *)(mf->src + mf->size - 4)));
    if (type == 0x31414546) // magic number "FEA1" in LE
    {
        mf->dlength -= (uint64_t)(*((const uint32_t *)(mf->src + mf->size - 8))) + 8; // remove metadata
    }
}

/**
 * Memory map the specified file.
 *
 * @param file  Path to the file to map.
 * @param mf    Structure containing the memory mapped file.
 *
 * @return Returns the memory-mapped file descriptors.
 */
static inline void mmap_binfile(const char *file, mmfile_t *mf)
{
    mf->src = (uint8_t*)MAP_FAILED; // NOLINT
    mf->fd = -1;
    mf->size = 0;
    mf->doffset = 0;
    mf->dlength = 0;
    mf->nrows = 0;
    struct stat statbuf;
    if (((mf->fd = open(file, O_RDONLY)) < 0) || (fstat(mf->fd, &statbuf) < 0))
    {
        return;
    }
    mf->size = (uint64_t)statbuf.st_size;
    mf->src = (uint8_t*)mmap(0, mf->size, PROT_READ, MAP_PRIVATE, mf->fd, 0);
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

/**
 * Unmap and close the memory-mapped file.
 *
 * @param mf Descriptor of memory-mapped file.
 *
 * @return On success, munmap() returns 0,
 *         on failure -1, and errno is set (probably to EINVAL).
 */
static inline int munmap_binfile(mmfile_t mf)
{
    int err = munmap(mf.src, mf.size);
    if (err != 0)
    {
        return err;
    }
    return close(mf.fd);
}

#endif  // BINSEARCH_H
