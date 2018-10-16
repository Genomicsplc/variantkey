// RegionKey
//
// regionkey.h
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/regionkey
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
 * @file regionkey.h
 * @brief RegionKey main functions.
 *
 * The functions provided here allows to generate and process a 64 bit Unsigned Integer Keys for Human Genetic Regions.
 * The RegionKey is sortable for chromosome and start position, and it is also fully reversible.
 */

#ifndef VARIANTKEY_REGIONKEY_H
#define VARIANTKEY_REGIONKEY_H

#include <stdio.h>
#include "nrvk.h"

#define RK_MAX_POS       0x000000000FFFFFFF  //!< Maximum position value (2^28 - 1)
#define RKMASK_CHROM     0xF800000000000000  //!< RegionKey binary mask for CHROM     [ 11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 ]
#define RKMASK_STARTPOS  0x07FFFFFF80000000  //!< RegionKey binary mask for START POS [ 00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000 ]
#define RKMASK_ENDPOS    0x000000007FFFFFF8  //!< RegionKey binary mask for END POS   [ 00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111000 ]
#define RKMASK_STRAND    0x0000000000000006  //!< RegionKey binary mask for STRAND    [ 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000110 ]
#define RKMASK_NOPOS     0xF800000000000007  //!< RegionKey binary mask WITHOUT POS   [ 11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000111 ]
#define RKSHIFT_CHROM     59 //!< CHROM LSB position from the VariantKey LSB
#define RKSHIFT_STARTPOS  31 //!< START POS LSB position from the VariantKey LSB
#define RKSHIFT_ENDPOS     3 //!< END POS LSB position from the VariantKey LSB
#define RKSHIFT_STRAND     1 //!< STRAND LSB position from the VariantKey LSB

#define RK_CHROM    ((rk & RKMASK_CHROM) >> RKSHIFT_CHROM)       //!< Extract the CHROM code from RegionKey.
#define RK_STARTPOS ((rk & RKMASK_STARTPOS) >> RKSHIFT_STARTPOS) //!< Extract the START POS code from RegionKey.
#define RK_ENDPOS   ((rk & RKMASK_ENDPOS) >> RKSHIFT_ENDPOS)     //!< Extract the END POS code from RegionKey.
#define RK_STRAND   ((rk & RKMASK_STRAND) >> RKSHIFT_STRAND)     //!< Extract the STRAND from RegionKey.

/**
 * RegionKey struct.
 * Contains the numerically encoded RegionKey components (CHROM, STARTPOS, ENDPOS, STRAND).
 */
typedef struct regionkey_t
{
    uint8_t chrom;     //!< Chromosome encoded number (only the LSB 5 bit are used)
    uint32_t startpos; //!< Region start position (zero based)
    uint32_t endpos;   //!< Region end position (pos_start + region_length)
    uint8_t strand;    //!< Encoded region strand direction (0 > 0, 1 > +1, 2 > -1).
} regionkey_t;

/**
 * RegionKey decoded struct
 */
typedef struct regionkey_rev_t
{
    char chrom[3];     //!< Chromosome.
    uint32_t startpos; //!< Region start position (zero based)
    uint32_t endpos;   //!< Region end position (pos_start + region_length)
    int8_t strand;     //!< Region strand direction (-1, 0, +1)
} regionkey_rev_t;

/** @brief Encode the strand direction (-1 > 2, 0 > 0, +1 > 1).
 *
 * @param strand     Strand direction (-1, 0, +1).
 *
 * @return      Strand code.
 */
static inline uint8_t encode_region_strand(int8_t strand)
{
    static const uint8_t map[] = {2, 0, 1, 0};
    return map[((uint8_t)(++strand) & 3)];
}

/** @brief Decode the strand direction code (0 > 0, 1 > +1, 2 > -1).
 *
 * @param strand     Strand code.
 *
 * @return      Strand direction.
 */
static inline int8_t decode_region_strand(uint8_t strand)
{
    static const int8_t map[] = {0, 1, -1, 0};
    return map[(strand & 3)];
}

/** @brief Returns a 64 bit regionkey
 *
 * @param chrom      Encoded Chromosome (see encode_chrom).
 * @param startpos   Start position (zero based).
 * @param endpos     End position (startpos + region_length).
 * @param strand     Encoded Strand direction (-1 > 2, 0 > 0, +1 > 1)
 *
 * @return      RegionKey 64 bit code.
 */
