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

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>

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
    unsigned char *src; //!< Pointer to the memory map.
    int fd;             //!< File descriptor.
    uint64_t size;      //!< File size in bytes.
    uint64_t last;      //!< Index of the last element (if set as last 4 bytes) or it can be used as index size.
    uint64_t *index;    //!< Index of elements (if required)
} mmfile_t;

/**
 * Convert bytes to the specified type.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_declare_bytes_to(O, T) \
/** Convert bytes in "O" format to T.
@param src      Memory mapped file address.
@param i        Start position.
@return Converted number
*/ \
T bytes_##O##_to_##T(const unsigned char *src, uint64_t i);

define_declare_bytes_to(be, uint8_t)
define_declare_bytes_to(be, uint16_t)
define_declare_bytes_to(be, uint32_t)
define_declare_bytes_to(be, uint64_t)
define_declare_bytes_to(le, uint8_t)
define_declare_bytes_to(le, uint16_t)
define_declare_bytes_to(le, uint32_t)
define_declare_bytes_to(le, uint64_t)

/**
 * Define functions to return a pointer to the offset position.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_declare_get_src_offset(T) \
/** Return a pointer to the offset position.
@param src      Memory mapped file address.
@param offset   Start position.
@return Pointer
*/ \
const T *get_src_offset_##T(const unsigned char *src, uint64_t offset);

define_declare_get_src_offset(uint8_t)
define_declare_get_src_offset(uint16_t)
define_declare_get_src_offset(uint32_t)
define_declare_get_src_offset(uint64_t)

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_declare_find_first(O, T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return item number if found or (last + 1) if not found.
 */ \
uint64_t find_first_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search);

define_declare_find_first(be, uint8_t)
define_declare_find_first(be, uint16_t)
define_declare_find_first(be, uint32_t)
define_declare_find_first(be, uint64_t)
define_declare_find_first(le, uint8_t)
define_declare_find_first(le, uint16_t)
define_declare_find_first(le, uint32_t)
define_declare_find_first(le, uint64_t)

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_first_sub(O, T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return item number if found or (last + 1) if not found.
 */ \
uint64_t find_first_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search);

