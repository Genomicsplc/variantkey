// VariantHash
//
// varianthash.h
//
// @category   Tools
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/varianthash
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
//
// VariantHash by Nicola Asuni

/**
 * @file rsidvar.h
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to generate or decode
 * a genetic Variant Hash.
 *
 * A Genetic Variant Hash is composed of 4 sections that can be also used separately:
 *
 *    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
 *    +---------+ +---------+ +---------+ +---------+
 *    | ASSBLY  | |  CHROM  | |   POS   | | REF_ALT |
 *    +---------+ +---------+ +---------+ +---------+
 *    |                VARIANT_HASH                 |
 *    +---------------------------------------------+
 *
 * ASSBLY  : 32 bits (8 hex bytes) to represent the hash of Genome Assembly (genome sequence).
 * CHROM   : 32 bits (8 hex bytes) to represent the chromosome.
 *           Chromosomes are always encoded as numbers.
 *           Non numerical human chromosomes (X, Y, XY, MT) are auomatically converted to numbers.
 *           For other species than human the string-to-number chromosome conversion should happen before calling the hashing function.
 * POS     : 32 bits (8 hex bytes) for the reference position (POS), with the 1st base having position 0.
 * REF_ALT : 32 bits (8 hex bytes) for the hash of the "REF_ALT" string.
 */

#ifndef VARIANTHASH_H
#define VARIANTHASH_H

#include "farmhash64.h"

/**
 * Contains the crom, pos and ref+alt encoded data.
 */
typedef struct varhash_t
{
    uint32_t assembly; //!< Genome Assembly encoded number.
    uint32_t chrom;    //!< Chromosome encoded number.
    uint32_t pos;      //!< Position. The reference position, with the 1st base having position 0.
    uint32_t refalt;   //!< Hash code for Reference and Alternate
} varhash_t;

/** @brief Fast ASCII Uppercase function for a-z letters
 *
 * @param c character code to convert
 *
 * @return uppercase character code
 */
int aztoupper(int c);

/** @brief Returns 32-bit genome assembly encoding.
 *
 * @param assembly Genome Assembly string.
 *
 * @return Chrom encoding
 */
uint32_t encode_assembly(const char *assembly);

/** @brief Returns 32-bit chromosome encoding.
 *
 * @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 *
 * @return Chrom encoding
 */
uint32_t encode_chrom(const char *chrom);

/** @brief Decode the CHROM code.
 *
 * @param code   CHROM code.
 * @param chrom  CHROM string buffer to be returned.
 *
 * @return If successful, the total number of characters written is returned excluding the null-character appended
 *         at the end of the string, otherwise a negative number is returned in case of failure.
 */
size_t decode_chrom(uint32_t code, char *chrom);

/** @brief Returns 32-bit reference+alternate hash code.
 *
 * @param ref   Reference allele. String containing a sequence of nucleotide letters.
 *              The value in the pos field refers to the position of the first nucleotide in the String.
 * @param alt   Alternate non-reference allele string.
 *
 * @return 32-bit hash code
 */
uint32_t encode_ref_alt(const char *ref, const char *alt);

/** @brief Decode the REF+ALT code if reversible (if it has less than 5 nucleotidesin total).
 *
 * @param code  REF+ALT code
 * @param ref   REF string buffer to be returned.
 * @param alt   ALT string buffer to be returned.
 *
 * @return      Returns the number of characters in the "alt" string if the code is reversible, 0 otherwise.
 */
size_t decode_ref_alt(uint32_t code, char *ref, char *alt);

/** @brief Returns a Genetic Variant Hash based on CHROM, POS (0-base), REF, ALT.
 *
 * @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param pos   Position. The reference position, with the 1st base having position 0.
 * @param ref   Reference allele. String containing a sequence of nucleotide letters.
 *              The value in the pos field refers to the position of the first nucleotide in the String.
 * @param alt   Alternate non-reference allele string.
 *
 * @return      A varhash_t structure.
 */
varhash_t variant_hash(const char *assembly, const char *chrom, uint32_t pos, const char *ref, const char *alt);

/** @brief Returns a human-readable Genetic Variant Hash string (32 hex characters).
 *
 * The string represent a 128bit number or:
 *   - 32bit  (8 hex bytes) for ASSBLY Hash
 *   - 32bit  (8 hex bytes) for CHROM
 *   - 32bit  (8 hex bytes) for POS
 *   - 32bit  (8 hex bytes) for REF_ALT Hash
 *
 * @param str   String buffer to be returned.
 * @param size  Size of the string buffer.
 * @param vh    Variant hash structure to be processed.
 *
 * @return      Upon successful return, these function returns the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
size_t variant_hash_string(char *str, size_t size, varhash_t vh);

/** @brief Parses a variant hash hex string and returns the components as varhash_t structure.
 *
 * @param vh Variant Hash string
 *
 * @return A varhash_t structure.
 */

varhash_t decode_variant_hash_string(const char *vs);

#endif  // VARIANTHASH_H
