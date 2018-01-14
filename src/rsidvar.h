// Copyright (c) 2017 GENOMICS plc
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
 * The functions provided here allows to search RSIDs and
 * VariantHashes from binary files made of adjacent
 * constant-length binary blocks sorted in ascending order.
 *
 * The input binary files can be generated using some open source tools:
 *
 * # split multialleic variants in dbSNP
 * time bcftools norm --multiallelics -any --output ~/Downloads/dbSNP_All_20151104_split.vcf ~/Downloads/All_20151104.vcf.gz
 * # create RSID to Variant map
 * time bcftools query -f '%RSID_HEX%VARIANT_HASH_HEX\n' ~/Downloads/dbSNP_All_20151104_split.vcf > ~/Downloads/rsid_varhash.txt
 * # create Variant to RSID map
 * time bcftools query -f '%VARIANT_HASH_HEX%RSID_HEX\n' ~/Downloads/dbSNP_All_20151104_split.vcf > ~/Downloads/varhash_rsid.txt
 * # sort the maps
 * LC_ALL=C time sort --parallel=4 --output=rsid_varhash.sorted.txt rsid_varhash.txt
 * LC_ALL=C time sort --parallel=4 --output=varhash_rsid.sorted.txt varhash_rsid.txt
 * # convert the maps to binary format
 * time xxd -r -p rsid_varhash.sorted.txt rsid_varhash.bin
 * time xxd -r -p varhash_rsid.sorted.txt varhash_rsid.bin
 *
 * The rsid_varhash.bin file contains adjacent 20 bytes binary blocks
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
 * The varhash_rsid.bin file contains adjacent 20 bytes binary blocks
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

#ifndef RSIDVAR_H
#define RSIDVAR_H

#include "varianthash.h"
#include "binsearch.h"

#define RSIDVAR_BIN_BLKLEN 20 //!< Length of a binary block containing RSID + VARHASH

#define RSIDVAR_BPOS_RSID    0 //!< RSIDVAR offset of RS ID
#define RSIDVAR_BPOS_ASSBLY  4 //!< RSIDVAR offset of ASSEMBLY HASH
#define RSIDVAR_BPOS_CHROM   8 //!< RSIDVAR offset of CHROM
#define RSIDVAR_BPOS_POS    12 //!< RSIDVAR offset of POS
#define RSIDVAR_BPOS_REFALT 16 //!< RSIDVAR offset of REF_ALT HASH

#define VARRSID_BPOS_ASSBLY  0 //!< VARRSID offset of ASSEMBLY HASH
#define VARRSID_BPOS_CHROM   4 //!< VARRSID offset of CHROM
#define VARRSID_BPOS_POS     8 //!< VARRSID offset of POS
#define VARRSID_BPOS_REFALT 12 //!< VARRSID offset of REF_ALT HASH
#define VARRSID_BPOS_RSID   16 //!< VARRSID offset of RS ID

/**
 * Returns the RSID at the specified position.
 *
 * @param src       Memory mapped file address.
 * @param item      Binary block number.
 *
 * @return RS ID
 */
uint32_t get_vr_rsid(const unsigned char *src, uint64_t item);

/**
 * Returns the VariantHash at the specified position.
 *
 * @param src       Memory mapped file address.
 * @param item      Binary block number.
 *
 * @return variant hash data
 */
varhash_t get_rv_varhash(const unsigned char *src, uint64_t item);

/**
 * Search for the specified RSID and returns the first occurrence of VariantHash.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param rsid      RSID to search.
 *
 * @return variant hash data or all-zero data if not found
 */
varhash_t find_rv_varhash_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid);

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
uint32_t find_vr_rsid_by_varhash(const unsigned char *src, uint64_t *first, uint64_t last, varhash_t vh);

/**
 * Search for the specified CHROM-POS range and returns the first occurrence of RSID.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 * @param last      Pointer to the last element of the range to search (max value = nitems - 1).
 * @param chrom     Chromosome number. Non numerical human chromosomes mest be encoded as (X=23, Y=24, XY=25, MT=26)
 * @param pos_start Start reference position, with the 1st base having position 0.
 * @param pos_end   End reference position, with the 1st base having position 0.
 *
 * @return RS ID
 */
uint32_t find_vr_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t chrom, uint32_t pos_start, uint32_t pos_end);

#endif  // RSIDVAR_H