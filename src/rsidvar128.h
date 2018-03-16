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
 * VariantHash128 from binary files made of adjacent
 * constant-length binary blocks sorted in ascending order.
 *
 * The functions provided here allows fast search for RSID and VariantHash128 values from binary files
 * made of adjacent constant-length binary blocks sorted in ascending order.
 *
 * The input binary files can be generated using the rsidvar128.sh script in the resources/tools folder.
 *
 * The rsid_varhash128.bin file contains adjacent 20 bytes binary blocks
 * with the following structure:
 *
 *     00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19
 *     +---------+ +---------+ +---------+ +---------+ +---------+
 *     |  RSID   | | ASSBLY  | |  CHROM  | |   POS   | | REF_ALT |
 *     +---------+ +---------+ +---------+ +---------+ +---------+
 *                 |                VARIANT_HASH                 |
 *                 +---------------------------------------------+
 *
 *
 * The varhash128_rsid.bin file contains adjacent 20 bytes binary blocks
 * with the following structure:
 *
 *     00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19
 *     +---------+ +---------+ +---------+ +---------+ +---------+
 *     | ASSBLY  | |  CHROM  | |   POS   | | REF_ALT | |  RSID   |
 *     +---------+ +---------+ +---------+ +---------+ +---------+
 *     |                VARIANT_HASH                 |
 *     +---------------------------------------------+
 *
 */

#ifndef RSIDVAR128_H
#define RSIDVAR128_H

#include "varianthash.h"
#include "binsearch.h"

#define RSIDVAR128_BIN_BLKLEN 20 //!< Length of a binary block containing RSID + VARHASH128

#define RSIDVAR128_BPOS_RSID    0 //!< RSIDVAR offset of RS ID
#define RSIDVAR128_BPOS_ASSBLY  4 //!< RSIDVAR offset of ASSEMBLY HASH
#define RSIDVAR128_BPOS_CHROM   8 //!< RSIDVAR offset of CHROM
#define RSIDVAR128_BPOS_POS    12 //!< RSIDVAR offset of POS
#define RSIDVAR128_BPOS_REFALT 16 //!< RSIDVAR offset of REF+ALT HASH

#define VARRSID128_BPOS_ASSBLY  0 //!< VARRSID offset of ASSEMBLY HASH
#define VARRSID128_BPOS_CHROM   4 //!< VARRSID offset of CHROM
#define VARRSID128_BPOS_POS     8 //!< VARRSID offset of POS
#define VARRSID128_BPOS_REFALT 12 //!< VARRSID offset of REF+ALT HASH
#define VARRSID128_BPOS_RSID   16 //!< VARRSID offset of RS ID

/**
 * Returns the RSID at the specified position.
 *
 * @param src       Memory mapped file address.
 * @param item      Binary block number.
 *
 * @return RS ID
 */
uint32_t get_vr128_rsid(const unsigned char *src, uint64_t item);

/**
 * Returns the VariantHash128 at the specified position.
 *
 * @param src       Memory mapped file address.
 * @param item      Binary block number.
 *
 * @return variant hash data
 */
varhash128_t get_rv128_varhash(const unsigned char *src, uint64_t item);

/**
 * Search for the specified RSID and returns the first occurrence of VariantHash128.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param rsid      RSID to search.
 *
 * @return variant hash data or all-zero data if not found
 */
varhash128_t find_rv128_varhash_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid);

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
uint32_t find_vr128_rsid_by_varhash(const unsigned char *src, uint64_t *first, uint64_t last, varhash128_t vh);

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
uint32_t find_vr128_chrom_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t chrom);

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
uint32_t find_vr128_pos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t pos_start, uint32_t pos_end);

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
uint32_t find_vr128_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t chrom, uint32_t pos_start, uint32_t pos_end);

#endif  // RSIDVAR128_H
