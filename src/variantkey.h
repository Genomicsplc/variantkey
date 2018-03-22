// VariantKey
//
// variantkey.h
//
// @category   Tools
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
//
// VariantKey by Nicola Asuni

/**
 * @file variantkey.h
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to generate or decode
 * a genetic VariantKey128 and VariantKey64.
 */

#ifndef VARIANTKEY_H
#define VARIANTKEY_H

#include "farmhash64.h"

#define CHECK_BIT(var,pos) (((var)>>(pos)) & 1)

/**
 * VariantKey struct
 */
typedef struct variantkey_t
{
    uint8_t chrom;     //!< Chromosome encoded number (only the LSB 5 bit are used)
    uint32_t pos;      //!< Position. The reference position, with the 1st base having position 0 (only the LSB 28 bit are used)
    uint32_t refalt;   //!< Hash code for Reference and Alternate (only the LSB 31 bits are used)
} variantkey_t;

/** @brief Fast ASCII Uppercase function for a-z letters
 *
 * @param c character code to convert
 *
 * @return uppercase character code
 */
int aztoupper(int c);

/** @brief Returns chromosome encoding.
 *
 * @param chrom  Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param size   Length of the chrom string, excluding the terminating null byte.
 *
 * @return Chrom encoding
 */
uint8_t encode_chrom(const char *chrom, size_t size);

/** @brief Decode the CHROM code.
 *
 * @param code   CHROM code.
 * @param chrom  CHROM string buffer to be returned. Its size should be enough to contain the results (max 4 bytes).
 *
 * @return If successful, the total number of characters written is returned excluding the null-character appended
 *         at the end of the string, otherwise a negative number is returned in case of failure.
 */
size_t decode_chrom(uint8_t code, char *chrom);

/** @brief Returns reference+alternate encoding.
 *
 * @param ref      Reference allele. String containing a sequence of nucleotide letters.
 *                 The value in the pos field refers to the position of the first nucleotide in the String.
 * @param sizeref  Length of the ref string, excluding the terminating null byte.
 * @param alt      Alternate non-reference allele string.
 * @param sizealt  Length of the alt string, excluding the terminating null byte.
 *
 * @return Ref+Alt code
 */
uint32_t encode_refalt(const char *ref, size_t sizeref, const char *alt, size_t sizealt);

/** @brief Decode the 32 bit REF+ALT code if reversible (if it has less than 5 nucleotides in total).
 *
 * @param code     REF+ALT code
 * @param ref      REF string buffer to be returned.
 * @param sizeref  Pointer to the size of the ref buffer, excluding the terminating null byte.
 *                 This will contain the final ref size.
 * @param alt      ALT string buffer to be returned.
 * @param sizealt  Pointer to the size of the alt buffer, excluding the terminating null byte.
 *                 This will contain the final alt size.
 *
 * @return      If the code is reversible (less than 6 nucleotides in total), then
 *              the total number of characters of REF+ALT is returned.
 *              Otherwise 0 is returned.
 */
size_t decode_refalt(uint32_t code, char *ref, size_t *sizeref, char *alt, size_t *sizealt);

/** @brief Returns a 64 bit variant key based on CHROM, POS (0-base), REF, ALT.
 *
 * @param chrom    Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param size     Length of the chrom string, excluding the terminating null byte.
 * @param pos      Position. The reference position, with the 1st base having position 0.
 * @param ref      Reference allele. String containing a sequence of nucleotide letters.
 *                 The value in the pos field refers to the position of the first nucleotide in the String.
 * @param sizeref  Length of the ref string, excluding the terminating null byte.
 * @param alt      Alternate non-reference allele string.
 * @param sizealt  Length of the alt string, excluding the terminating null byte.
 *
 * @return      VariantKey 64 bit code.
 */
uint64_t variantkey(const char *chrom, size_t sizechrom, uint32_t pos, const char *ref, size_t sizeref, const char *alt, size_t sizealt);

/** @brief Returns VariantKey hexadecimal string (16 characters).
 *
 * The string represent a 64 bit number or:
 *   -  5 bit for CHROM
 *   - 28 bit for POS
 *   - 31 bit for REF+ALT
 *
 * @param code  VariantKey code.
 * @param str   String buffer to be returned.
 * @param size  Size of the string buffer.
 *
 * @return      Upon successful return, these function returns the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
size_t variantkey_string(uint64_t code, char *str, size_t size);

/** @brief Parses a VariantKey hex string and returns the code.
 *
 * @param vs    VariantKey hexadecimal string (it must contain 16 hexadecimal characters).
 *
 * @return A VariantKey code
 */
uint64_t parse_variantkey_string(const char *vs);

/** @brief Decode a VariantKey code and returns the components as variantkey_t structure.
 *
 * @param code VariantKey code.
 *
 * @return A variantkey_t structure.
 */
variantkey_t decode_variantkey(uint64_t code);

#endif  // VARIANTKEY_H
