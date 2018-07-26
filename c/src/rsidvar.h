// VariantKey
//
// rsidvar.h
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey
//
// LICENSE
//
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
 * @brief Functions to read VariantKey-rsID binary files.
 *
 * The functions provided here allows fast search for rsID and VariantKey values from binary files
 * made of adjacent constant-length binary blocks sorted in ascending order.
 *
 * The input binary files can be generated using the vkhexbin.sh script in the resources/tools folder.
 *
 * The `rsvk.bin` (RV) file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:
 *
 * ```
 *     00 01 02 03 04 05 06 07 08 09 10 11
 *     +---------+ +---------------------+
 *     |  RSID   | |     VARIANTKEY      |
 *     +---------+ +---------------------+
 * ```
 *
 *
 * The `vkrs.bin` (VR) file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:
 *
 * ```
 *     00 01 02 03 04 05 06 07 08 09 10 11
 *     +---------------------+ +---------+
 *     |      VARIANTKEY     | |  RSID   |
 *     +---------------------+ +---------+
 * ```
 *
 */

#ifndef RSIDVAR_H
#define RSIDVAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "binsearch.h"
#include "variantkey.h"

#define BINBLKLEN   12 //!< Length in bytes of a binary block containing RSID + VARIANTKEY
#define RVPOS_RSID   0 //!< RSIDVAR byte offset of RSID
#define RVPOS_VK     4 //!< RSIDVAR byte offset of VARIANTKEY
#define VRPOS_VK     0 //!< VARRSID byte offset of VARIANTKEY
#define VRPOS_RSID   8 //!< VARRSID byte offset of RSID

/**
 * Returns the RSID at the specified position in the VR file.
 *
 * @param src       Address of the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).
 * @param item      Binary block number.
 *
 * @return RS ID
 */
uint32_t get_vr_rsid(const unsigned char *src, uint64_t item);

/**
 * Returns the VariantKey at the specified position in the RV file.
 *
 * @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
 * @param item      Binary block number.
 *
 * @return variant key data
 */
uint64_t get_rv_variantkey(const unsigned char *src, uint64_t item);

/**
 * Search for the specified rsID and returns the first occurrence of VariantKey in the RV file.
 *
 * @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param rsid      rsID to search.
 *
 * @return VariantKey data or zero data if not found
 */
uint64_t find_rv_variantkey_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid);

/**
 * Get the next VariantKey for the specified rsID in the RV file.
 * This function should be used after find_rv_variantkey_by_rsid.
 * This function can be called in a loop to get all VariantKeys that are associated with the same rsID (if any).
 *
 * @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
 * @param pos       Pointer to the current item. This will hold the position of the next record.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param rsid      rsID to search.
 *
 * @return VariantKey data or zero data if not found
 */
uint64_t get_next_rv_variantkey_by_rsid(const unsigned char *src, uint64_t *pos, uint64_t last, uint32_t rsid);

/**
 * Search for the specified VariantKey and returns the first occurrence of rsID in the VR file.
 *
 * @param src       Address of the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param vk        VariantKey.
 *
 * @return rsID or 0 if not found
 */
uint32_t find_vr_rsid_by_variantkey(const unsigned char *src, uint64_t *first, uint64_t last, uint64_t vk);

/**
 * Search for the specified CHROM-POS range and returns the first occurrence of rsID in the VR file.
 *
 * @param src       Address of the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).
 * @param first     Pointer to the first element of the range to search (min value = 0).
 * @param last      Pointer to the last element of the range to search (max value = nitems - 1).
 * @param chrom     Chromosome encoded number.
 * @param pos_min   Start reference position, with the first base having position 0.
 * @param pos_max   End reference position, with the first base having position 0.
 *
 * @return rsID
 */
uint32_t find_vr_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint8_t chrom, uint32_t pos_min, uint32_t pos_max);

#ifdef __cplusplus
}
#endif

#endif  // RSIDVAR_H
