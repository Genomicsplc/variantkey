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
 * The functions provided here allows to search big-endian
 * unsigned integers in a binary file made of adjacent
 * constant-length binary blocks sorted in ascending order.
 *
 * For example, the first 4 bytes of each 8-bytes blocks below
 * represent a uint32 in big-endian. The integers are sorted in
 * ascending order.
 *
 * 2f 81 f5 77 1a cc 7b 43
 * 2f 81 f5 78 76 5f 63 b8
 * 2f 81 f5 79 ca a9 a6 52
 *
 * This binary representation can be used to encode sortable key-value data,
 * even with nested keys.
 *
 * The xxd command-line application can be used to convert a binary file to
 * hexdump and reverse. For example:
 *
 * xxd -p binaryfile.bin > hexfile.txt
 * xxd -r -p hexfile.txt > binaryfile.bin
 *
 */

#ifndef BINSEARCH_H
#define BINSEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

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

/**
 * Returns the absolute file address position of the specified item (binary block).
 *
 * @param blklen    Length of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param item      Item number to search.
 *
 * Return First byte position of the specified item number.
 */
uint64_t get_address(uint64_t blklen, uint64_t blkpos, uint64_t item);

/**
 * Convert bytes to the specified type.
 */
#define define_declare_bytes_to(T) \
/** Convert bytes in big-endian format to unsigned integer
@param src      Memory mapped file address.
@param i        Start position.
@return Converted number
*/ \
T bytes_to_##T(const unsigned char *src, uint64_t i);

define_declare_bytes_to(uint8_t)
define_declare_bytes_to(uint16_t)
define_declare_bytes_to(uint32_t)
define_declare_bytes_to(uint64_t)

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_first(T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must encoded in big-endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return item number if found or (last + 1) if not found.
 */ \
uint64_t find_first_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search);

define_declare_find_first(uint8_t)
define_declare_find_first(uint16_t)
define_declare_find_first(uint32_t)
define_declare_find_first(uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_last(T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must encoded in big-endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return Item number if found or (last + 1) if not found.
*/ \
uint64_t find_last_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search);

define_declare_find_last(uint8_t)
define_declare_find_last(uint16_t)
define_declare_find_last(uint32_t)
define_declare_find_last(uint64_t)


/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_first_sub(T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must encoded in big-endian format and sorted in ascending order.
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
uint64_t find_first_sub_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search);

define_declare_find_first_sub(uint8_t)
define_declare_find_first_sub(uint16_t)
define_declare_find_first_sub(uint32_t)
define_declare_find_first_sub(uint64_t)

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_declare_find_last_sub(T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must encoded in big-endian format and sorted in ascending order.
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
uint64_t find_last_sub_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint8_t bitstart, uint8_t bitend, uint64_t *first, uint64_t *last, T search);

define_declare_find_last_sub(uint8_t)
define_declare_find_last_sub(uint16_t)
define_declare_find_last_sub(uint32_t)
define_declare_find_last_sub(uint64_t)

#ifdef __cplusplus
}
#endif

#endif  // BINSEARCH_H
