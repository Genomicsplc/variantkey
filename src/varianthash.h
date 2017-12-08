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
//
// VariantHash by Nicola Asuni

/**
 * @file rsidvar.h
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to generate or decode
 * a genetic Variant Hash.
 *
 * A Genetic Variant Hash is composed of 3 sections that can be also used separately:
 *
 * CHROM   : 32 bits (8 hex bytes) to represent the chromosome.
 *           Chromosomes are always encoded as numbers.
 *           Non numerical human chromosomes (X, Y, XY, MT) are auomatically converted to numbers.
 *           For other species than human the string-to-number chromosome conversion should happen before calling the hashing function.
 * POS     : 32 bits (8 hex bytes) for the reference position (POS), with the 1st base having position 0.
 * REF_ALT : 64 bits (16 hex bytes) for the farmhash64 of the "REF_ALT" string.
 */

#ifndef VARIANTHASH_H
#define VARIANTHASH_H

#include "farmhash64.h"

/**
 * Contains the crom, pos and ref+alt encoded data.
 */
typedef struct varhash_t
{
    uint32_t chrom;  //!< Chromosome encoded number.
    uint32_t pos;    //!< Position. The reference position, with the 1st base having position 0.
    uint64_t refalt; //!< Hash code for Reference and Alternate
} varhash_t;

/** @brief Fast ASCII Uppercase function for a-z letters
 *
 * @param c character code to convert
 *
 * @return uppercase character code
 */
int aztoupper(int c);

/** @brief Returns 32-bit chromosome encoding.
 *
 * @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 *
 * @return Chrom encoding
 */
uint32_t encode_chrom(const char *chrom);

/** @brief Returns 64-bit reference+alternate hash code.
 *
 * @param ref   Reference base. Each base must be one of A,C,G,T,N (case insensitive). Multiple bases are permitted.
 *              The value in the POS field refers to the position of the first base in the String.
 * @param alt   Alternate base. Non-reference allele.
 *              Options are base Strings made up of the bases A,C,G,T,N,*, (case insensitive).
 *              The ‘*’ allele is reserved to indicate that the allele is missing due to a upstream deletion.
 *              If there are no alternative alleles, then the missing value should be used.
 *
 * @return 64-bit hash code
 */
uint64_t encode_ref_alt(const char *ref, const char *alt);

/** @brief Returns a Genetic Variant Hash based on CHROM, POS (0-base), REF, ALT.
 *
 * @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param pos   Position. The reference position, with the 1st base having position 0.
 * @param ref   Reference base. Each base must be one of A,C,G,T,N (case insensitive). Multiple bases are permitted.
 *              The value in the POS field refers to the position of the first base in the String.
 * @param alt   Alternate base. Non-reference allele.
 *              Options are base Strings made up of the bases A,C,G,T,N,*, (case insensitive).
 *              The ‘*’ allele is reserved to indicate that the allele is missing due to a upstream deletion.
 *              If there are no alternative alleles, then the missing value should be used.
 *
 * @return      A varhash_t structure.
 */
varhash_t variant_hash(const char *chrom, uint32_t pos, const char *ref, const char *alt);

/** @brief Returns a human-readable Genetic Variant Hash string (32 hex characters).
 *
 * The string represent a 128bit number or:
 *   - 32bit  (8 hex bytes) for CHROM
 *   - 32bit  (8 hex bytes) for POS
 *   - 64bit (16 hex bytes) for REF+ALT
 *
 * @param str   String buffer to be returned.
 * @param size  Size of the string buffer.
 * @param vh    Variant hash structure to be processed.
 *
 * @return      Upon successful return, these functions return the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
size_t variant_hash_string(char *str, size_t size, varhash_t vh);

/** @brief Parses a variant hash string and returns the components as varhash_t structure.
 *
 * @param vh Variant Hash string
 *
 * @return A varhash_t structure.
 */

varhash_t decode_variant_hash_string(const char *vs);

#endif  // VARIANTHASH_H
