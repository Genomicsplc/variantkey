// Copyright (c) 2017 Nicola Asuni - Tecnick.com
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
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to search big-endian
 * unsigned integers in a binary file made of adjacent
 * constant-lenght binary blocks sorted in ascending order.
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

#include <inttypes.h>

/**
 * Struct containing the memory mapped file info.
 */
typedef struct mmfile_t
{
    /** Pointer to the memory map. */
    unsigned char *src;
    /** File descriptor. */
    int fd;
    /** File size in bytes. */
    uint64_t size;
} mmfile_t;

/**
 * Memory map the specified file.
 *
 * @param file  Path to the file to map.
 *
 * @return Returns the memory-mapped file descriptors.
 */
mmfile_t mmap_binfile(const char *file);

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
 * Returns the absolute file start position of the specified item (binary block).
 *
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param item      Item number to search.
 *
 * Return First byte position of the specified item number.
 */
uint64_t get_address(uint64_t blklen, uint64_t blkpos, uint64_t item);

/**
 * Convert 4 bytes in big-endian format to uint32
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint32_t bytes_to_uint32be(const unsigned char *src, uint64_t i);

/**
 * Convert 8 bytes in big-endian format to uint64
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint64_t bytes_to_uint64be(const unsigned char *src, uint64_t i);

/**
 * Search for the first occurrence of a 32 bit unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param src       Memory mapped file address.
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param first     First element of the range to search (min value = 0).
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param search    Unsigned number to search (32 bit).
 *
 * @return item number if found or (last + 1) if not found.
 */
uint64_t find_first_uint32be(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t last, uint32_t search);

/**
 * Search for the last occurrence of a 32 bit unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param src       Memory mapped file address.
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param first     First element of the range to search (min value = 0).
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param search    Unsigned number to search (32 bit).
 *
 * @return Item number if found or (last + 1) if not found.
 */
uint64_t find_last_uint32be(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t last, uint32_t search);

/**
 * Search for the first occurrence of a 64 bit unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param src       Memory mapped file address.
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param first     First element of the range to search (min value = 0).
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param search    Unsigned number to search (64 bit).
 *
 * @return item number if found or (last + 1) if not found.
 */
uint64_t find_first_uint64be(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t last, uint64_t search);

/**
 * Search for the last occurrence of a 64 bit unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param src       Memory mapped file address.
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param first     First element of the range to search (min value = 0).
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param search    Unsigned number to search (64 bit).
 *
 * @return item number if found or (last + 1) if not found.
 */
uint64_t find_last_uint64be(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t last, uint64_t search);

/**
 * Search for the first occurrence of a 128 bit unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param src       Memory mapped file address.
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param first     First element of the range to search (min value = 0).
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param search_hi First 64 bit of unsigned number to search.
 * @param search_lo Last 64 bit of unsigned number to search.
 *
 * @return item number if found or (last + 1) if not found.
 */
uint64_t find_first_uint128be(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t last, uint64_t search_hi, uint64_t search_lo);

/**
 * Search for the last occurrence of a 128 bit unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param src       Memory mapped file address.
 * @param blklen    Lenght of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param first     First element of the range to search (min value = 0).
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param search_hi First 64 bit of unsigned number to search.
 * @param search_lo Last 64 bit of unsigned number to search.
 *
 * @return item number if found or (last + 1) if not found.
 */
uint64_t find_last_uint128be(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t first, uint64_t last, uint64_t search_hi, uint64_t search_lo);

#endif  // BINSEARCH_H