static inline uint64_t encode_regionkey(uint8_t chrom, uint32_t startpos, uint32_t endpos, uint8_t strand)
{
    return (((uint64_t)chrom << RKSHIFT_CHROM) | ((uint64_t)startpos << RKSHIFT_STARTPOS) | ((uint64_t)endpos << RKSHIFT_ENDPOS) | ((uint64_t)strand << RKSHIFT_STRAND));
}

/** @brief Extract the CHROM code from RegionKey.
 *
 * @param rk RegionKey code.
 *
 * @return CHROM code.
 */
static inline uint8_t extract_regionkey_chrom(uint64_t rk)
{
    return (uint8_t)RK_CHROM;
}

/** @brief Extract the START POS code from RegionKey.
 *
 * @param rk RegionKey code.
 *
 * @return START POS.
 */
static inline uint32_t extract_regionkey_startpos(uint64_t rk)
{
    return (uint32_t)RK_STARTPOS;
}

/** @brief Extract the END POS code from RegionKey.
 *
 * @param rk RegionKey code.
 *
 * @return END POS.
 */
static inline uint32_t extract_regionkey_endpos(uint64_t rk)
{
    return (uint32_t)RK_ENDPOS;
}

/** @brief Extract the STRAND from RegionKey.
 *
 * @param rk RegionKey code.
 *
 * @return STRAND.
 */
static inline uint8_t extract_regionkey_strand(uint64_t rk)
{
    return (uint8_t)RK_STRAND;
}

/** @brief Decode a RegionKey code and returns the components as regionkey_t structure.
 *
 * @param code RegionKey code.
 * @param rk   Decoded regionkey structure.
 */
static inline void decode_regionkey(uint64_t code, regionkey_t *rk)
{
    rk->chrom = extract_regionkey_chrom(code);
    rk->startpos = extract_regionkey_startpos(code);
    rk->endpos = extract_regionkey_endpos(code);
    rk->strand = extract_regionkey_strand(code);
}

/**
 * Reverse a RegionKey code and returns the normalized components as regionkey_rev_t structure.
 *
 * @param rk       RegionKey code.
 * @param rev      Structure containing the return values.
 */
static inline void reverse_regionkey(uint64_t rk, regionkey_rev_t *rev)
{
    decode_chrom(extract_regionkey_chrom(rk), rev->chrom);
    rev->startpos = extract_regionkey_startpos(rk);
    rev->endpos = extract_regionkey_endpos(rk);
    rev->strand = decode_region_strand(extract_regionkey_strand(rk));
}

/** @brief Returns a 64 bit regionkey based on CHROM, START POS (0-based), END POS and STRAND.
 *
 * @param chrom      Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param sizechrom  Length of the chrom string, excluding the terminating null byte.
 * @param startpos   Start position (zero based).
 * @param endpos     End position (startpos + region_length).
 * @param strand     Strand direction (-1, 0, +1)
 *
 * @return      RegionKey 64 bit code.
 */
static inline uint64_t regionkey(const char *chrom, size_t sizechrom, uint32_t startpos, uint32_t endpos, int8_t strand)
{
    return encode_regionkey(encode_chrom(chrom, sizechrom), startpos, endpos, encode_region_strand(strand));
}

/** @brief Extend a regionkey region by a fixed amount from the start and end position.
 *
 * @param rk   RegionKey code.
 * @param size Amount to extend the region.
 */
static inline uint64_t extend_regionkey(uint64_t rk, uint32_t size)
{
    uint64_t startpos = RK_STARTPOS;
    uint64_t endpos = RK_ENDPOS;
    startpos = ((size >= startpos) ? 0 : (startpos - size));
    endpos = (((RK_MAX_POS - endpos) <= size) ? RK_MAX_POS : (endpos + size));
    return ((rk & RKMASK_NOPOS) | (startpos << RKSHIFT_STARTPOS) | (endpos << RKSHIFT_ENDPOS));
}

/** @brief Returns RegionKey hexadecimal string (16 characters).
 *
 * @param rk    RegionKey code.
 * @param str   String buffer to be returned (it must be sized 17 bytes at least).
 *
 * @return      Upon successful return, these function returns the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
static inline size_t regionkey_hex(uint64_t rk, char *str)
{
    return hex_uint64_t(rk, str);
}

/** @brief Parses a RegionKey hexadecimal string and returns the code.
 *
 * @param rs    RegionKey hexadecimal string (it must contain 16 hexadecimal characters).
 *
 * @return A RegionKey code.
 */
