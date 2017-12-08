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
 * constant-lenght binary blocks sorted in ascending order.
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
 *     2F 81 F5 7B 00 00 00 11 02 61 0D FC 26 CA A8 F5 48 8B 87 17
 *     +---------+ +---------+ +---------+ +---------------------+
 *     |  RSID   | |  CHROM  | |   POS   | |    REF_ALT_HASH     |
 *     +---------+ +---------+ +---------+ +---------------------+
 *                 +---------------------------------------------+
 *                 |                VARIANT_HASH                 |
 *                 +---------------------------------------------+
 *
 *
 * The varhash_rsid.bin file contains adjacent 20 bytes binary blocks
 * with the following structure:
 *
 *     00 00 11 02 61 0D FC 26 CA A8 F5 48 8B 87 17 2F 81 F5 7B 00
 *     +---------+ +---------+ +---------------------+ +---------+
 *     |  CHROM  | |   POS   | |    REF_ALT_HASH     | |  RSID   |
 *     +---------+ +---------+ +---------------------+ +---------+
 *     +---------------------------------------------+
 *     |                VARIANT_HASH                 |
 *     +---------------------------------------------+
 *
 */

#ifndef RSIDVAR_H
#define RSIDVAR_H

#include "varianthash.h"
#include "binsearch.h"

#define RSIDVAR_BIN_BLKLEN 20 //!< Lenght of a binary block containing RSID + VARHASH

#define RSIDVAR_BPOS_RSID   0 //!< RSIDVAR offset of RS ID 
#define RSIDVAR_BPOS_CHROM  4 //!< RSIDVAR offset of CHROM
#define RSIDVAR_BPOS_POS    8 //!< RSIDVAR offset of POS
#define RSIDVAR_BPOS_RFH   12 //!< RSIDVAR offset of REF_ALT_HASH

#define VARRSID_BPOS_CHROM  0 //!< VARRSID offset of CHROM 
#define VARRSID_BPOS_POS    4 //!< VARRSID offset of POS
#define VARRSID_BPOS_RFH    8 //!< VARRSID offset of REF_ALT_HASH
#define VARRSID_BPOS_RSID  16 //!< VARRSID offset of RS ID

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
 * @return variant hash data
 */
varhash_t find_rv_varhash_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid);

/**
 * Search for the specified CHROM-POS range and returns the first occurrence of RSID.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Pointer to the last element of the range to search (max value = nitems - 1).
 *                  This will hold the position of the first record found.
 * @param chrom     Chromosome number. Non numerical human chromosomes mest be encoded as (X=23, Y=24, XY=25, MT=26)
 * @param pos_start Start reference position, with the 1st base having position 0.
 * @param pos_end   End reference position, with the 1st base having position 0.
 *
 * @return RS ID
 */
uint32_t find_vr_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t chrom, uint32_t pos_start, uint32_t pos_end);

/**
 * Search for the specified VariantHash and returns the first occurrence of RSID.
 *
 * @param src       Memory mapped file address.
 * @param first     Pointer to the first element of the range to search (min value = 0).
 *                  This will hold the position of the first record found.
 * @param last      Last element of the range to search (max value = nitems - 1).
 * @param vh        VariantHash.
 *
 * @return RS ID
 */
uint32_t find_vr_rsid_by_varhash(const unsigned char *src, uint64_t *first, uint64_t last, varhash_t vh);

#endif  // RSIDVAR_H
