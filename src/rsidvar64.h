// Copyright (c) 2017-2018 GENOMICS plc
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
 * @file rsidvar.h
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to search RSID and
 * VariantHash64 from binary files made of adjacent
 * constant-length binary blocks sorted in ascending order.
 *
 * The functions provided here allows fast search for RSID and VariantHash64 values from binary files
 * made of adjacent constant-length binary blocks sorted in ascending order.
 *
 * The input binary files can be generated using the rsidvar64.sh script in the resources/tools folder.
 *
 * The rsid_varhash64.bin file contains adjacent 12 bytes binary blocks
 * with the following structure:
 *
 *     00 01 02 03 04          05 06 07    08 09 10 11
 *     +---------+ +---------+ +---------+ +---------+
 *     |  RSID   | |  CHROM  | |   POS   | | REF_ALT |
 *     +---------+ +---------+ +---------+ +---------+
 *                 |          VARIANT_HASH           |
 *                 +---------------------------------+
 *
 *
 * The varhash64_rsid.bin file contains adjacent 12 bytes binary blocks
 * with the following structure:
 *
 *     00          01 02 03 04 05 06 07    08 09 10 11
 *     +---------+ +---------+ +---------+ +---------+
 *     |  CHROM  | |   POS   | | REF_ALT | |  RSID   |
 *     +---------+ +---------+ +---------+ +---------+
 *     |          VARIANT_HASH           |
 *     +---------------------------------+
 */

#ifndef RSIDVAR64_H
#define RSIDVAR64_H

#include "varianthash.h"
#include "binsearch.h"

#define RSIDVAR64_BIN_BLKLEN 12 //!< Length of a binary block containing RSID + VARHASH64

#define RSIDVAR64_BPOS_RSID   0 //!< RSIDVAR offset of RS ID
#define RSIDVAR64_BPOS_CHROM  4 //!< RSIDVAR offset of CHROM
#define RSIDVAR64_BPOS_POS    5 //!< RSIDVAR offset of POS
#define RSIDVAR64_BPOS_REFALT 9 //!< RSIDVAR offset of REF+ALT HASH

#define VARRSID64_BPOS_CHROM  0 //!< VARRSID offset of CHROM
#define VARRSID64_BPOS_POS    1 //!< VARRSID offset of POS
#define VARRSID64_BPOS_REFALT 5 //!< VARRSID offset of REF+ALT HASH
#define VARRSID64_BPOS_RSID   8 //!< VARRSID offset of RS ID

/**
 * Returns the RSID at the specified position.
 *
 * @param src       Memory mapped file address.
 * @param item      Binary block number.
 *
 * @return RS ID
 */
uint32_t get_vr64_rsid(const unsigned char *src, uint64_t item);

/**
 * Returns the VariantHash64 at the specified position.
 *
 * @param src       Memory mapped file address.
 * @param item      Binary block number.
 *
 * @return variant hash data
 */
uint64_t get_rv64_varhash(const unsigned char *src, uint64_t item);

/**
 * Search for the specified RSID and returns the first occurrence of VariantHash64.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param rsid      RSID to search.
 *
 * @return VariantHash64 data or zero data if not found
 */
uint64_t find_rv64_varhash_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid);

/**
 * Search for the specified VariantHash and returns the first occurrence of RSID.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param vh        VariantHash.
 *
 * @return RS ID or 0 if not found
 */
uint32_t find_vr64_rsid_by_varhash(const unsigned char *src, uint64_t *first, uint64_t last, uint64_t vh);

/**
 * Search for the specified CHROM range and returns the first occurrence of RSID.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 * @param last      Pointer to the last element of the range to search (max value = nitems - 1).
 * @param chrom     Chromosome number.
 *
 * @return RS ID
 */
uint32_t find_vr64_chrom_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint8_t chrom);

/**
 * Search for the specified POS range and returns the first occurrence of RSID.
 * This function should be used only in conjuction with find_vr128_chrom_range to search a range inside a given chromosome.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 * @param last      Pointer to the last element of the range to search (max value = nitems - 1).
 * @param pos_start Start reference position, with the 1st base having position 0.
 * @param pos_end   End reference position, with the 1st base having position 0.
 *
 * @return RS ID
 */
uint32_t find_vr64_pos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t pos_start, uint32_t pos_end);

/**
 * Search for the specified CHROM-POS range and returns the first occurrence of RSID.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 * @param last      Pointer to the last element of the range to search (max value = nitems - 1).
 * @param chrom     Chromosome number.
 * @param pos_start Start reference position, with the 1st base having position 0.
 * @param pos_end   End reference position, with the 1st base having position 0.
 *
 * @return RS ID
 */
uint32_t find_vr64_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint8_t chrom, uint32_t pos_start, uint32_t pos_end);

#endif  // RSIDVAR64_H