define_declare_find_first_sub(be, uint8_t)
define_declare_find_first_sub(be, uint16_t)
define_declare_find_first_sub(be, uint32_t)
define_declare_find_first_sub(be, uint64_t)
define_declare_find_first_sub(le, uint8_t)
define_declare_find_first_sub(le, uint16_t)
define_declare_find_first_sub(le, uint32_t)
define_declare_find_first_sub(le, uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_last(O, T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return Item number if found or (last + 1) if not found.
*/ \
uint64_t find_last_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search);

define_declare_find_last(be, uint8_t)
define_declare_find_last(be, uint16_t)
define_declare_find_last(be, uint32_t)
define_declare_find_last(be, uint64_t)
define_declare_find_last(le, uint8_t)
define_declare_find_last(le, uint16_t)
define_declare_find_last(le, uint32_t)
define_declare_find_last(le, uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_last_sub(O, T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must be encoded in "O" format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return Item number if found or (last + 1) if not found.
*/ \
uint64_t find_last_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search);

define_declare_find_last_sub(be, uint8_t)
define_declare_find_last_sub(be, uint16_t)
define_declare_find_last_sub(be, uint32_t)
define_declare_find_last_sub(be, uint64_t)
define_declare_find_last_sub(le, uint8_t)
define_declare_find_last_sub(le, uint16_t)
define_declare_find_last_sub(le, uint32_t)
define_declare_find_last_sub(le, uint64_t)

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_has_next(O, T) \
/** Check if the next occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data still matches the search value.
The values in the file must be encoded in "O" format and sorted in ascending order.
This function can be used after find_first_##T to get the next elements that still satisfy the search.
The item returned by find_first_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
bool has_next_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *pos, uint64_t last, T search);

define_declare_has_next(be, uint8_t)
define_declare_has_next(be, uint16_t)
define_declare_has_next(be, uint32_t)
define_declare_has_next(be, uint64_t)
define_declare_has_next(le, uint8_t)
define_declare_has_next(le, uint16_t)
define_declare_has_next(le, uint32_t)
define_declare_has_next(le, uint64_t)

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_has_next_sub(O, T) \
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
@param last      Last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
bool has_next_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search);

define_declare_has_next_sub(be, uint8_t)
define_declare_has_next_sub(be, uint16_t)
define_declare_has_next_sub(be, uint32_t)
define_declare_has_next_sub(be, uint64_t)
define_declare_has_next_sub(le, uint8_t)
define_declare_has_next_sub(le, uint16_t)
define_declare_has_next_sub(le, uint32_t)
define_declare_has_next_sub(le, uint64_t)

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_has_prev(O, T) \
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
bool has_prev_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t *pos, T search);

define_declare_has_prev(be, uint8_t)
define_declare_has_prev(be, uint16_t)
define_declare_has_prev(be, uint32_t)
define_declare_has_prev(be, uint64_t)
define_declare_has_prev(le, uint8_t)
define_declare_has_prev(le, uint16_t)
define_declare_has_prev(le, uint32_t)
define_declare_has_prev(le, uint64_t)

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param O Endiannes: be or le.
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_has_prev_sub(O, T) \
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
bool has_prev_sub_##O##_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search);

define_declare_has_prev_sub(be, uint8_t)
define_declare_has_prev_sub(be, uint16_t)
define_declare_has_prev_sub(be, uint32_t)
define_declare_has_prev_sub(be, uint64_t)
define_declare_has_prev_sub(le, uint8_t)
define_declare_has_prev_sub(le, uint16_t)
define_declare_has_prev_sub(le, uint32_t)
define_declare_has_prev_sub(le, uint64_t)

// --- COLUMN MODE ---

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_declare_col_find_first(T) \
/** Search for the first occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return item number if found or (last + 1) if not found.
 */ \
uint64_t col_find_first_##T(const T *src, uint64_t *first, uint64_t *last, T search);

define_declare_col_find_first(uint8_t)
define_declare_col_find_first(uint16_t)
define_declare_col_find_first(uint32_t)
define_declare_col_find_first(uint64_t)

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_find_first_sub(T) \
/** Search for the first occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return item number if found or (last + 1) if not found.
 */ \
uint64_t col_find_first_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search);

define_declare_col_find_first_sub(uint8_t)
define_declare_col_find_first_sub(uint16_t)
define_declare_col_find_first_sub(uint32_t)
define_declare_col_find_first_sub(uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_find_last(T) \
/** Search for the last occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return Item number if found or (last + 1) if not found.
*/ \
uint64_t col_find_last_##T(const T *src, uint64_t *first, uint64_t *last, T search);

define_declare_col_find_last(uint8_t)
define_declare_col_find_last(uint16_t)
define_declare_col_find_last(uint32_t)
define_declare_col_find_last(uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory buffer containing contiguos blocks of unsigned integers of the same type.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_find_last_sub(T) \
/** Search for the last occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of unsigned integers of the same type.
The values must be encoded in Little-Endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return Item number if found or (last + 1) if not found.
*/ \
uint64_t col_find_last_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search);

define_declare_col_find_last_sub(uint8_t)
define_declare_col_find_last_sub(uint16_t)
define_declare_col_find_last_sub(uint32_t)
define_declare_col_find_last_sub(uint64_t)

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_has_next(T) \
/** Check if the next occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of sorted binary data of the same type still matches the search value.
The values must be encoded in Little-Endian format and sorted in ascending order.
This function can be used after find_first_##T to get the next elements that still satisfy the search.
The item returned by col_find_first_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
bool col_has_next_##T(const T *src, uint64_t *pos, uint64_t last, T search);

define_declare_col_has_next(uint8_t)
define_declare_col_has_next(uint16_t)
define_declare_col_has_next(uint32_t)
define_declare_col_has_next(uint64_t)

/**
 * Generic function to check if the next item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_has_next_sub(T) \
/** Check if the next occurrence of an unsigned integer on a memory buffer
containing contiguos blocks of sorted binary data of the same type still matches the search value.
The values must be encoded in Little-Endian format and sorted in ascending order.
This function can be used after find_first_sub_##T to get the next elements that still satisfy the search.
The item returned by col_find_first_sub_##T should be set as the "pos" parameter in this function.
@param src       Memory mapped file address.
@param bitstart  First bit position to consider (usually 0).
@param bitend    Last bit position to consider (usually the last bit, e.g. 7 for uint8_t, 15 for uint16_t, etc).
@param pos       Pointer to the current item position. This will be updated to point to the next position.
@param last      Last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return 1 if the next item is valid, 0 otherwise.
 */ \
bool col_has_next_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t *pos, uint64_t last, T search);

define_declare_col_has_next_sub(uint8_t)
define_declare_col_has_next_sub(uint16_t)
define_declare_col_has_next_sub(uint32_t)
define_declare_col_has_next_sub(uint64_t)

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_has_prev(T) \
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
bool col_has_prev_##T(const T *src, uint64_t first, uint64_t *pos, T search);

define_declare_col_has_prev(uint8_t)
define_declare_col_has_prev(uint16_t)
define_declare_col_has_prev(uint32_t)
define_declare_col_has_prev(uint64_t)

/**
 * Generic function to check if the previous item still matches the search value.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_col_has_prev_sub(T) \
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
bool col_has_prev_sub_##T(const T *src, uint8_t bitstart, uint8_t bitend, uint64_t first, uint64_t *pos, T search);

define_declare_col_has_prev_sub(uint8_t)
define_declare_col_has_prev_sub(uint16_t)
define_declare_col_has_prev_sub(uint32_t)
define_declare_col_has_prev_sub(uint64_t)

// --- FILE ---

/**
 * Memory map the specified file.
 *
 * @param file  Path to the file to map.
 * @param mf    Structure containing the memory mapped file.
 *
 * @return Returns the memory-mapped file descriptors.
 */
void mmap_binfile(const char *file, mmfile_t *mf);

/**
 * Unmap and close the memory-mapped file.
 *
 * @param mf Descriptor of memory-mapped file.
 *
 * @return On success, munmap() returns 0,
 *         on failure -1, and errno is set (probably to EINVAL).
 */
int munmap_binfile(mmfile_t mf);

#ifdef __cplusplus
}
#endif

#endif  // BINSEARCH_H