static inline uint64_t parse_regionkey_hex(const char *rs)
{
    return parse_hex_uint64_t(rs);
}

/** @brief Get the CHROM + START POS encoding from RegionKey.
 *
 * @param rk RegionKey code.
 *
 * @return CHROM + START POS.
 */
static inline uint64_t get_regionkey_chrom_startpos(uint64_t rk)
{
    return (rk >> RKSHIFT_STARTPOS);
}

/** @brief Get the CHROM + END POS encoding from RegionKey.
 *
 * @param rk RegionKey code.
 *
 * @return CHROM + END POS.
 */
static inline uint64_t get_regionkey_chrom_endpos(uint64_t rk)
{
    return (((rk & RKMASK_CHROM) >> RKSHIFT_STARTPOS) | extract_regionkey_endpos(rk));
}

/** @brief Check if two regions are overlapping.
 *
 * @param a_chrom     Region A chromosome code.
 * @param a_startpos  Region A start position.
 * @param a_endpos    Region A end position (startpos + region length).
 * @param b_chrom     Region B chromosome code.
 * @param b_startpos  Region B start position.
 * @param b_endpos    Region B end position (startpos + region length).
 *
 * @return 1 if the regions overlap, 0 otherwise.
 */
static inline uint8_t are_overlapping_regions(uint8_t a_chrom, uint32_t a_startpos, uint32_t a_endpos, uint8_t b_chrom, uint32_t b_startpos, uint32_t b_endpos)
{
    return (uint8_t)((a_chrom == b_chrom) && (a_startpos < b_endpos) && (a_endpos > b_startpos));
}

/** @brief Check if a region and a regionkey are overlapping.
 *
 * @param chrom     Region A chromosome code.
 * @param startpos  Region A start position.
 * @param endpos    Region A end position (startpos + region length).
 * @param rk        RegionKey or region B.
 *
 * @return 1 if the regions overlap, 0 otherwise.
 */
static inline uint8_t are_overlapping_region_regionkey(uint8_t chrom, uint32_t startpos, uint32_t endpos, uint64_t rk)
{
    return (uint8_t)((chrom == extract_regionkey_chrom(rk)) && (startpos < extract_regionkey_endpos(rk)) && (endpos > extract_regionkey_startpos(rk)));
}

/** @brief Check if two regionkeys are overlapping.
 *
 * @param rka        RegionKey A.
 * @param rkb        RegionKey B.
 *
 * @return 1 if the regions overlap, 0 otherwise.
 */
static inline uint8_t are_overlapping_regionkeys(uint64_t rka, uint64_t rkb)
{
    return (uint8_t)((extract_regionkey_chrom(rka) == extract_regionkey_chrom(rkb)) && (extract_regionkey_startpos(rka) < extract_regionkey_endpos(rkb)) && (extract_regionkey_endpos(rka) > extract_regionkey_startpos(rkb)));
}

/** @brief Check if variantkey and regionkey are overlapping.
 *
 * @param nvc   Structure containing the pointers to the NRVK memory mapped file columns.
 * @param vk    VariantKey code.
 * @param rk    RegionKey code.
 *
 * @return 1 if the regions overlap, 0 otherwise.
 */
static inline uint8_t are_overlapping_variantkey_regionkey(nrvk_cols_t nvc, uint64_t vk, uint64_t rk)
{
    return (uint8_t)((extract_variantkey_chrom(vk) == extract_regionkey_chrom(rk)) && (extract_variantkey_pos(vk) < extract_regionkey_endpos(rk)) && (get_variantkey_endpos(nvc, vk) > extract_regionkey_startpos(rk)));
}

/** @brief Get RegionKey from VariantKey.
 *
 * @param nvc   Structure containing the pointers to the NRVK memory mapped file columns.
 * @param vk    VariantKey code.
 *
 * @return RegionKey.
 */
static inline uint64_t variantkey_to_regionkey(nrvk_cols_t nvc, uint64_t vk)
{
    return ((vk & VKMASK_CHROMPOS) | ((uint64_t)get_variantkey_endpos(nvc, vk) << RKSHIFT_ENDPOS));
}

#endif  // VARIANTKEY_REGIONKEY_H
